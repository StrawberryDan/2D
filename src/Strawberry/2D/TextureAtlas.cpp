#include "TextureAtlas.hpp"

#include "TextureManifest.hpp"
#include "Strawberry/Vulkan/Queue/CommandBuffer.hpp"


namespace Strawberry::TwoD
{
	TextureAtlas::TextureAtlas(
		Vulkan::Queue& queue,
		const Core::Math::Vec2u& pageSize,
		unsigned int arrayLength)
			: mCommandPool(queue)
			, mGPUImage(Vulkan::Image::Builder(queue.GetDevice(), Vulkan::MemoryTypeCriteria::DeviceLocal())
				.WithFormat(VK_FORMAT_R8G8B8A8_SRGB)
				.WithExtent(pageSize)
				.WithArrayLayers(arrayLength)
				.WithUsage(VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT)
				.Build())
	{
		for (unsigned int i = 0; i < arrayLength; i++)
		{
			mBoxTree.emplace(i, BoxTree(Core::Math::AABB<unsigned int, 2>::FromOriginAndExtent(
				{0, 0}, pageSize)
			));
		}
	}


	void TextureAtlas::Register(const TextureManifest& textureManifest)
	{
		for (const auto& entry : textureManifest.Entries())
		{
			Register(entry.handle, entry.resource.Visit(Core::Overload(
				[](const std::filesystem::path& path) { return Core::Image<Core::PixelRGBA>::FromFile(path).Unwrap(); }
			)));
		}
	}


	TextureAtlas::Key TextureAtlas::Register(const std::string& handle, Core::Image<Core::PixelRGBA>&& texture)
	{
		Key key = Register(std::move(texture));
		mHandleMapping.emplace(handle, key);
		return key;
	}


	TextureAtlas::Key TextureAtlas::Register(Core::Image<Core::PixelRGBA>&& texture)
	{
		const size_t DATA_SIZE = texture.PixelSize() * texture.Size().Fold(std::multiplies{});
		Vulkan::Buffer stagingBuffer = Vulkan::Buffer::Builder(mCommandPool.GetQueue()->GetDevice(), Vulkan::MemoryTypeCriteria::HostVisible())
			.WithSize(DATA_SIZE)
			.WithUsage(VK_IMAGE_USAGE_TRANSFER_SRC_BIT)
			.Build();
		stagingBuffer.SetData({texture.Data(), DATA_SIZE});

		Key nextKey = (Key) (mEntries.size() + mPendingEntries.size());
		Core::Optional<Entry> entry = FindRectangleForTexture(texture.Size());
		mPendingEntries.emplace_back(nextKey,
			std::move(entry
				.Expect("Unable to find space in gpu texture map for image!")),
			std::move(stagingBuffer));
		return nextKey;
	}


	void TextureAtlas::Flush() const
	{
		Vulkan::CommandBuffer commandBuffer(mCommandPool);

		commandBuffer.Begin(true);
		commandBuffer.PipelineBarrier(VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, {
			Vulkan::ImageMemoryBarrier(mGPUImage, VK_IMAGE_ASPECT_COLOR_BIT)
				.WithSrcAccessMask(VK_ACCESS_SHADER_READ_BIT)
				.WithDstAccessMask(VK_ACCESS_TRANSFER_WRITE_BIT)
				.FromLayout(mEntries.empty() ? VK_IMAGE_LAYOUT_UNDEFINED : VK_IMAGE_LAYOUT_GENERAL)
				.ToLayout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
		});
		for (const auto& entry : mPendingEntries)
		{
			commandBuffer.CopyBufferToImage(Vulkan::CommandCopyBufferToImage()
				.WithSrcBuffer(entry.imageData)
				.WithDstImage(mGPUImage)
				.WithDstArrayLayer(entry.entry.arrayLayer)
				.WithDstOffset(entry.entry.region.Min().AppendedWith(0))
				.WithDstExtent(entry.entry.region.Size().AppendedWith(1)));
			mEntries.emplace(entry.key, entry.entry);
		}
		commandBuffer.PipelineBarrier(VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, {
			Vulkan::ImageMemoryBarrier(mGPUImage, VK_IMAGE_ASPECT_COLOR_BIT)
				.WithSrcAccessMask(VK_ACCESS_TRANSFER_WRITE_BIT)
				.WithDstAccessMask(VK_ACCESS_SHADER_READ_BIT)
				.FromLayout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
				.ToLayout(VK_IMAGE_LAYOUT_GENERAL)
		});
		commandBuffer.End();
		commandBuffer.Submit();
		commandBuffer.Wait();
		mPendingEntries.clear();
	}


	void TextureAtlas::Reset()
	{
		mEntries.clear();
		mPendingEntries.clear();
		mHandleMapping.clear();
		for (auto& [page, boxTree] : mBoxTree)
		{
			boxTree.tree = Core::Math::NAryTree<Core::Math::AABB<unsigned int, 2>, 4>({{0, 0}, mGPUImage.GetSize().AsSize<2>()});
			boxTree.usedNodes.clear();
		}
	}


	TextureReference TextureAtlas::GetTextureReference(const Key& key) const
	{
		if (auto search = mEntries.find(key); search == mEntries.end())
		{
			if (!mPendingEntries.empty())
			{
				Flush();
				return GetTextureReference(key);
			}
			else
			{
				Core::Unreachable();
			}
		}
		else
		{
			const auto& entry = search->second;
			TextureReference reference(mGPUImage, entry.region, entry.arrayLayer);
			return reference;
		}
	}


	TextureReference TextureAtlas::GetTextureReference(const Handle& handle) const
	{
		return GetTextureReference(mHandleMapping.at(handle));
	}


	Core::Optional<TextureAtlas::Entry> TextureAtlas::FindRectangleForTexture(Core::Math::Vec2u size)
	{
		// Check each page
		for (unsigned int pageIndex = 0; pageIndex < mGPUImage.GetArrayLayerCount(); pageIndex++)
		{
			// Get page
			BoxTree& pageTree = mBoxTree.at(pageIndex);
			// Queue for iteration
			std::vector queue { pageTree.tree.Root() };
			// Iterate over queue in depth first order
			while (!queue.empty())
			{
				// Pop node from back
				auto currentNode = queue.back();
				queue.pop_back();

				// Get the box and its size
				Core::Math::AABB<unsigned int, 2> box = pageTree.tree.GetValue(currentNode);
				auto boxSize = box.Size();

				// Checks if this image can be stored in a smaller box.
				const bool tooBig   = !(size[0] <= boxSize[0] && size[1] <= boxSize[1]);
				// If this tree has no children, it is an potentially available space.
				if (pageTree.tree.GetChildCount(currentNode) == 0 && !pageTree.usedNodes.contains(currentNode))
				{
					// Checks if this image is too big to be stored in this box
					const bool tooSmall = !(2 * size[0] > boxSize[0] || 2 * size[1] > boxSize[1]);
					// If it's a good fit then record it.
					if (!tooBig && !tooSmall)
					{
						// Record that this node is full.
						pageTree.usedNodes.emplace(currentNode);
						// Make entry;
						Entry entry;
						entry.cellSize   = box.Size()[0];
						entry.region     = Core::Math::AABB<unsigned int, 2>::FromOriginAndExtent(box.Min(), size);
						entry.arrayLayer = pageIndex;
						// Return entry.
						return entry;
					}
					// If the image is too small, split this box into 4 sub boxes.
					if (tooSmall)
					{
						// Make child box in top left corner by copying and moving max to center.
						Core::Math::AABB<unsigned int, 2> childBox = box;
						childBox.SetMax(childBox.Center());

						// Split in CW order, which means they are accessed in CCW order;
						Core::Math::Vec2u offsets[] {
							{1, 0}, {1, 1}, {0, 1}, {0, 0} };

						for (int i = 0; i < 4; i++)
						{
							// Copy child box and adjust by offset.
							auto copy = childBox;
							copy.SetMin(childBox.Min() + offsets[i].Piecewise(std::multiplies{}, childBox.Size()));
							copy.SetMax(childBox.Max() + offsets[i].Piecewise(std::multiplies{}, childBox.Size()));
							// Add child to tree and queue;
							Key childNode = pageTree.tree.AddNode(currentNode, copy);
							queue.emplace_back(childNode);
						}
					}
				}
				else if (!tooBig)
				{
					// Check the children;
					auto children = pageTree.tree.GetChildren(currentNode).AsVector();
					for (const auto& n : children)
					{
						queue.emplace_back(n);
					}
				}
			}
		}

		return Core::NullOpt;
	}
}

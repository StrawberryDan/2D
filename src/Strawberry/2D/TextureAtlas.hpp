#pragma once


#include "TextureReference.hpp"
#include "Strawberry/Core/Math/Geometry/AABB.hpp"
#include "Strawberry/Core/Math/Graph/Tree.hpp"
#include "Strawberry/Core/Util/Image.hpp"
#include "Strawberry/Vulkan/Resource/Buffer.hpp"
#include "Strawberry/Vulkan/Resource/Image.hpp"


namespace Strawberry::TwoD
{
	class TextureManifest;


	class TextureAtlas
	{
	public:
		using Handle = std::string;
		using Key    = uint32_t;


		TextureAtlas(Vulkan::Queue& queue, const Core::Math::Vec2u& pageSize, unsigned int arrayLength = 1);
		TextureAtlas(const TextureAtlas&) = delete;
		TextureAtlas& operator=(const TextureAtlas&) = delete;
		TextureAtlas(TextureAtlas&&) = default;
		TextureAtlas& operator=(TextureAtlas&&) = default;
		virtual ~TextureAtlas() = default;


		void Register(const TextureManifest& textureManifest);
		Key  Register(const std::string& handle, Core::Image<Core::PixelRGBA>&& texture);
		virtual Key Register(Core::Image<Core::PixelRGBA>&& texture) = 0;


		void Flush() const;
		void Reset();


		[[nodiscard]] TextureReference GetTextureReference(const Key& key) const;
		[[nodiscard]] TextureReference GetTextureReference(const Handle& handle) const;


	protected:
		static constexpr unsigned int MIN_BOX_SIZE = 8;


		struct Entry
		{
			unsigned int                      cellSize;
			Core::Math::AABB<unsigned int, 2> region;
			unsigned int                      arrayLayer;
		};


		struct PendingEntry
		{
			Key            key;
			Entry          entry;
			Vulkan::Buffer imageData;
		};


		struct BoxTree
		{
			Core::Math::NAryTree<Core::Math::AABB<unsigned int, 2>, 4> tree;
			std::set<decltype(tree)::Config::NodeID>                   usedNodes;
		};


		Core::Optional<Entry> FindRectangleForTexture(Core::Math::Vec2u size);


		virtual Vulkan::Image& GetImage() const = 0;


		std::map<unsigned int, BoxTree>         mBoxTree;
		mutable Vulkan::CommandPool             mCommandPool;
		mutable std::map<Key, Entry>            mEntries;
		mutable std::vector<PendingEntry>       mPendingEntries;
		std::unordered_map<Handle, Key>         mHandleMapping;
	};


	class DiffuseTextureAtlas
		: public TextureAtlas
	{
	public:
		DiffuseTextureAtlas(Vulkan::Queue& queue, const Core::Math::Vec2u& pageSize, unsigned int arrayLength = 1);


		using TextureAtlas::Register;
		Key  Register(Core::Image<Core::PixelRGBA>&& texture) override;

	protected:
		Vulkan::Image& GetImage() const override;

	private:
		mutable Vulkan::Image mGPUImage;
	};


	class NormalTextureAtlas
		: public TextureAtlas
	{
	public:
		NormalTextureAtlas(Vulkan::Queue& queue, const Core::Math::Vec2u& pageSize, unsigned int arrayLength = 1);


		using TextureAtlas::Register;
		Key Register(Core::Image<Core::PixelRGBA>&& texture) override;

	protected:
		Vulkan::Image& GetImage() const override;

	private:
		mutable Vulkan::Image mGPUImage;
	};
}

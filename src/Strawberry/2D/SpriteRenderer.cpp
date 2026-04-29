#include "SpriteRenderer.hpp"

#include "NormalSprite.hpp"
#include "Sprite.hpp"
#include "Strawberry/Core/Math/Transformations.hpp"
#include "Strawberry/Vulkan/Descriptor/Sampler.hpp"
#include "Strawberry/Vulkan/Math/Projection.hpp"
#include "Strawberry/Vulkan/Resource/Buffer.hpp"


static uint8_t VERTEX_SHADER[] {
#include "SpriteRenderer.vert.bin"
};


static uint8_t FRAGMENT_SHADER[] {
#include "SpriteRenderer.frag.bin"
};


namespace Strawberry::TwoD
{
	static constexpr size_t DRAW_CONSTANTS_BUFFER_SIZE = sizeof(Core::Math::Mat4f);


	SpriteRenderer::SpriteRenderer(Vulkan::Framebuffer& frameBuffer, uint32_t subpassIndex)
		: mPipelineLayout(Vulkan::PipelineLayout::Builder(frameBuffer.GetDevice())
			.WithDescriptor(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
			.WithDescriptor(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
			.Build())
		, mPipeline(Vulkan::GraphicsPipeline::Builder(mPipelineLayout, frameBuffer.GetRenderPass(), subpassIndex)
			.WithShaderStages(
				Vulkan::Shader::Compile(frameBuffer.GetDevice(), VERTEX_SHADER).Unwrap(),
				Vulkan::Shader::Compile(frameBuffer.GetDevice(), FRAGMENT_SHADER).Unwrap())
			.WithInputAssembly(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
			.WithInputBinding(0, VK_VERTEX_INPUT_RATE_INSTANCE)
			.WithInputAttribute(0, 0, sizeof(Core::Math::Vec3f), VK_FORMAT_R32G32B32_SFLOAT)
			.WithInputAttribute(1, 0, sizeof(Core::Math::Vec3f), VK_FORMAT_R32G32B32_SFLOAT)
			.WithInputAttribute(2, 0, sizeof(Core::Math::Vec2f), VK_FORMAT_R32G32_SFLOAT)
			.WithInputAttribute(3, 0, sizeof(Core::Math::Vec2f), VK_FORMAT_R32G32_SFLOAT)
			.WithInputAttribute(4, 0, sizeof(uint32_t), VK_FORMAT_R32_UINT)
			.WithRasterization(VK_POLYGON_MODE_FILL, VK_CULL_MODE_NONE, VK_FRONT_FACE_COUNTER_CLOCKWISE)
			.WithAlphaColorBlending()
			.WithViewport(frameBuffer)
			.Build())
		, mSampler(Vulkan::Sampler(frameBuffer.GetDevice(), VK_FILTER_NEAREST, VK_FILTER_NEAREST))
		, mDrawConstantsDescriptorSet(Vulkan::DescriptorSet::Allocate(frameBuffer.GetDevice(), mPipelineLayout.GetSetLayout(0)).Unwrap())
		, mDrawConstantsBuffer(Vulkan::Buffer::Builder(frameBuffer.GetDevice(), Vulkan::MemoryTypeCriteria::HostVisible())
			.WithSize(DRAW_CONSTANTS_BUFFER_SIZE)
			.WithUsage(VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT)
			.Build())
		, mProjectionMatrix(
			Core::Math::Scale(
				frameBuffer.GetSize()
					.Map([] (auto x) { return 1.0 / x; })
					.AppendedWith(1.0f, 1.0f)
					.AsType<float>()))
	{
		SetProjectionMatrix(mProjectionMatrix);
		mDrawConstantsDescriptorSet.SetUniformBuffer(0, 0, mDrawConstantsBuffer);
	}


	const Core::Math::Mat4f SpriteRenderer::GetProjectionMatrix() const noexcept
	{
		return mProjectionMatrix;
	}


	void SpriteRenderer::SetProjectionMatrix(const Core::Math::Mat4f& projectionMatrix)
	{
		mProjectionMatrix = projectionMatrix;
		mDrawConstantsBuffer.SetData(Core::IO::DynamicByteBuffer::FromObjects(mProjectionMatrix));
	}


	std::vector<Vulkan::Batch> SpriteRenderer::MakeBatch(const Sprite& sprite)
	{
		std::vector<Vulkan::Batch> batch;

		if (auto handle = sprite.GetTexture().Image()->GetHandle(); !mTextureAtlasDescriptorSets.contains(handle)) [[unlikely]]
		{
			mTextureAtlasViews.emplace(
				handle,
				Vulkan::ImageView::Builder(*sprite.GetTexture().Image(), VK_IMAGE_ASPECT_COLOR_BIT)
					.WithFormat(VK_FORMAT_R8G8B8A8_SRGB)
					.WithType(VK_IMAGE_VIEW_TYPE_2D_ARRAY)
					.Build());
			mTextureAtlasDescriptorSets.emplace(
				handle,
				mPipeline.CreateDescriptorSet(1).Unwrap());
			mTextureAtlasDescriptorSets.at(handle).SetCombinedImageSampler(0, 0, mSampler, mTextureAtlasViews.at(handle), VK_IMAGE_LAYOUT_GENERAL);
		}

		batch.emplace_back(Vulkan::Batch(mPipeline)
			.WithVertexCount(6)
			.WithDescriptorSet(0, &mDrawConstantsDescriptorSet)
			.WithDescriptorSet(1, mTextureAtlasDescriptorSets.at(sprite.GetTexture().Image()->GetHandle()))
			.WithVertexBuffer(0,
				Vulkan::Buffer::Builder(mPipeline.GetDevice(), Vulkan::MemoryTypeCriteria::HostVisible())
					.WithUsage(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT)
					.WithData(Core::IO::DynamicByteBuffer::FromObjects(
						(mScale * sprite.GetScale() * sprite.GetPosition().AsSize<2>()).AppendedWith(sprite.GetPosition()[2]),
						(mScale * sprite.GetScale() * sprite.GetExtent()).AppendedWith(sprite.GetRotation()),
						sprite.GetTextureMin(),
						sprite.GetTextureMax(),
						sprite.GetTexturePage()))
					.Build()));

		return batch;
	}


	float SpriteRenderer::GetScale() const
	{
		return mScale;
	}


	void SpriteRenderer::SetScale(float scale)
	{
		mScale = scale;
	}
}

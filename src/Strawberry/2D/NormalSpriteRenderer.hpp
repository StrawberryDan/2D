#pragma once
#include "Strawberry/Core/Math/Matrix.hpp"
#include "Strawberry/Vulkan/Descriptor/Sampler.hpp"
#include "Strawberry/Vulkan/Pipeline/GraphicsPipeline.hpp"
#include "Strawberry/Vulkan/Queue/Batch.hpp"
#include "Strawberry/Vulkan/Resource/Buffer.hpp"


namespace Strawberry::TwoD
{
	class NormalSprite;

	class NormalSpriteRenderer
	{
	public:
		NormalSpriteRenderer(Vulkan::Framebuffer& frameBuffer, uint32_t subpassIndex);


		[[nodiscard]] const Core::Math::Mat4f GetProjectionMatrix() const noexcept;
		void SetProjectionMatrix(const Core::Math::Mat4f& projectionMatrix);


		std::vector<Vulkan::Batch> MakeBatch(const NormalSprite& sprite);


		float GetScale() const;
		void SetScale(float scale);


	private:
		float mScale = 1.0f;

		Vulkan::PipelineLayout   mPipelineLayout;
		Vulkan::GraphicsPipeline mPipeline;


		Vulkan::DescriptorSet    mDrawConstantsDescriptorSet;
		Vulkan::Buffer           mDrawConstantsBuffer;
		Core::Math::Mat4f        mProjectionMatrix;

		Vulkan::Sampler mSampler;
		std::map<Vulkan::Image::Handle, Vulkan::DescriptorSet> mTextureAtlasDescriptorSets;
		std::map<Vulkan::Image::Handle, Vulkan::ImageView> mTextureAtlasViews;
	};
}

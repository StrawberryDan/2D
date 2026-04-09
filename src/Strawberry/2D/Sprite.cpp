#include "Sprite.hpp"


namespace Strawberry::TwoD
{
	Sprite::Sprite(const TextureReference& texture)
		: mDiffuseTextureReference(texture)
	{
		mScreenTransform.SetExtent(texture.Region().Size().AsType<float>());
	}


	Core::Math::Vec3f Sprite::GetPosition() const
	{
		return mScreenTransform.GetPosition();
	}


	void Sprite::SetPosition(const Core::Math::Vec3f& position)
	{
		mScreenTransform.SetPosition(position);
	}


	Core::Math::Vec2f Sprite::GetExtent() const
	{
		return mScreenTransform.GetExtent();
	}


	float Sprite::GetRotation() const
	{
		return mScreenTransform.GetRotation();
	}


	void Sprite::SetRotation(Core::Math::Radians radians)
	{
		mScreenTransform.SetRotation(radians);
	}


	Core::Math::Vec2f Sprite::GetTextureMin() const
	{
		return mDiffuseTextureReference.Region().Min()
			.Piecewise(std::divides{}, mDiffuseTextureReference.Image()->GetSize().AsSize<2>().AsType<float>());
	}
	Core::Math::Vec2f Sprite::GetTextureMax() const
	{
		return mDiffuseTextureReference.Region().Max()
			.Piecewise(std::divides{}, mDiffuseTextureReference.Image()->GetSize().AsSize<2>().AsType<float>());
	}


	uint32_t Sprite::GetTexturePage() const
	{
		return mDiffuseTextureReference.ArrayIndex();
	}


	float Sprite::GetScale() const
	{
		return mScale;
	}


	void Sprite::SetScale(float scale)
	{
		mScale = scale;
	}


	bool Sprite::GetFlipX() const
	{
		return mScreenTransform.GetFlipX();
	}


	void Sprite::SetFlipX(bool flipX)
	{
		mScreenTransform.SetFlipX(flipX);
	}


	const TextureReference& Sprite::GetTexture() const
	{
		return mDiffuseTextureReference;
	}
}

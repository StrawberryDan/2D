#include "Strawberry/2D/SpriteTransform.hpp"


namespace Strawberry::TwoD
{
	const Core::Math::Vec3f SpriteTransform::GetPosition() const
	{
		return mPosition;
	}


	void SpriteTransform::SetPosition(Core::Math::Vec3f position)
	{
		mPosition = position;
	}


	const Core::Math::Vec2f SpriteTransform::GetExtent() const
	{
		return mExtent;
	}


	void SpriteTransform::SetExtent(const Core::Math::Vec2f& extent)
	{
		mExtent = extent;
	}


	Core::Math::Radians SpriteTransform::GetRotation() const
	{
		return mRotation;
	}


	void SpriteTransform::SetRotation(Core::Math::Radians radians)
	{
		mRotation = radians;
	}


	bool SpriteTransform::GetFlipX() const
	{
		return mFlipX;
	}

	void SpriteTransform::SetFlipX(bool flip)
	{
		mFlipX = flip;
	}
}

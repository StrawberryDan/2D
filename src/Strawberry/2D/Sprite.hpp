#pragma once


#include "TextureReference.hpp"
#include "SpriteTransform.hpp"


namespace Strawberry::TwoD
{
	class Sprite
	{
	public:

	private:
		TextureReference mDiffuseTextureReference;
		SpriteTransform  mScreenTransform;
	};
}

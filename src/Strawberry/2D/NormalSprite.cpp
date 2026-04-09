#include "NormalSprite.hpp"


namespace Strawberry::TwoD
{
	NormalSprite::NormalSprite(const TextureReference& diffuseTexture, const TextureReference& normalTexture)
		: Sprite(diffuseTexture)
		, mNormalTextureReference(normalTexture)
	{}


	const TextureReference& NormalSprite::GetNormalTexture() const
	{
		return mNormalTextureReference;
	}
}

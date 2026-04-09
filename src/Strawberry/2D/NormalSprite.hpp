#pragma once


#include "Sprite.hpp"


namespace Strawberry::TwoD
{
	class NormalSprite
		: public Sprite
	{
	public:
		NormalSprite(const TextureReference& diffuseTexture, const TextureReference& normalTexture);


		const TextureReference& GetNormalTexture() const;


	private:
		TextureReference mNormalTextureReference;
	};
}
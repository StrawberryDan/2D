#pragma once


#include "TextureReference.hpp"
#include "SpriteTransform.hpp"


namespace Strawberry::TwoD
{
	class Sprite
	{
	public:
		Sprite(const TextureReference& texture);

		[[nodiscard]] Core::Math::Vec3f GetPosition() const;
		void SetPosition(const Core::Math::Vec3f& position);
		[[nodiscard]] Core::Math::Vec2f GetExtent() const;
		[[nodiscard]] float             GetRotation() const;
		void                            SetRotation(Core::Math::Radians radians);
		[[nodiscard]] Core::Math::Vec2f GetTextureMin() const;
		[[nodiscard]] Core::Math::Vec2f GetTextureMax() const;
		[[nodiscard]] uint32_t          GetTexturePage() const;
		[[nodiscard]] float             GetScale() const;
		void                            SetScale(float scale);
		bool                            GetFlipX() const;
		void                            SetFlipX(bool flipX);


		const TextureReference& GetTexture() const;

	private:
		TextureReference mDiffuseTextureReference;
		SpriteTransform  mScreenTransform;
		float            mScale = 1.0f;
	};
}

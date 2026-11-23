#pragma once
#include "Strawberry/Core/Math/Vector.hpp"


namespace Strawberry::TwoD
{
    class SpriteTransform
    {
    public:
        const Core::Math::Vec3f GetPosition() const;
        void SetPosition(Core::Math::Vec3f position);

        const Core::Math::Vec2f GetExtent() const;
        void SetExtent(const Core::Math::Vec2f& extent);


        Core::Math::Radians GetRotation() const;
        void SetRotation(Core::Math::Radians radians);


    private:
        Core::Math::Vec3f mPosition;
        Core::Math::Vec2f mExtent;
        Core::Math::Radians mRotation = 0.0f;
    };
}

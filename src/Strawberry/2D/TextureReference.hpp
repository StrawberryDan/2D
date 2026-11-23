#pragma once
#include "Strawberry/Core/Math/Geometry/AABB.hpp"
#include "Strawberry/Core/Types/ReflexivePointer.hpp"
#include "Strawberry/Vulkan/Resource/Image.hpp"


namespace Strawberry::TwoD
{
	class TextureReference
	{
	public:
		TextureReference(
			const Vulkan::Image& image,
			const Core::Math::Geometry::AABB<unsigned int, 2>& region,
			unsigned int arrayIndex = 0);


		void RotateCCW();


		const auto& Image() const { return mImage; }
		const auto& Region() const { return mRegion; }
		unsigned int ArrayIndex() const { return mArrayIndex; }

	private:
		Core::ReflexivePointer<Vulkan::Image> mImage;
		Core::Math::Geometry::AABB<unsigned int, 2>     mRegion;
		unsigned int                          mArrayIndex = 0;
	};
}

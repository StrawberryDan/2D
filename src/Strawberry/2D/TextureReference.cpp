#include "TextureReference.hpp"


namespace Strawberry::TwoD
{
	TextureReference::TextureReference(
		const Vulkan::Image& image,
		const Core::Math::Geometry::AABB<unsigned int, 2>& region,
		unsigned int arrayIndex)
			: mImage(image.GetReflexivePointer())
			, mRegion(region)
			, mArrayIndex(arrayIndex)
	{}
}

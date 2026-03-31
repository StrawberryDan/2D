#pragma once

#include "Strawberry/Core/Types/Variant.hpp"
#include <filesystem>
#include <random>


namespace Strawberry::TwoD
{
	class TextureManifest
	{
	public:
		using Resource = Core::Variant<std::filesystem::path>;
		using Handle = std::string;

		struct Entry
		{
			Handle   handle;
			Resource resource;
		};


		void AddEntry(const Handle& handle, Resource&& resource)
		{
			mEntries.emplace_back(Entry{.handle = handle, .resource = std::move(resource)});
		}


		auto Entries() const
		{
			return std::views::all(mEntries);
		}
	private:
		std::vector<Entry> mEntries;
	};
}

#include "gbcpch.h"
#include "FileTypes.h"

namespace gbc
{
	Scope<FilterGenerator> FilterGenerator::generator = nullptr;

	FilterGenerator& FilterGenerator::Get()
	{
		if (!generator)
			generator = CreateScope<FilterGenerator>();
		return *generator;
	}

	FilterGenerator::FilterGenerator()
	{
		// Filters
		CreateFilter(L"EZ80", FileType::ez80);
	}

	FilterGenerator::~FilterGenerator()
	{
		for (auto [extension0, filter] : filters)
			delete[] filter;
	}

	void FilterGenerator::CreateFilter(std::wstring_view fileTypeName, std::wstring_view extension) noexcept
	{
		wchar_t* filter = new wchar_t[fileTypeName.size() + 7 + extension.size() * 2 + 1];

		std::memcpy(filter, fileTypeName.data(), fileTypeName.size() * sizeof(wchar_t));
		size_t offset = fileTypeName.size();
		filter[offset] = L' ';
		filter[++offset] = L'(';
		filter[++offset] = L'*';
		std::memcpy(filter + ++offset, extension.data(), extension.size() * sizeof(wchar_t));
		filter[offset += extension.size()] = L')';
		filter[++offset] = L'\0';

		filter[++offset] = L'*';
		std::memcpy(filter + ++offset, extension.data(), extension.size() * sizeof(wchar_t));
		filter[offset += extension.size()] = L'\0';
		filter[++offset] = L'\0';

		filters[extension] = filter;
	}

	const wchar_t* FilterGenerator::GetFilter(std::wstring_view extension) noexcept
	{
		if (auto it = filters.find(extension); it != filters.end())
			return it->second;
		return nullptr;
	}

	const wchar_t* GetFilter(std::wstring_view extension) noexcept
	{
		return FilterGenerator::Get().GetFilter(extension);
	}

	std::filesystem::path AppendType(const std::filesystem::path& filepath, std::wstring_view extension)
	{
		return filepath.native() + extension.data();
	}

	std::filesystem::path& AppendTypeTo(std::filesystem::path& filepath, std::wstring_view extension)
	{
		return filepath = filepath.native() + extension.data();
	}

	// Simply those alias' definitions
#define FILE_EXTENSION_DEFINITION(name, exts) \
	std::filesystem::path Append##name##Type(const std::filesystem::path& filepath) { return AppendType(filepath, exts); } \
	std::filesystem::path& Append##name##TypeTo(std::filesystem::path& filepath) { return AppendTypeTo(filepath, exts); } \
	const wchar_t* Get##name##Filter() noexcept { return GetFilter(exts); }

	// Generic extensions
	FILE_EXTENSION_DEFINITION(EZ80, FileType::ez80);
}

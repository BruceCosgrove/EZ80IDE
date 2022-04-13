#pragma once

#include "GBC/Core/Core.h"
#include <array>
#include <filesystem>

namespace gbc
{
	// Converts an std::array<const wchar_t*, Size> to a an std::array<std::wstring_view, Size> at compile time
	template<size_t Size>
	static constexpr std::array<std::wstring_view, Size> WideCharsToWideStringViews(std::wstring_view(&& array)[Size])
	{ return std::to_array(array); }

	namespace FileType
	{
		static constexpr auto ez80 = WideCharsToWideStringViews({ L".ez80", L".asm", L".inc" });
	}

	const wchar_t* GetFilter(std::wstring_view extension) noexcept;
	template<size_t Size> const wchar_t* GetFilter(const std::array<std::wstring_view, Size>& extensions) noexcept;

	constexpr bool IsFile(std::wstring_view filepath, std::wstring_view extension) noexcept;
	constexpr bool IsFilepath(const std::filesystem::path& filepath, std::wstring_view extension) noexcept;
	std::filesystem::path AppendType(const std::filesystem::path& filepath, std::wstring_view extension);
	std::filesystem::path& AppendTypeTo(std::filesystem::path& filepath, std::wstring_view extension);

	template<size_t Size> constexpr bool IsFile(std::wstring_view filepath, const std::array<std::wstring_view, Size>& extensions) noexcept;
	template<size_t Size> constexpr bool IsFilepath(const std::filesystem::path& filepath, const std::array<std::wstring_view, Size>& extensions) noexcept;
	template<size_t Size> std::filesystem::path AppendType(const std::filesystem::path& filepath, const std::array<std::wstring_view, Size>& extensions);
	template<size_t Size> std::filesystem::path& AppendTypeTo(std::filesystem::path& filepath, const std::array<std::wstring_view, Size>& extensions);
}

#include "FileTypes.inl"

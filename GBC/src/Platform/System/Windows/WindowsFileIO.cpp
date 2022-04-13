#include "gbcpch.h"
#include "GBC/IO/FileIO.h"
#include <shellapi.h>

namespace gbc::FileIO
{
	bool IsHidden(const std::filesystem::path& filepath)
	{
		std::error_code error;
		if (!std::filesystem::exists(filepath, error))
			return false;

		LPCWSTR lpFileName = filepath.native().c_str();
		DWORD existingAttributes = GetFileAttributesW(lpFileName);

		if (existingAttributes == INVALID_FILE_ATTRIBUTES) return false;
		return !!(existingAttributes & FILE_ATTRIBUTE_HIDDEN);
	}

	bool MakeHiddenDirectory(const std::filesystem::path& filepath)
	{
		std::error_code error;
		if (!std::filesystem::exists(filepath, error))
		{
			if (error) return false;
			std::filesystem::create_directory(filepath, error);
			if (error) return false;
		}

		LPCWSTR lpFileName = filepath.native().c_str();
		DWORD existingAttributes = GetFileAttributesW(lpFileName);

		if (existingAttributes == INVALID_FILE_ATTRIBUTES) return false;
		if (existingAttributes & FILE_ATTRIBUTE_HIDDEN) return true;

		return !!SetFileAttributesW(lpFileName, existingAttributes | FILE_ATTRIBUTE_HIDDEN);
	}
}

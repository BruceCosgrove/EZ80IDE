#pragma once

#include <cstdint>
#include <filesystem>
#include <string_view>
#include <vector>

namespace gbc::FileIO
{
	std::string ReadFile(const std::filesystem::path& filepath);
	std::vector<uint8_t> ReadBinaryFile(const std::filesystem::path& filepath);
	bool WriteFile(const std::filesystem::path& filepath, std::string_view contents);
	bool WriteBinaryFile(const std::filesystem::path& filepath, const std::vector<uint8_t>& contents);
	bool WriteBinaryFile(const std::filesystem::path& filepath, const void* contents, size_t size);

	bool FileExists(const std::filesystem::path& filepath) noexcept;
	bool DirectoryExists(const std::filesystem::path& filepath) noexcept;

	bool IsEmptyDirectory(const std::filesystem::path& filepath);
	bool IsNonemptyDirectory(const std::filesystem::path& filepath);
	bool IsAncestorOf(const std::filesystem::path& ancestor, const std::filesystem::path& filepath) noexcept;

	std::filesystem::path Relative(const std::filesystem::path& filepath, const std::filesystem::path& parent);
	std::filesystem::path Absolute(const std::filesystem::path& filepath);

	// Makes a file if it does not already exist.
	// Returns if the file exists or was created.
	bool MakeFileIfNotExists(const std::filesystem::path& filepath) noexcept;

	// Makes a directory if the it does not already exist.
	// Returns if the directory exists or was created.
	bool MakeDirectoryIfNotExists(const std::filesystem::path& filepath) noexcept;

	// Renames a file if the destination does not exist.
	// Returns if the file was renamed.
	bool RenameFile(const std::filesystem::path& filepathOld, const std::filesystem::path& filepathNew) noexcept;

	// Renames a directory without overriding the destination, unless it's an empty directory.
	// Returns if the directory was renamed.
	bool RenameDirectory(const std::filesystem::path& filepathOld, const std::filesystem::path& filepathNew) noexcept;

	// Deletes a file or empty directory.
	// Returns if the file or directory was deleted or did not exist in the first place.
	bool Delete(const std::filesystem::path& filepath) noexcept;

	// Deletes a file or recursively deletes a directory and its contents.
	// Returns if the file or directory was deleted or did not exist in the first place.
	bool DeleteRecursive(const std::filesystem::path& filepath);

	// Platform Dependent

	bool IsHidden(const std::filesystem::path& filepath);

	// Makes a hidden directory. If the directory already exists, it's made hidden if it's not already.
	// For cross-platform support (e.g. for linux), prepend a period (.) to the directory name.
	bool MakeHiddenDirectory(const std::filesystem::path& filepath);
}

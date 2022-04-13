#pragma once

#include <filesystem>

namespace gbc::FileDialog
{
	// Opens an open file window at the initial directory.
	std::filesystem::path OpenFile(const wchar_t* filter, const std::filesystem::path& initialDirectoryPath);
	// Opens an open file window at the last visited directory.
	std::filesystem::path OpenFile(const wchar_t* filter);

	// Opens a save file window at the initial directory.
	std::filesystem::path SaveFile(const wchar_t* filter, const std::filesystem::path& initialDirectoryPath);
	// Opens a save file window at the last visited directory.
	std::filesystem::path SaveFile(const wchar_t* filter);

	// Opens an open folder window at the initial directory.
	std::filesystem::path OpenFolder(const std::filesystem::path& initialDirectoryPath);
	// Opens an open folder window at the last visited directory.
	std::filesystem::path OpenFolder();

	// Opens a directory in the file explorer.
	bool OpenFolderInExplorer(const std::filesystem::path& directoryPath);
	// Opens a file in the file explorer.
	bool OpenFileInExplorer(const std::filesystem::path& filepath);
	// Opens a directory in the terminal/command line.
	bool OpenFolderInTerminal(const std::filesystem::path& directoryPath);
}

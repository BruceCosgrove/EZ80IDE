#include "gbcpch.h"
#include "FileIO.h"

namespace gbc::FileIO
{
	std::string ReadFile(const std::filesystem::path& filepath)
	{
		std::ifstream file(filepath);
		if (file.is_open())
		{
			std::stringstream contents;
			contents << file.rdbuf();
			file.close();
			return contents.str();
		}
		return {};
	}

	std::vector<uint8_t> ReadBinaryFile(const std::filesystem::path& filepath)
	{
		std::ifstream file(filepath, std::ios::binary);
		if (file.is_open())
		{
			file.seekg(0, std::ios::end);
			size_t size = file.tellg();
			file.seekg(0, std::ios::beg);

			std::vector<uint8_t> contents(size, 0);
			file.read((char*)contents.data(), size);
			file.close();
			return contents;
		}
		return {};
	}

	bool WriteFile(const std::filesystem::path& filepath, std::string_view contents)
	{
		std::ofstream file(filepath);
		if (file.is_open())
		{
			file << contents;
			file.close();
			return file.good();
		}
		return false;
	}

	bool WriteBinaryFile(const std::filesystem::path& filepath, const std::vector<uint8_t>& contents)
	{
		return WriteBinaryFile(filepath, contents.data(), contents.size());
	}

	bool WriteBinaryFile(const std::filesystem::path& filepath, const void* contents, size_t size)
	{
		std::ofstream file(filepath, std::ios::binary);
		if (file.is_open())
		{
			file.write((const char*)contents, size);
			file.close();
			return file.good();
		}
		return false;
	}

	bool FileExists(const std::filesystem::path& filepath) noexcept
	{
		std::error_code error;
		return std::filesystem::is_regular_file(filepath, error);
	}

	bool DirectoryExists(const std::filesystem::path& filepath) noexcept
	{
		std::error_code error;
		return std::filesystem::is_directory(filepath, error);
	}

	bool IsEmptyDirectory(const std::filesystem::path& filepath)
	{
		std::error_code error;
		return DirectoryExists(filepath) && std::filesystem::is_empty(filepath, error);
	}

	bool IsAncestorOf(const std::filesystem::path& ancestor, const std::filesystem::path& filepath) noexcept
	{
		if (filepath.empty() || ancestor.empty() || filepath.native().size() <= ancestor.native().size()) return false;
		auto ancestorAbsolute = Absolute(ancestor);
		auto filepathAbsolute = Absolute(filepath);
		return filepathAbsolute.native().starts_with(ancestorAbsolute.native());
	}

	std::filesystem::path Absolute(const std::filesystem::path& filepath)
	{
		std::error_code error;
		return std::filesystem::absolute(filepath, error);
	}

	bool MakeFileIfNotExists(const std::filesystem::path& filepath) noexcept
	{
		if (FileExists(filepath))
			return true;
		std::ofstream file(filepath);
		if (!file.is_open())
			return false;
		file.close();
		return true;
	}

	bool MakeDirectoryIfNotExists(const std::filesystem::path& filepath) noexcept
	{
		std::error_code error;
		return DirectoryExists(filepath) || std::filesystem::create_directory(filepath, error);
	}

	bool Delete(const std::filesystem::path& filepath) noexcept
	{
		std::error_code error;
		return std::filesystem::remove(filepath, error) || !error;
	}

	bool DeleteRecursive(const std::filesystem::path& filepath)
	{
		std::error_code error;
		return std::filesystem::remove_all(filepath, error) || !error;
	}
}

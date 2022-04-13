namespace gbc
{
	// I'm putting this here because it is unimportant to the public API,
	// it's simply implementation details which need not distract the users.
	class FilterGenerator
	{
	public:
		FilterGenerator();
		~FilterGenerator();
	private:
		void CreateFilter(std::wstring_view fileTypeName, std::wstring_view extension) noexcept;
		template<size_t Size> void CreateFilter(std::wstring_view fileTypeName, const std::array<std::wstring_view, Size>& extensions) noexcept;
		const wchar_t* GetFilter(std::wstring_view extension) noexcept;

		friend const wchar_t* gbc::GetFilter(std::wstring_view extension) noexcept;
		template<size_t Size> friend const wchar_t* gbc::GetFilter(const std::array<std::wstring_view, Size>& extensions) noexcept;

		static Scope<FilterGenerator> generator;
		static FilterGenerator& Get();

		std::unordered_map<std::wstring_view, wchar_t*> filters;
	};

	template<size_t Size>
	void FilterGenerator::CreateFilter(std::wstring_view fileTypeName, const std::array<std::wstring_view, Size>& extensions) noexcept
	{
		std::wstring_view extension0 = extensions.front();
		size_t filterSize = fileTypeName.size() + 4;
		size_t typesSize = 0;
		for (std::wstring_view extension : extensions)
			typesSize += extension.size() + 2;
		filterSize += 2 * typesSize;

		wchar_t* filter = new wchar_t[filterSize];

		std::memcpy(filter, fileTypeName.data(), fileTypeName.size() * sizeof(wchar_t));
		size_t offset = fileTypeName.size();
		filter[offset] = L' ';
		filter[++offset] = L'(';
		size_t typesOffset = offset + 1;

		for (std::wstring_view extension : extensions)
		{
			filter[++offset] = L'*';
			std::memcpy(filter + ++offset, extension.data(), extension.size() * sizeof(wchar_t));
			filter[offset += extension.size()] = L';';
		}

		filter[offset] = L')';
		filter[++offset] = L'\0';
		std::memcpy(filter + ++offset, filter + typesOffset, typesSize * sizeof(wchar_t));

		// The last two have to be null terminators
		filter[offset += typesSize - 1] = L'\0';
		filter[++offset] = L'\0';

		filters[extension0] = filter;
	}

	template<size_t Size>
	const wchar_t* GetFilter(const std::array<std::wstring_view, Size>& extensions) noexcept
	{
		return FilterGenerator::Get().GetFilter(extensions.front());
	}

	template<size_t Size>
	std::filesystem::path AppendType(const std::filesystem::path& filepath, const std::array<std::wstring_view, Size>& extensions)
	{
		return filepath.native() + extensions.front().data();
	}

	template<size_t Size>
	std::filesystem::path& AppendTypeTo(std::filesystem::path& filepath, const std::array<std::wstring_view, Size>& extensions)
	{
		return filepath = filepath.native() + extensions.front().data();
	}

	constexpr bool IsFile(std::wstring_view filepath, std::wstring_view extension) noexcept
	{
		return filepath.ends_with(extension);
	}

	constexpr bool IsFilepath(const std::filesystem::path& filepath, std::wstring_view extension) noexcept
	{
		return IsFile(filepath.native(), extension);
	}

	template<size_t Size>
	constexpr bool IsFile(std::wstring_view filepath, const std::array<std::wstring_view, Size>& extensions) noexcept
	{
		for (std::wstring_view extension : extensions)
			if (IsFile(filepath, extension))
				return true;
		return false;
	}

	template<size_t Size>
	constexpr bool IsFilepath(const std::filesystem::path& filepath, const std::array<std::wstring_view, Size>& extensions) noexcept
	{
		for (std::wstring_view extension : extensions)
			if (IsFilepath(filepath, extension))
				return true;
		return false;
	}

	// Simply aliases
#define FILE_EXTENSION_DECLARATION(name, exts) \
	constexpr bool Is##name##File(std::wstring_view filepath) noexcept { return IsFile(filepath, exts); } \
	constexpr bool Is##name##Filepath(const std::filesystem::path& filepath) noexcept { return IsFilepath(filepath.native(), exts); } \
	std::filesystem::path Append##name##Type(const std::filesystem::path& filepath); \
	std::filesystem::path& Append##name##TypeTo(std::filesystem::path& filepath); \
	const wchar_t* Get##name##Filter() noexcept

	// Extensions
	FILE_EXTENSION_DECLARATION(EZ80, FileType::ez80);

#undef FILE_EXTENSION_DECLARATION
}

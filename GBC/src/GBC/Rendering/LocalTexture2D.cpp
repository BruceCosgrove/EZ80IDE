#include "gbcpch.h"
#include "LocalTexture2D.h"
#include "GBC/Rendering/RendererAPI.h"
#include <stb/stb_image.h>
#include <stb/stb_image_write.h>

namespace gbc
{
	static bool GetDefaultVerticalFlip()
	{
		return RendererAPI::GetAPI() == RendererAPI::API::OpenGL;
	}

	LocalTexture2D::LocalTexture2D(const std::filesystem::path& filepath, int32_t requiredChannels)
	{
		ReadFile(filepath, requiredChannels);
	}

	LocalTexture2D::LocalTexture2D(const std::filesystem::path& filepath, bool flipVertically, int32_t requiredChannels)
	{
		ReadFile(filepath, flipVertically, requiredChannels);
	}

	LocalTexture2D::LocalTexture2D(int32_t width, int32_t height, int32_t channels, bool clear)
		: width(width), height(height), channels(channels)
	{
		size_t size = static_cast<size_t>(width) * height * channels;
		data = clear ? new uint8_t[size]{ 0 } : new uint8_t[size];
	}

	LocalTexture2D::LocalTexture2D(int32_t width, int32_t height, int32_t channels, uint8_t* data)
		: width(width), height(height), channels(channels), data(data), container(true) {}

	LocalTexture2D::LocalTexture2D(const LocalTexture2D& texture)
		: filepath(texture.filepath), width(texture.width), height(texture.height), channels(texture.channels), container(texture.container)
	{
		if (texture)
		{
			if (container)
				data = texture.data;
			else
			{
				size_t size = static_cast<size_t>(width) * height * channels;
				data = new uint8_t[size];
				memcpy_s(data, size, texture.data, size);
			}
		}
		else
			data = nullptr;
	}

	LocalTexture2D::LocalTexture2D(LocalTexture2D&& texture) noexcept
		: filepath(std::move(texture.filepath)), width(texture.width), height(texture.height), channels(texture.channels), data(texture.data), container(texture.container)
	{
		texture.width = 0;
		texture.height = 0;
		texture.channels = 0;
		texture.data = nullptr;
		texture.container = false;
	}

	LocalTexture2D& LocalTexture2D::operator=(const LocalTexture2D& texture)
	{
		if (this != &texture)
		{
			if (data && !container)
				delete[] data;

			width = texture.width;
			height = texture.height;
			channels = texture.channels;
			filepath = texture.filepath;
			container = texture.container;

			if (texture)
			{
				if (container)
					data = texture.data;
				else
				{
					size_t size = static_cast<size_t>(width) * height * channels;
					data = new uint8_t[size];
					memcpy_s(data, size, texture.data, size);
				}
			}
			else
				data = nullptr;
		}
		return *this;
	}

	LocalTexture2D& LocalTexture2D::operator=(LocalTexture2D&& texture) noexcept
	{
		if (this != &texture)
		{
			filepath = std::move(texture.filepath);
			width = texture.width;
			height = texture.height;
			channels = texture.channels;

			if (data && !container)
				delete[] data;

			data = texture.data;
			container = texture.container;

			texture.width = 0;
			texture.height = 0;
			texture.channels = 0;
			texture.data = nullptr;
			texture.container = false;
		}
		return *this;
	}

	LocalTexture2D::~LocalTexture2D()
	{
		if (!container)
			delete[] data;
	}

	LocalTexture2D LocalTexture2D::createContainer() const noexcept
	{
		return LocalTexture2D(width, height, channels, data);
	}

	bool LocalTexture2D::ReadFile(const std::filesystem::path& filepath, int32_t requiredChannels)
	{
		return ReadFile(filepath, GetDefaultVerticalFlip(), requiredChannels);
	}

	bool LocalTexture2D::ReadFile(const std::filesystem::path& filepath, bool flipVertically, int32_t requiredChannels)
	{
		GBC_PROFILE_FUNCTION();

		stbi_set_flip_vertically_on_load(flipVertically);
		auto filepathString = filepath.string();
		uint8_t* stbiData = stbi_load(filepathString.c_str(), &width, &height, &channels, requiredChannels);
		if (stbiData)
		{
			if (container)
				container = false;
			else if (data)
				delete[] data;
			data = stbiData;

			if (requiredChannels != 0)
				channels = requiredChannels;
			this->filepath = filepath;
			return true;
		}
		return false;
	}

	bool LocalTexture2D::WriteFile(const std::filesystem::path& filepath)
	{
		return WriteFile(filepath, GetDefaultVerticalFlip());
	}

	bool LocalTexture2D::WriteFile(const std::filesystem::path& filepath, bool flipVertically)
	{
		GBC_PROFILE_FUNCTION();

		stbi_flip_vertically_on_write(flipVertically);
		// TODO: let the user decide the output file format
		auto filepathString = filepath.string();
		return data && stbi_write_png(filepathString.c_str(), width, height, channels, data, 0);
	}

	bool LocalTexture2D::SetSubregion(const LocalTexture2D& texture, int32_t positionX, int32_t positionY)
	{
		GBC_PROFILE_FUNCTION();

		if (channels == texture.channels &&
			positionX + texture.width <= width && positionX >= 0 &&
			positionY + texture.height <= height && positionY >= 0)
		{
			size_t textureIncrement = static_cast<size_t>(texture.width) * channels;
			size_t increment = static_cast<size_t>(width) * channels;

			uint8_t* source = texture.data;
			uint8_t* destination = data + ((static_cast<size_t>(positionY) * width + positionX) * channels);
			for (int32_t y = 0; y < texture.height; y++, source += textureIncrement, destination += increment)
				memcpy_s(destination, textureIncrement, source, textureIncrement);
			return true;
		}
		return false;
	}

	Ref<LocalTexture2D> LocalTexture2D::Create()
	{ return CreateRef<LocalTexture2D>(); }

	Ref<LocalTexture2D> LocalTexture2D::Create(const std::filesystem::path& filepath, int32_t requiredChannels)
	{ return CreateRef<LocalTexture2D>(filepath, requiredChannels); }

	Ref<LocalTexture2D> LocalTexture2D::Create(const std::filesystem::path& filepath, bool flipVertically, int32_t requiredChannels)
	{ return CreateRef<LocalTexture2D>(filepath, flipVertically, requiredChannels); }

	Ref<LocalTexture2D> LocalTexture2D::Create(int32_t width, int32_t height, int32_t channels)
	{ return CreateRef<LocalTexture2D>(width, height, channels); }

	Ref<LocalTexture2D> LocalTexture2D::Create(int32_t width, int32_t height, int32_t channels, uint8_t* data)
	{ return CreateRef<LocalTexture2D>(width, height, channels, data); }
}

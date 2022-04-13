#include "gbcpch.h"
#include "Font.h"
#include "GBC/Rendering/RendererAPI.h"
#include "Platform/Renderer/OpenGL/OpenGLFont.h"

namespace gbc
{
	Ref<Font> Font::Create(const std::filesystem::path& filepath, float size)
	{
		switch (RendererAPI::GetAPI())
		{
			case RendererAPI::API::Headless: return nullptr;
			case RendererAPI::API::OpenGL: return CreateRef<OpenGLFont>(filepath, size);
		}

		GBC_CORE_ASSERT(false, "Unknown Renderer API!");
		return nullptr;
	}
}

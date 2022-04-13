#include "gbcpch.h"
#include "RendererAPI.h"
#include "Platform/Renderer/OpenGL/OpenGLRendererAPI.h"

namespace gbc
{
	RendererAPI::API RendererAPI::api = static_cast<RendererAPI::API>(GBC_RENDERER_API_ID);

	Scope<RendererAPI> RendererAPI::Create()
	{
		switch (api)
		{
			case API::Headless: return nullptr;
			case API::OpenGL: return CreateScope<OpenGLRendererAPI>();
		}

		GBC_CORE_ASSERT(false, "Unknown Renderer API!");
		return nullptr;
	}
}

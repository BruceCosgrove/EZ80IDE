#include "Context.h"
#include "gbcpch.h"
#include "GBC/Rendering/RendererAPI.h"
#include "Platform/Renderer/OpenGL/OpenGLContext.h"

namespace gbc
{
	Scope<Context> Context::Create(void* nativeWindow)
	{
		switch (RendererAPI::GetAPI())
		{
			case RendererAPI::API::Headless: return nullptr;
			case RendererAPI::API::OpenGL: return CreateScope<OpenGLContext>(nativeWindow);
		}

		GBC_CORE_ASSERT(false, "Unknown Renderer API!");
		return nullptr;
	}
}

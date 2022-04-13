#include "gbcpch.h"
#include "RendererCapabilities.h"
#include "GBC/Rendering/Renderer.h"

namespace gbc
{
	int32_t RendererCapabilities::GetMaxTextureSlots()
	{ return Renderer::api->GetMaxTextureSlots(); }

	int32_t RendererCapabilities::GetMaxTextureSize()
	{ return Renderer::api->GetMaxTextureSize(); }

	int32_t RendererCapabilities::GetMaxFramebufferWidth()
	{ return Renderer::api->GetMaxFramebufferWidth(); }

	int32_t RendererCapabilities::GetMaxFramebufferHeight()
	{ return Renderer::api->GetMaxFramebufferHeight(); }

	int32_t RendererCapabilities::GetMaxFramebufferColorAttachments()
	{ return Renderer::api->GetMaxFramebufferColorAttachments(); }
}

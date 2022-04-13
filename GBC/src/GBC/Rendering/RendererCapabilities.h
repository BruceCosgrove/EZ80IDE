#pragma once

#include <cstdint>

namespace gbc
{
	class RendererCapabilities
	{
	public:
		static int32_t GetMaxTextureSlots();
		static int32_t GetMaxTextureSize();
		static int32_t GetMaxFramebufferWidth();
		static int32_t GetMaxFramebufferHeight();
		static int32_t GetMaxFramebufferColorAttachments();
	};
}

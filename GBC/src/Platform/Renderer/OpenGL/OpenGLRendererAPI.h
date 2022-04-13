#pragma once

#include "GBC/Rendering/RendererAPI.h"

namespace gbc
{
	class OpenGLRendererAPI : public RendererAPI
	{
	public:
		virtual void Init() override;
		virtual void Shutdown() override;

		virtual void EnableDepthTest() override;
		virtual void DisableDepthTest() override;
		virtual void EnableBlending() override;
		virtual void DisableBlending() override;
		virtual void EnableCullFace() override;
		virtual void DisableCullFace() override;

		virtual void SetViewport(int32_t x, int32_t y, int32_t width, int32_t height) override;

		virtual void Clear() override;
		virtual void ClearDepthOnly() override;
		virtual void SetClearColor(float red, float green, float blue, float alpha) override;

		virtual void DrawIndexed(const Ref<VertexArray>& vertexArray, const Ref<IndexBuffer>& indexBuffer, uint32_t offset, uint32_t count, RendererPrimitive primitive) override;

		// Renderer capabilities
		virtual int32_t GetMaxTextureSlots() override;
		virtual int32_t GetMaxTextureSize() override;
		virtual int32_t GetMaxFramebufferWidth() override;
		virtual int32_t GetMaxFramebufferHeight() override;
		virtual int32_t GetMaxFramebufferColorAttachments() override;
	private:
		uint32_t clearBits = 0;
	};
}

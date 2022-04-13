#pragma once

#include "GBC/Rendering/Framebuffer.h"

namespace gbc
{
	class OpenGLFramebuffer : public Framebuffer
	{
	public:
		OpenGLFramebuffer(const FramebufferSpecification& specification);
		virtual ~OpenGLFramebuffer();

		virtual void Bind() override;
		virtual void Unbind() override;

		virtual void OnViewportResize(int32_t width, int32_t height) override;
		virtual const FramebufferSpecification& GetSpecification() const override { return specification; }

		virtual RendererID GetColorAttachment(uint32_t index = 0) const override { GBC_CORE_ASSERT(index < static_cast<uint32_t>(colorAttachments.size())); return colorAttachments[index]; }
		virtual RendererID GetDepthAttachment() const override { return depthAttachment; }

		virtual int32_t GetColorPixel(int32_t x, int32_t y, uint32_t index = 0) const override;

		virtual void ClearColorAttachment(int32_t value, uint32_t index = 0) override;
	private:
		void Recreate();
		void Clear();

		FramebufferSpecification specification;
		RendererID rendererID = 0;

		std::vector<FramebufferTextureSpecification> colorAttachmentSpecifications;
		FramebufferTextureSpecification depthAttachmentSpecification = FramebufferTextureFormat::None;

		std::vector<RendererID> colorAttachments;
		RendererID depthAttachment = 0;
	};
}

#include "gbcpch.h"
#include "OpenGLFramebuffer.h"
#include "GBC/Rendering/RendererCapabilities.h"
#include <glad/glad.h>

namespace gbc
{
	static constexpr GLenum GetOpenGLFilterMode(FramebufferFilterMode filter)
	{
		switch (filter)
		{
			case FramebufferFilterMode::Linear:  return GL_LINEAR;
			case FramebufferFilterMode::Nearest: return GL_NEAREST;
		}

		GBC_CORE_ASSERT(false, "Unknown Framebuffer Filter Mode!");
		return 0;
	}

	static constexpr GLenum GetOpenGLWrapMode(FramebufferWrapMode wrap)
	{
		switch (wrap)
		{
			case FramebufferWrapMode::ClampToEdge: return GL_CLAMP_TO_EDGE;
			case FramebufferWrapMode::Repeat:      return GL_REPEAT;
		}

		GBC_CORE_ASSERT(false, "Unknown Framebuffer Wrap Mode!");
		return 0;
	}

	static constexpr GLenum GetOpenGLFormat(FramebufferTextureFormat format)
	{
		switch (format)
		{
			case FramebufferTextureFormat::RGBA8:      return GL_RGBA8;
			case FramebufferTextureFormat::RedInteger: return GL_RED_INTEGER;
		}

		GBC_CORE_ASSERT(false, "Unknown Framebuffer Texture Format!");
		return 0;
	}

	static constexpr GLenum GetTextureTarget(bool multisampled)
	{
		return multisampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
	}

	static void CreateTextures(GLenum textureTarget, uint32_t* id, uint32_t count)
	{
		glCreateTextures(textureTarget, count, id);
	}

	static void BindTexture(GLenum textureTarget, uint32_t id)
	{
		glBindTexture(textureTarget, id);
	}

	static void AttachColorTexture(uint32_t id, int32_t samples, GLenum internalFormat, GLenum format, int32_t width, int32_t height, FramebufferTextureSpecification textureSpecification, uint32_t index)
	{
		bool multisampled = samples > 1;
		if (multisampled)
		{
			glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, internalFormat, width, height, GL_FALSE);
		}
		else
		{
			glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, nullptr);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GetOpenGLFilterMode(textureSpecification.minFilter));
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GetOpenGLFilterMode(textureSpecification.magFilter));
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GetOpenGLWrapMode(textureSpecification.wrapS));
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GetOpenGLWrapMode(textureSpecification.wrapT));
		}

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, GetTextureTarget(multisampled), id, 0);
	}

	static void AttachDepthTexture(uint32_t id, int32_t samples, GLenum format, GLenum attachmentType, int32_t width, int32_t height, FramebufferTextureSpecification textureSpecification)
	{
		bool multisampled = samples > 1;
		if (multisampled)
		{
			glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, format, width, height, GL_FALSE);
		}
		else
		{
			glTexStorage2D(GL_TEXTURE_2D, 1, format, width, height);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GetOpenGLFilterMode(textureSpecification.minFilter));
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GetOpenGLFilterMode(textureSpecification.magFilter));
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GetOpenGLWrapMode(textureSpecification.wrapS));
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GetOpenGLWrapMode(textureSpecification.wrapT));
		}

		glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentType, GetTextureTarget(multisampled), id, 0);
	}

	OpenGLFramebuffer::OpenGLFramebuffer(const FramebufferSpecification& specification)
		: specification(specification)
	{
		for (const auto& specification : this->specification.attachments.attachments)
		{
			if (IsDepthFormat(specification.format))
			{
				GBC_CORE_ASSERT(depthAttachmentSpecification.format == FramebufferTextureFormat::None, "Framebuffer already has depth attachment!");
				depthAttachmentSpecification = specification;
			}
			else
				colorAttachmentSpecifications.push_back(specification);
		}

		Recreate();
	}

	OpenGLFramebuffer::~OpenGLFramebuffer()
	{
		Clear();
	}

	void OpenGLFramebuffer::Recreate()
	{
		Clear();

		glCreateFramebuffers(1, &rendererID);
		glBindFramebuffer(GL_FRAMEBUFFER, rendererID);

		// Attachments
		bool multisampled = specification.samples > 1;
		GLenum textureTarget = GetTextureTarget(multisampled);

		if (colorAttachmentSpecifications.size() != 0)
		{
			colorAttachments.resize(colorAttachmentSpecifications.size());
			CreateTextures(textureTarget, colorAttachments.data(), static_cast<uint32_t>(colorAttachments.size()));

			for (uint32_t i = 0; i < static_cast<uint32_t>(colorAttachments.size()); i++)
			{
				BindTexture(textureTarget, colorAttachments[i]);

				switch (colorAttachmentSpecifications[i].format)
				{
					case FramebufferTextureFormat::RGBA8:
						AttachColorTexture(colorAttachments[i], specification.samples, GL_RGBA8, GL_RGBA, specification.width, specification.height, colorAttachmentSpecifications[i], i);
						break;
					case FramebufferTextureFormat::RedInteger:
						AttachColorTexture(colorAttachments[i], specification.samples, GL_R32I, GL_RED_INTEGER, specification.width, specification.height, colorAttachmentSpecifications[i], i);
						break;
				}
			}
		}
	
		if (depthAttachmentSpecification.format != FramebufferTextureFormat::None)
		{
			CreateTextures(textureTarget, &depthAttachment, 1);
			BindTexture(textureTarget, depthAttachment);

			switch (depthAttachmentSpecification.format)
			{
				case FramebufferTextureFormat::Depth24Stencil8:
					AttachDepthTexture(depthAttachment, specification.samples, GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL_ATTACHMENT, specification.width, specification.height, depthAttachmentSpecification);
					break;
			}
		}

		if (colorAttachments.empty())
		{
			// Only depth-pass
			glDrawBuffer(GL_NONE);
		}
		else
		{
			int maxColorAttachments = RendererCapabilities::GetMaxFramebufferColorAttachments();
			GBC_CORE_ASSERT(colorAttachments.size() <= maxColorAttachments, "Too many Framebuffer color attachments!");
			GLenum* buffers = (GLenum*)alloca(colorAttachments.size() * sizeof(GLenum));
			for (uint32_t i = 0; i < static_cast<uint32_t>(colorAttachments.size()); i++)
				buffers[i] = static_cast<GLenum>(GL_COLOR_ATTACHMENT0 + i);

			glDrawBuffers(static_cast<GLsizei>(colorAttachments.size()), buffers);
		}

		GBC_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete!");

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void OpenGLFramebuffer::Clear()
	{
		if (rendererID != 0)
		{
			glDeleteFramebuffers(1, &rendererID);
			glDeleteTextures(static_cast<GLsizei>(colorAttachments.size()), colorAttachments.data());
			glDeleteTextures(1, &depthAttachment);

			rendererID = 0;
			colorAttachments.clear();
			depthAttachment = 0;
		}
	}

	void OpenGLFramebuffer::Bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, rendererID);
		glViewport(0, 0, specification.width, specification.height);
	}

	void OpenGLFramebuffer::Unbind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void OpenGLFramebuffer::OnViewportResize(int32_t width, int32_t height)
	{
		GBC_CORE_ASSERT(width <= RendererCapabilities::GetMaxFramebufferWidth() && height <= RendererCapabilities::GetMaxFramebufferHeight(), "Framebuffer too large!");
		specification.width = width;
		specification.height = height;
		Recreate();
	}

	int32_t OpenGLFramebuffer::GetColorPixel(int32_t x, int32_t y, uint32_t index) const
	{
		GBC_CORE_ASSERT(index < colorAttachments.size(), "Framebuffer attachment index out of bounds!");
		GBC_CORE_ASSERT(x >= 0 && x < specification.width && y >= 0 && y < specification.height, "Framebuffer position out of bounds!");

		glReadBuffer(GL_COLOR_ATTACHMENT0 + index);
		GLenum format = GetOpenGLFormat(colorAttachmentSpecifications[index].format);
		int32_t pixel;
		glReadPixels(x, y, 1, 1, format, GL_INT, &pixel);
		return pixel;
	}

	void OpenGLFramebuffer::ClearColorAttachment(int32_t value, uint32_t index)
	{
		GBC_CORE_ASSERT(index < colorAttachments.size(), "Framebuffer attachment index out of bounds!");

		GLenum format = GetOpenGLFormat(colorAttachmentSpecifications[index].format);
		glClearTexImage(colorAttachments[index], 0, format, GL_INT, &value);
	}
}

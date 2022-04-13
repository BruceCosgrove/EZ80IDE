namespace gbc
{
	static constexpr bool IsDepthFormat(FramebufferTextureFormat format)
	{
		switch (format)
		{
			case FramebufferTextureFormat::Depth24Stencil8:
				return true;
			case FramebufferTextureFormat::RGBA8:
			case FramebufferTextureFormat::RedInteger:
				return false;
		}

		GBC_CORE_ASSERT(false, "Unknown Framebuffer Texture Format!");
		return false;
	}

	constexpr FramebufferTextureSpecification::FramebufferTextureSpecification(FramebufferTextureFormat format) noexcept
		: format(format) {}

	constexpr FramebufferTextureSpecification::FramebufferTextureSpecification(FramebufferTextureFormat format, FramebufferFilterMode minFilter, FramebufferFilterMode magFilter, FramebufferWrapMode wrapS, FramebufferWrapMode wrapT) noexcept
		: format(format), minFilter(minFilter), magFilter(magFilter), wrapS(wrapS), wrapT(wrapT) {}

	constexpr FramebufferAttachmentSpecification::FramebufferAttachmentSpecification(std::initializer_list<FramebufferTextureSpecification> attachments) noexcept
		: attachments(attachments) {}
}

#pragma once

#include "GBC/Rendering/Framebuffer.h"
#include "GBC/Rendering/LocalTexture2D.h"

namespace gbc
{
	enum class TextureFormat : uint8_t
	{
		None = 0,

		R8,
		RGB8,
		RGBA8
	};

	enum class TextureFilterMode : uint8_t
	{
		Linear,
		Nearest
	};

	enum class TextureWrapMode : uint8_t
	{
		ClampToEdge,
		Repeat
	};

	struct TextureSpecification
	{
		constexpr TextureSpecification() noexcept = default;
		TextureSpecification(const Ref<LocalTexture2D>& texture) noexcept
			: texture(texture) {}
		TextureSpecification(const Ref<LocalTexture2D>& texture, TextureFilterMode minFilter, TextureFilterMode magFilter, TextureWrapMode wrapS, TextureWrapMode wrapT) noexcept
			: texture(texture), minFilter(minFilter), magFilter(magFilter), wrapS(wrapS), wrapT(wrapT) {}
		TextureSpecification(TextureFilterMode minFilter, TextureFilterMode magFilter, TextureWrapMode wrapS, TextureWrapMode wrapT) noexcept
			: minFilter(minFilter), magFilter(magFilter), wrapS(wrapS), wrapT(wrapT) {}
		TextureSpecification(const TextureSpecification& specs) noexcept
			: texture(specs.texture), minFilter(specs.minFilter), magFilter(specs.magFilter), wrapS(specs.wrapS), wrapT(specs.wrapT) {}
		TextureSpecification& operator=(const TextureSpecification& specs) noexcept;

		Ref<LocalTexture2D> texture = nullptr;
		TextureFilterMode minFilter = TextureFilterMode::Linear;
		TextureFilterMode magFilter = TextureFilterMode::Linear;
		TextureWrapMode wrapS = TextureWrapMode::ClampToEdge;
		TextureWrapMode wrapT = TextureWrapMode::ClampToEdge;
	};

	enum class SwizzleType
	{
		Zero, One, Red, Green, Blue, Alpha
	};

	class Texture
	{
	public:
		virtual ~Texture() = default;
		virtual void Recreate(const TextureSpecification& specification) = 0; // TODO: this is dumb.

		virtual void ChangeSampling(const TextureSpecification& specification) = 0;

		virtual void Bind(uint32_t slot = 0) const = 0;
		virtual void Unbind(uint32_t slot = 0) const = 0;

		virtual void Swizzle(const std::array<SwizzleType, 4>& swizzles) = 0;

		virtual RendererID GetRendererID() const = 0;
		virtual const Ref<LocalTexture2D>& GetLocalTexture() const = 0;
		virtual const TextureSpecification& GetSpecification() const = 0;

		// Call this when you want to update the internal
		// texture after the the local texture has updated.
		// Only call if this was not constructed with a Framebuffer.
		virtual void Update() = 0;

		// Returns true if this texture was constructed with a Framebuffer, false otherwise.
		virtual bool IsFramebufferTexture() const = 0;
	};

	class Texture2D : public Texture
	{
	public:
		static Ref<Texture2D> Create(const TextureSpecification& specification);
		static Ref<Texture2D> Create(const Ref<Framebuffer>& framebuffer, int32_t attachmentIndex);
	};
}

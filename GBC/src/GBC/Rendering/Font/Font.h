#pragma once

#include "GBC/Core/Core.h"
#include "GBC/Rendering/Texture.h"
#include "Glyph.h"
#include <filesystem>

namespace gbc
{
	class Font
	{
	public:
		virtual ~Font() = default;

		virtual float GetSize() = 0;
		virtual float GetScale() = 0;
		virtual Ref<Texture2D> GetTexture() = 0;
		virtual const glm::ivec2& GetTextureSize() = 0;

		virtual const Glyph* GetGlyph(int32_t codepoint) = 0;
		virtual int32_t GetKerning(int32_t codepoint1, int32_t codepoint2) = 0;

		static Ref<Font> Create(const std::filesystem::path& filepath, float size);
	};
}

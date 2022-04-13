#pragma once

#include "GBC/Rendering/Font/Font.h"
#include "GBC/Util/TextureAtlas.h"
#include "Platform/Renderer/OpenGL/OpenGLGlyph.h"
#include <stb/stb_truetype.h>
#include <unordered_map>

namespace gbc
{
	class OpenGLFont : public Font
	{
	public:
		OpenGLFont(const std::filesystem::path& filepath, float size);
		virtual ~OpenGLFont();

		constexpr virtual float GetSize() override { return size; }
		constexpr virtual float GetScale() override { return scale; }
		virtual Ref<Texture2D> GetTexture() override;
		constexpr virtual const glm::ivec2& GetTextureSize() override { return atlas.GetTextureSize(); }

		virtual const Glyph* GetGlyph(int32_t codepoint) override;
		virtual int32_t GetKerning(int32_t codepoint1, int32_t codepoint2) override;
	private:
		std::vector<uint8_t> fontFileContents;
		float size;
		float scale;
		stbtt_fontinfo info;
		std::unordered_map<int32_t, OpenGLGlyph> glyphs;
		std::unordered_map<uint64_t, int32_t> kernings;

		TextureAtlas atlas;
		Ref<Texture2D> texture = nullptr;
		bool recreateTexture = false;
		bool updateTexture = false;
	};
}

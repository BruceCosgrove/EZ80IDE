#include "gbcpch.h"
#include "OpenGLFont.h"
#include "GBC/IO/FileIO.h"
#include "GBC/Rendering/RendererAPI.h"

namespace gbc
{
	OpenGLFont::OpenGLFont(const std::filesystem::path& filepath, float size)
		: fontFileContents(FileIO::ReadBinaryFile(filepath))
		, size(size)
	{
		int32_t status = stbtt_InitFont(&info, fontFileContents.data(), stbtt_GetFontOffsetForIndex(fontFileContents.data(), 0));
		GBC_ASSERT(status, "Failed to initialize font!");
		scale = stbtt_ScaleForPixelHeight(&info, size);
	}

	OpenGLFont::~OpenGLFont()
	{
		for (auto& [codepoint, glyph] : glyphs)
			stbtt_FreeBitmap(glyph.bitmap, nullptr);
	}

	Ref<Texture2D> OpenGLFont::GetTexture()
	{
		if (updateTexture)
		{
			updateTexture = false;
			{
				texture->Update();
				//texture->GetLocalTexture()->WriteFile("C:/Workspace/test.png", false);
			}
		}
		else if (recreateTexture)
		{
			recreateTexture = false;
			if (texture)
			{
				texture->Recreate(atlas.Create(1));
				//texture->GetLocalTexture()->WriteFile("C:/Workspace/test.png", false);
			}
			else
			{
				texture = Texture2D::Create(atlas.Create(1));

				// TODO: the swizzle stopped working when i separated the quad shader and the text shader.
				// I don't understand because I literally copied the text shader to start with and only
				// removed the unnecessary things.
				//texture->Swizzle({ SwizzleType::One, SwizzleType::One, SwizzleType::One, SwizzleType::Red });
			}
		}
		return texture;
	}

	const Glyph* OpenGLFont::GetGlyph(int32_t codepoint)
	{
		auto it = glyphs.find(codepoint);
		if (it != glyphs.end())
			return &it->second;

		int32_t glyphIndex = stbtt_FindGlyphIndex(&info, codepoint);
		if (!glyphIndex) return nullptr;

		auto& glyph = glyphs[codepoint];
		glyph.index = glyphIndex;
		//glyph.bitmap = stbtt_GetGlyphBitmap(&info, 0, scale, glyphIndex, &glyph.textureRect.z, &glyph.textureRect.w, &glyph.bearing.x, &glyph.bearing.y);
		glyph.bitmap = stbtt_GetGlyphSDF(&info, scale, glyphIndex, 8, 0x80, 32.0f, &glyph.textureRect.z, &glyph.textureRect.w, &glyph.bearing.x, &glyph.bearing.y);
		glyph.bearing.y = -glyph.bearing.y;
		int32_t advance;
		stbtt_GetGlyphHMetrics(&info, glyphIndex, &advance, nullptr);
		glyph.advance = advance * scale;

		glm::ivec2 preAtlasSize = atlas.GetTextureSize();
		glm::ivec2 texCoord = atlas.AddTexture(LocalTexture2D::Create(glyph.textureRect.z, glyph.textureRect.w, 1, glyph.bitmap), 2);
		glyph.textureRect.x = texCoord.x;
		glyph.textureRect.y = texCoord.y;

		if (preAtlasSize != atlas.GetTextureSize() || !texture)
			recreateTexture = true;
		else
			updateTexture = true;

		return &glyph;
	}

	int32_t OpenGLFont::GetKerning(int32_t codepoint1, int32_t codepoint2)
	{
		uint64_t kerningIndex = static_cast<uint64_t>(codepoint1) << 32 | static_cast<uint32_t>(codepoint2);
		auto it = kernings.find(kerningIndex);
		if (it != kernings.end())
			return it->second;

		auto glyph1 = (OpenGLGlyph*)GetGlyph(codepoint1);
		if (!glyph1) return 0;
		auto glyph2 = (OpenGLGlyph*)GetGlyph(codepoint2);
		if (!glyph2) return 0;

		int32_t kerning = stbtt_GetGlyphKernAdvance(&info, glyph1->index, glyph2->index);
		kernings[kerningIndex] = kerning;
		return kerning;
	}
}

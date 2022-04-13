#pragma once

#include "GBC/Rendering/Font/Glyph.h"

namespace gbc
{
	class OpenGLGlyph : public Glyph
	{
	public:
		constexpr OpenGLGlyph() noexcept = default; // only needed for unordered_map
		constexpr OpenGLGlyph(const glm::ivec4& textureRect, const glm::ivec2& bearing, float advance, int32_t index, uint8_t* bitmap) noexcept
			: textureRect(textureRect)
			, bearing(bearing)
			, advance(advance)
			, index(index)
			, bitmap(bitmap)
		{}

		constexpr virtual const glm::ivec4& GetTextureRect() const override { return textureRect; }
		constexpr virtual const glm::ivec2& GetBearing() const override { return bearing; }
		constexpr virtual float GetAdvance() const override { return advance; }
	public:
		constexpr int32_t GetIndex() const noexcept { return index; }
		constexpr uint8_t* GetBitmap() const noexcept { return bitmap; }
	private:
		friend class OpenGLFont;

		glm::ivec4 textureRect;
		glm::ivec2 bearing;
		float advance;

		int32_t index;
		uint8_t* bitmap;
	};
}

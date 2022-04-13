#pragma once

#include <glm/glm.hpp>

namespace gbc
{
	class Glyph
	{
	public:
		virtual ~Glyph() = default;

		// Returns a 4-component vector as follows:
		//	x = texture coord x (texel space)
		//	y = texture coord y (texel space)
		//	z = width (texel space)
		//	w = height (texel space)
		virtual const glm::ivec4& GetTextureRect() const = 0;

		// Returns a 2-component vector as follows:
		//	x = bearing x (texel space)
		//	y = bearing y (texel space)
		virtual const glm::ivec2& GetBearing() const = 0;

		//	y = bearing y (texel space)
		// Returns the advance (texel space)
		virtual float GetAdvance() const = 0;
	};
}

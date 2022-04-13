#pragma once

#include "GBC/Core/Core.h"
#include "GBC/Core/Window.h"
#include "GBC/Rendering/LocalTexture2D.h"
#include <glm/glm.hpp>

namespace gbc
{
	enum class StandardCursor
	{
		Arrow, IBeam, Crosshair, Hand, HorizontalResize, VerticalResize, ResizeAll, ResizeNESW, ResizeNWSE, Disallowed
	};

	class Cursor
	{
	public:
		~Cursor();
	public:
		// Creates a standard cursor.
		static Ref<Cursor> CreateStandardCursor(StandardCursor cursor);

		// Creates a custom cursor.
		static Ref<Cursor> CreateCustomCursor(const Ref<LocalTexture2D>& texture, const glm::ivec2& hotspot);

		// Destroys either a standard or custom a cursor.
		static void DestroyCursor(Ref<Cursor>& cursor);

		// Sets the given window's active cursor.
		static void SetCursor(Window& window, const Ref<Cursor>& cursor);

		// Resets the given window's cursor to the system's default.
		static void ResetCursor(Window& window);
	private:
		void* nativeCursor;
	};
}

#include "gbcpch.h"
#include "GBC/Core/Cursor.h"
#include <glfw/glfw3.h>

namespace gbc
{
	static constexpr int GetWindowsCursor(StandardCursor standardCursor)
	{
		switch (standardCursor)
		{
			case StandardCursor::Arrow:            return GLFW_ARROW_CURSOR;
			case StandardCursor::IBeam:            return GLFW_IBEAM_CURSOR;
			case StandardCursor::Crosshair:        return GLFW_CROSSHAIR_CURSOR;
			case StandardCursor::Hand:             return GLFW_HAND_CURSOR;
			case StandardCursor::HorizontalResize: return GLFW_HRESIZE_CURSOR;
			case StandardCursor::VerticalResize:   return GLFW_VRESIZE_CURSOR;
			case StandardCursor::ResizeAll:        return GLFW_RESIZE_ALL_CURSOR;
			case StandardCursor::ResizeNESW:       return GLFW_RESIZE_NESW_CURSOR;
			case StandardCursor::ResizeNWSE:       return GLFW_RESIZE_NWSE_CURSOR;
			case StandardCursor::Disallowed:       return GLFW_NOT_ALLOWED_CURSOR;
		}

		GBC_CORE_ASSERT(false, "Unknown Standard Cursor!");
		return 0;
	}

	Cursor::~Cursor()
	{
		if (nativeCursor)
			glfwDestroyCursor((GLFWcursor*)nativeCursor);
	}

	Ref<Cursor> Cursor::CreateStandardCursor(StandardCursor standardCursor)
	{
		Ref<Cursor> cursor = CreateRef<Cursor>();
		cursor->nativeCursor = glfwCreateStandardCursor(GetWindowsCursor(standardCursor));
		return cursor;
	}

	Ref<Cursor> Cursor::CreateCustomCursor(const Ref<LocalTexture2D>& texture, const glm::ivec2& hotspot)
	{
		if (!texture)
			return nullptr;

		GLFWimage image{ texture->GetWidth(), texture->GetHeight(), texture->GetData() };

		Ref<Cursor> cursor = CreateRef<Cursor>();
		cursor->nativeCursor = glfwCreateCursor(&image, hotspot.x, hotspot.y);
		return cursor;
	}

	void Cursor::DestroyCursor(Ref<Cursor>& cursor)
	{
		if (cursor)
		{
			glfwDestroyCursor((GLFWcursor*)cursor->nativeCursor);
			cursor->nativeCursor = nullptr;
			cursor.reset();
		}
	}

	void Cursor::SetCursor(Window& window, const Ref<Cursor>& cursor)
	{
		if (!cursor)
			ResetCursor(window);
		else
			glfwSetCursor((GLFWwindow*)window.GetNativeWindow(), (GLFWcursor*)cursor->nativeCursor);
	}

	void Cursor::ResetCursor(Window& window)
	{
		glfwSetCursor((GLFWwindow*)window.GetNativeWindow(), nullptr);
	}
}

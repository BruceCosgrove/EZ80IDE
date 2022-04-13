#pragma once

#include "GBC/Core/Keycodes.h"
#include "GBC/Core/MouseButtons.h"
#include "GBC/Core/Window.h"
#include "GBC/Events/KeyEvents.h"
#include "GBC/Events/MouseEvents.h"
#include <glm/glm.hpp>

namespace gbc
{
	class Input
	{
	public:
		// Returns true if the key is currently pressed, false otherwise
		static bool IsKeyPressed(Keycode keycode);
		// Returns true if the key is currently released, false otherwise
		static bool IsKeyReleased(Keycode keycode);

		// Returns true if the mouse button is currently pressed, false otherwise
		static bool IsMouseButtonPressed(MouseButton button);
		// Returns true if the mouse button is currently released, false otherwise
		static bool IsMouseButtonReleased(MouseButton button);

		// Gets the position of the mouse relative to the top left corner of the primary monitor.
		static glm::vec2 GetAbsoluteMousePosition();
		// Gets the position of the mouse relative to the top left of nativeWindow's content area.
		static glm::vec2 GetRelativeMousePosition(void* nativeWindow);
		// Gets the difference between the relative mouse position and the absolute mouse position.
		static glm::vec2 GetWindowContentAreaOffset(void* nativeWindow);

		// Gets the clipboard string.
		static std::string GetClipboardString();
		// Sets the clipboard string.
		static void SetClipboardString(const std::string& string);
	private:
		friend class Application;

		static void SetEventCallback(const EventCallbackFunc& callback);
		static void OnKeyPressEvent(KeyPressEvent& event);
		static void OnKeyReleaseEvent(KeyReleaseEvent& event);
		static void OnMouseButtonPressEvent(MouseButtonPressEvent& event);
		static void OnMouseButtonReleaseEvent(MouseButtonReleaseEvent& event);
	};
}

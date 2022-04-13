#include "gbcpch.h"
#include "GBC/Core/Input.h"
#include "GBC/Core/Application.h"
#include <glfw/glfw3.h>
#include <array>

namespace gbc
{
	struct InputData
	{
		std::array<bool, GLFW_KEY_LAST + 1> keys{ false };
		std::array<bool, GLFW_MOUSE_BUTTON_LAST + 1> mouseButtons{ false };
		EventCallbackFunc eventCallback;
	};
	static InputData data;
	
	void Input::SetEventCallback(const EventCallbackFunc& callback)
	{
		data.eventCallback = callback;
	}

	void Input::OnKeyPressEvent(KeyPressEvent& event)
	{
		data.keys[static_cast<size_t>(event.GetKeycode())] = true;
	}

	void Input::OnKeyReleaseEvent(KeyReleaseEvent& event)
	{
		data.keys[static_cast<size_t>(event.GetKeycode())] = false;
	}

	void Input::OnMouseButtonPressEvent(MouseButtonPressEvent& event)
	{
		data.mouseButtons[static_cast<size_t>(event.GetButton())] = true;
	}

	void Input::OnMouseButtonReleaseEvent(MouseButtonReleaseEvent& event)
	{
		data.mouseButtons[static_cast<size_t>(event.GetButton())] = false;
	}

	// Key

	bool Input::IsKeyPressed(Keycode keycode)
	{
		GBC_CORE_ASSERT(keycode < Keycode::Count, "Keycode index out of bounds!");
		return data.keys[static_cast<size_t>(keycode)];
	}

	bool Input::IsKeyReleased(Keycode keycode)
	{
		GBC_CORE_ASSERT(keycode < Keycode::Count, "Keycode index out of bounds!");
		return !data.keys[static_cast<size_t>(keycode)];
	}

	// Mouse

	bool Input::IsMouseButtonPressed(MouseButton button)
	{
		GBC_CORE_ASSERT(button < MouseButton::Count, "MouseButton index out of bounds!");
		return data.mouseButtons[static_cast<size_t>(button)];
	}

	bool Input::IsMouseButtonReleased(MouseButton button)
	{
		GBC_CORE_ASSERT(button < MouseButton::Count, "MouseButton index out of bounds!");
		return !data.mouseButtons[static_cast<size_t>(button)];
	}

	glm::vec2 Input::GetAbsoluteMousePosition()
	{
		GLFWwindow* window = glfwGetCurrentContext();
		int32_t windowX, windowY;
		double cursorX, cursorY;
		glfwGetWindowPos(window, &windowX, &windowY);
		glfwGetCursorPos(window, &cursorX, &cursorY);
		return { static_cast<float>(windowX + cursorX), static_cast<float>(windowY + cursorY) };
	}

	glm::vec2 Input::GetRelativeMousePosition(void* nativeWindow)
	{
		GLFWwindow* window = static_cast<GLFWwindow*>(nativeWindow);
		double cursorX, cursorY;
		glfwGetCursorPos(window, &cursorX, &cursorY);
		return { static_cast<float>(cursorX), static_cast<float>(cursorY) };
	}

	glm::vec2 Input::GetWindowContentAreaOffset(void* nativeWindow)
	{
		int32_t windowX, windowY;
		glfwGetWindowPos((GLFWwindow*)nativeWindow, &windowX, &windowY);
		return { static_cast<float>(windowX), static_cast<float>(windowY) };
	}

	std::string Input::GetClipboardString()
	{
		return glfwGetClipboardString(nullptr);
	}

	void Input::SetClipboardString(const std::string& string)
	{
		glfwSetClipboardString(nullptr, string.c_str());
	}
}

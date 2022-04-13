#pragma once

#include "GBC/Core/Core.h"
#if GBC_ENABLE_IMGUI
#include "GBC/Events/Event.h"
#include <glm/glm.hpp>
#include <imgui/imgui.h>
#include <string>

namespace gbc
{
	class EZ80IDELayer;

	class Panel
	{
	public:
		constexpr Panel() noexcept = default; // Needed to be stored in most stl types.
		constexpr Panel(EZ80IDELayer* ez80IDELayer, const std::string& title, bool enabled = true)
			: ez80IDELayer(ez80IDELayer), title(title), enabled(enabled) {}
		constexpr virtual ~Panel() = default;

		void OnImGuiRender();
		constexpr virtual void OnEvent(Event& event) {}

		constexpr const std::string& GetDefaultTitle() const noexcept { return title; }

		constexpr bool HasEnabledChanged() const noexcept { return enabledChanged; }
		constexpr bool IsEnabled() const noexcept { return enabled; }
		virtual void SetEnabled(bool enabled) noexcept;
		void ToggleEnabled() noexcept { SetEnabled(!IsEnabled()); }

		constexpr bool HasFocusChanged() const noexcept { return focusChanged; }
		constexpr bool IsFocused() const noexcept { return focused; }
		virtual void SetFocused(bool focused) noexcept;

		constexpr bool HasHoverChanged() const noexcept { return hoverChanged; }
		constexpr bool IsHovered() const noexcept { return hovered; }
		virtual void SetHovered(bool hovered) noexcept;

		constexpr bool HasSizeChanged() const noexcept { return sizeChanged; }
		constexpr const glm::ivec2& GetSize() const noexcept { return size; }
		virtual void SetSize(const glm::ivec2& size) noexcept;

		constexpr bool HasPositionChanged() const noexcept { return positionChanged; }
		constexpr const glm::ivec2& GetPosition() const noexcept { return position; }
		virtual void SetPosition(const glm::ivec2& position) noexcept;
	public:
		// Make sure the imgui hash of this is always constant.
		// Ways to do this include not changing it, or by appending
		// three hashes (#) to the end of the title (which you can
		// then change at your leisure), followed by Panel::GetDefaultTitle().
		// Returns Panel::GetDefaultTitle() by default.
		constexpr virtual const std::string& GetTitle() const { return GetDefaultTitle(); }
	protected:
		// Called when enabled and should render. If ProvideDefaultWindow() returns
		// true, an imgui window will be opened for you with the title from GetTitle()
		// and closed after this function ends. To have multiple windows, simply call
		// ImGui::End() to end the provided window, and ImGui::Begin() to open a new
		// window. This new window will automatically be closed as well. You can do
		// this for as many windows as you need, within reasonable limits, of course.
		virtual void OnImGuiRender(int) = 0;

		constexpr virtual bool ProvideDefaultWindow() const { return true; }
		constexpr virtual ImGuiWindowFlags GetDefaultWindowFlags() const { return ImGuiWindowFlags_None; }

		constexpr EZ80IDELayer& GetIDE() noexcept { return *ez80IDELayer; }
		constexpr const EZ80IDELayer& GetIDE() const noexcept { return *ez80IDELayer; }

		// Updates enabled, focused, hovered, position, and size for the active imgui window (call between ImGui::Begin and ImGui::End).
		void Update(bool enabled);
	private:
		EZ80IDELayer* ez80IDELayer = nullptr;
		std::string title;
		glm::ivec2 size{ 1 };
		glm::ivec2 position{ 0 };

		bool enabled : 1 = false;
		bool focused : 1 = false;
		bool hovered : 1 = false;

		bool enabledChanged : 1 = false;
		bool focusChanged : 1 = false;
		bool hoverChanged : 1 = false;
		bool sizeChanged : 1 = false;
		bool positionChanged : 1 = false;
	};
}
#endif

#pragma once

#include "GBC/Core/Core.h"
#include "GBC/Events/Event.h"
#include <glm/glm.hpp>
#include <imgui/imgui.h>
#include <string>

namespace ide
{
	class EZ80IDELayer;

	class Panel
	{
	public:
		constexpr Panel() noexcept = default; // Needed to be stored in most stl types.
		constexpr Panel(EZ80IDELayer* ez80IDELayer, const std::string& title, bool enabled = true)
			: m_pEZ80IDELayer(ez80IDELayer), m_Title(title), m_Enabled(enabled) {}
		constexpr virtual ~Panel() = default;

		void OnImGuiRender();
		constexpr virtual void OnEvent(gbc::Event& event) {}

		constexpr const std::string& GetDefaultTitle() const noexcept { return m_Title; }

		constexpr bool HasEnabledChanged() const noexcept { return m_EnabledChanged; }
		constexpr bool IsEnabled() const noexcept { return m_Enabled; }
		virtual void SetEnabled(bool enabled) noexcept;
		void ToggleEnabled() noexcept { SetEnabled(!IsEnabled()); }

		constexpr bool HasFocusChanged() const noexcept { return m_FocusChanged; }
		constexpr bool IsFocused() const noexcept { return m_Focused; }
		virtual void SetFocused(bool focused) noexcept;

		constexpr bool HasHoverChanged() const noexcept { return m_HoverChanged; }
		constexpr bool IsHovered() const noexcept { return m_Hovered; }
		virtual void SetHovered(bool hovered) noexcept;

		constexpr bool HasSizeChanged() const noexcept { return m_SizeChanged; }
		constexpr const glm::ivec2& GetSize() const noexcept { return m_Size; }
		virtual void SetSize(const glm::ivec2& size) noexcept;

		constexpr bool HasPositionChanged() const noexcept { return m_PositionChanged; }
		constexpr const glm::ivec2& GetPosition() const noexcept { return m_Position; }
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

		constexpr EZ80IDELayer& GetIDE() noexcept { return *m_pEZ80IDELayer; }
		constexpr const EZ80IDELayer& GetIDE() const noexcept { return *m_pEZ80IDELayer; }

		// Updates enabled, focused, hovered, position, and size for the active imgui window (call between ImGui::Begin and ImGui::End).
		void Update(bool enabled);
	private:
		EZ80IDELayer* m_pEZ80IDELayer = nullptr;
		std::string m_Title;
		glm::ivec2 m_Size{ 1 };
		glm::ivec2 m_Position{ 0 };

		bool m_Enabled : 1 = false;
		bool m_Focused : 1 = false;
		bool m_Hovered : 1 = false;

		bool m_EnabledChanged : 1 = false;
		bool m_FocusChanged : 1 = false;
		bool m_HoverChanged : 1 = false;
		bool m_SizeChanged : 1 = false;
		bool m_PositionChanged : 1 = false;
	private:
		Panel(const Panel&) = delete;
		Panel(Panel&&) = delete;
		Panel& operator=(const Panel&) = delete;
		Panel& operator=(Panel&&) = delete;
	};
}

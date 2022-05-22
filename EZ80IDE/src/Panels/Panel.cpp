#include "Panel.h"
#include <imgui/imgui.h>

namespace ide
{
	void Panel::OnImGuiRender()
	{
		m_EnabledChanged = false;
		m_FocusChanged = false;
		m_HoverChanged = false;
		m_SizeChanged = false;
		m_PositionChanged = false;

		if (m_Enabled)
		{
			if (ProvideDefaultWindow())
			{
				bool enabled = m_Enabled;
				ImGui::Begin(GetTitle().c_str(), &enabled);
				Update(enabled);
				if (enabled)
					OnImGuiRender(0);
				ImGui::End();
			}
			else
				OnImGuiRender(0);
		}
	}

	void Panel::Update(bool enabled)
	{
		if (enabled)
		{
			SetFocused(ImGui::IsWindowFocused());
			SetHovered(ImGui::IsWindowHovered());
			SetSize(ImGui::GetContentRegionAvail());
			SetPosition(ImGui::GetWindowPos() + ImGui::GetCursorPos());
		}
		else
			SetEnabled(false);
	}

	void Panel::SetEnabled(bool enabled) noexcept
	{
		m_EnabledChanged = m_Enabled != enabled;
		if (m_EnabledChanged)
		{
			m_Enabled = enabled;
			if (!enabled)
			{
				m_Focused = false;
				m_Hovered = false;
			}
		}
	}

	void Panel::SetFocused(bool focused) noexcept
	{
		if (m_Enabled)
		{
			m_FocusChanged = m_Focused != focused;
			if (m_FocusChanged)
				m_Focused = focused;
		}
	}

	void Panel::SetHovered(bool hovered) noexcept
	{
		if (m_Enabled)
		{
			m_HoverChanged = m_Hovered != hovered;
			if (m_HoverChanged)
				m_Hovered = hovered;
		}
	}

	void Panel::SetSize(const glm::ivec2& size) noexcept
	{
		if (m_Enabled)
		{
			m_SizeChanged = size.x > 0 && size.y > 0 && m_Size.x != size.x || m_Size.y != size.y;
			if (m_SizeChanged)
				m_Size = size;
		}
	}

	void Panel::SetPosition(const glm::ivec2& position) noexcept
	{
		if (m_Enabled)
		{
			m_PositionChanged = m_Position.x != position.x || m_Position.y != position.y;
			if (m_PositionChanged)
				m_Position = position;
		}
	}
}

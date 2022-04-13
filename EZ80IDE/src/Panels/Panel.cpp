#include "Panel.h"
#if GBC_ENABLE_IMGUI
#include <imgui/imgui.h>

namespace gbc
{
	void Panel::OnImGuiRender()
	{
		enabledChanged = false;
		focusChanged = false;
		hoverChanged = false;
		sizeChanged = false;
		positionChanged = false;

		if (enabled)
		{
			if (ProvideDefaultWindow())
			{
				bool enabled = this->enabled;
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
		enabledChanged = this->enabled != enabled;
		if (enabledChanged)
		{
			this->enabled = enabled;
			if (!enabled)
			{
				focused = false;
				hovered = false;
			}
		}
	}

	void Panel::SetFocused(bool focused) noexcept
	{
		if (enabled)
		{
			focusChanged = this->focused != focused;
			if (focusChanged)
				this->focused = focused;
		}
	}

	void Panel::SetHovered(bool hovered) noexcept
	{
		if (enabled)
		{
			hoverChanged = this->hovered != hovered;
			if (hoverChanged)
				this->hovered = hovered;
		}
	}

	void Panel::SetSize(const glm::ivec2& size) noexcept
	{
		if (enabled)
		{
			sizeChanged = size.x > 0 && size.y > 0 && this->size.x != size.x || this->size.y != size.y;
			if (sizeChanged)
				this->size = size;
		}
	}

	void Panel::SetPosition(const glm::ivec2& position) noexcept
	{
		if (enabled)
		{
			positionChanged = this->position.x != position.x || this->position.y != position.y;
			if (positionChanged)
				this->position = position;
		}
	}
}
#endif

#pragma once

#include "GBC/Core/Core.h"
#if GBC_ENABLE_IMGUI
#include "GBC/Events/Event.h"

namespace gbc
{
	class ImGuiWrapper
	{
	public:
		ImGuiWrapper();
		~ImGuiWrapper();
	public:
		void Begin();
		void End();
	public:
		void OnEvent(Event& event);
		constexpr void SetBlockEvents(bool blockEvents) noexcept { this->blockEvents = blockEvents; }

		bool ImGuiHasWindows() const;
	private:
		bool blockEvents = false;
	};
}
#endif

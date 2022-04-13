#pragma once

#include "GBC/Core/Core.h"
#if GBC_ENABLE_IMGUI
#if GBC_ENABLE_PROFILE_RUNTIME
#include "Panels/Panel.h"

namespace gbc
{
	class ProfilingPanel : public Panel
	{
	public:
		ProfilingPanel(EZ80IDELayer* ez80IDELayer, const std::string& title);

		virtual void OnImGuiRender(int) override;
	private:
		bool profiling = false;
		uint32_t profileCount = 0;
	};
}
#endif
#endif

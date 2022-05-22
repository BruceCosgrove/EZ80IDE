#pragma once

#include "GBC/Core/Core.h"
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
		bool m_Profiling = false;
		uint32_t m_ProfileCount = 0;
	};
}
#endif

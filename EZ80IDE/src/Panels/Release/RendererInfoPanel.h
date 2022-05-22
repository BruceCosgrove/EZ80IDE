#pragma once

#include "GBC/Core/Core.h"
#if GBC_ENABLE_STATS
#include "Panels/Panel.h"

namespace gbc
{
	class RendererInfoPanel : public Panel
	{
	public:
		RendererInfoPanel(EZ80IDELayer* ez80IDELayer, const std::string& title);

		virtual void OnImGuiRender(int) override;
	private:
		const char* m_pVendor;
		const char* m_pRenderer;
		const char* m_pVersion;
	};
}
#endif

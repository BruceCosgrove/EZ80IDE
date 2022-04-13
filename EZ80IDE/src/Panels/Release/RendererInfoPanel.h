#pragma once

#include "GBC/Core/Core.h"
#if GBC_ENABLE_IMGUI
#include "Panels/Panel.h"

namespace gbc
{
	class RendererInfoPanel : public Panel
	{
	public:
		RendererInfoPanel(EZ80IDELayer* ez80IDELayer, const std::string& title);

		virtual void OnImGuiRender(int) override;
	private:
		const char* vendor;
		const char* renderer;
		const char* version;
	};
}
#endif
#pragma once

#include "GBC/Core/Core.h"
#if GBC_CONFIG_DEBUG
#include "Panels/Panel.h"

namespace ide
{
	class DemoPanel : public Panel
	{
	public:
		DemoPanel(EZ80IDELayer* ez80IDELayer, const std::string& title);

		virtual void OnImGuiRender(int) override;
	protected:
		constexpr virtual bool ProvideDefaultWindow() const override { return false; }
	};
}
#endif

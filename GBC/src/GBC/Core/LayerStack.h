#pragma once

#include "GBC/Core/Layer.h"
#include <vector>

namespace gbc
{
	class LayerStack
	{
	public:
		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);
		Layer* PopLayer();
		Layer* PopOverlay();

		constexpr auto begin() noexcept { return layers.begin(); }
		constexpr auto end() noexcept { return layers.end(); }
		constexpr auto rbegin() noexcept { return layers.rbegin(); }
		constexpr auto rend() noexcept { return layers.rend(); }
		constexpr auto cbegin() const noexcept { return layers.cbegin(); }
		constexpr auto cend() const noexcept { return layers.cend(); }
		constexpr auto crbegin() const noexcept { return layers.crbegin(); }
		constexpr auto crend() const noexcept { return layers.crend(); }
		constexpr auto begin() const noexcept { return layers.begin(); }
		constexpr auto end() const noexcept { return layers.end(); }
		constexpr auto rbegin() const noexcept { return layers.rbegin(); }
		constexpr auto rend() const noexcept { return layers.rend(); }
	private:
		std::vector<Layer*> layers;
		size_t layerInsertIndex = 0;
	};
}

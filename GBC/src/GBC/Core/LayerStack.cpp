#include "gbcpch.h"
#include "LayerStack.h"

namespace gbc
{
	void LayerStack::PushLayer(Layer* layer)
	{
		GBC_CORE_ASSERT(layer != nullptr, "Layer is nullptr!");
		GBC_CORE_ASSERT(std::find(layers.begin(), layers.begin() + layerInsertIndex, layer) == layers.begin() + layerInsertIndex, "Cannot add the same layer twice!");
		
		layers.emplace(layers.begin() + layerInsertIndex, layer);
		layerInsertIndex++;
	}

	void LayerStack::PushOverlay(Layer* overlay)
	{
		GBC_CORE_ASSERT(overlay != nullptr, "Overlay is nullptr!");
		GBC_CORE_ASSERT(std::find(layers.begin() + layerInsertIndex, layers.end(), overlay) == layers.end(), "Cannot add the same overlay twice!");

		layers.emplace_back(overlay);
	}

	Layer* LayerStack::PopLayer()
	{
		if (!layerInsertIndex)
			return nullptr;

		auto it = layers.begin() + --layerInsertIndex;
		Layer* layer = *it;
		layers.erase(it);
		return layer;
	}

	Layer* LayerStack::PopOverlay()
	{
		if (layerInsertIndex == layers.size())
			return nullptr;

		auto it = layers.end() - 1;
		Layer* overlay = *it;
		layers.erase(it);
		return overlay;
	}
}

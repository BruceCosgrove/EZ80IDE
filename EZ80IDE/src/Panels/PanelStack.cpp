#include "PanelStack.h"

namespace gbc
{
	PanelStack::~PanelStack() noexcept
	{
		Clear();
	}

	void PanelStack::OnEvent(Event& event)
	{
		if (!Empty())
		{
			auto panel = panels.back();
			if (panel->IsFocused())
				panel->OnEvent(event);
		}
	}

	void PanelStack::UpdateOrder() noexcept
	{
		size_t count = 0;
		for (auto it = begin(); it != end();)
		{
			Panel* panel = *it;
			if (panel->HasFocusChanged())
			{
				if (panel->IsFocused())
					(void)Focus(it);
				if (++count >= panels.size())
					break;
			}
			else if (panel->HasEnabledChanged() && closedPanels <= static_cast<size_t>(it - begin()))
			{
				if (!panel->IsEnabled())
					(void)Close(it);
				if (++count >= panels.size())
					break;
			}
			else
				++it;
		}

		changed = false;
	}

	void PanelStack::Push(Panel* panel)
	{
		panels.push_back(panel);
	}

	void PanelStack::Close() noexcept
	{
		if (!Empty() && panels.back()->IsEnabled())
		{
			// Mark for disabling so UpdateOrder doesn't try to reclose it.
			panels.back()->SetEnabled(false);
			changed = true;
		}
	}

	void PanelStack::RemoveClosedPanels() noexcept
	{
		for (auto it = begin(); it != begin() + closedPanels; ++it)
			delete *it;
		panels.erase(begin(), begin() + closedPanels);
		closedPanels = 0;
	}

	void PanelStack::Remove(size_t index) noexcept
	{
		if (index < Size())
		{
			auto it = begin() + index;
			Panel* panel = *it;
			if (!panel->IsEnabled())
				closedPanels--;
			delete panel;
			panels.erase(it);
		}
	}

	void PanelStack::Focus(size_t index) noexcept
	{
		if (index < Size())
		{
			Panel* panel = Focus(begin() + index);
			if (!panel->IsEnabled())
				closedPanels--;
			panel->SetEnabled(true);
			panel->SetFocused(true);
		}
	}

	void PanelStack::Clear() noexcept
	{
		for (auto panel : panels)
			delete panel;
		panels.clear();
		closedPanels = 0;
		changed = false;
	}

	size_t PanelStack::Size() const noexcept
	{
		return panels.size();
	}

	size_t PanelStack::GetOpenPanelCount() const noexcept
	{
		return Size() - closedPanels;
	}

	bool PanelStack::HasFocusedPanel() const noexcept
	{
		if (changed)
		{
			// If changed from most recent OnImGuiRender(), then simply checking
			// if the last panel is focused might be working with outdated data.
			for (auto it = rbegin(); it != rend() - closedPanels; ++it)
				if ((*it)->IsFocused())
					return true;
			return false;
		}

		return !Empty() && panels.back()->IsFocused();
	}

	bool PanelStack::Empty() const noexcept
	{
		return panels.empty();
	}

	Panel* PanelStack::operator[](size_t index) noexcept
	{
		return const_cast<Panel*>((*(const PanelStack*)this)[index]);
	}

	const Panel* PanelStack::operator[](size_t index) const noexcept
	{
		return !Empty() ? panels[index] : nullptr;
	}

	Panel* PanelStack::Peek() noexcept
	{
		return const_cast<Panel*>(((const PanelStack*)this)->Peek());
	}

	const Panel* PanelStack::Peek() const noexcept
	{
		return !Empty() ? panels.back() : nullptr;
	}

	Panel* PanelStack::Focus(std::vector<Panel*>::iterator it) noexcept
	{
		Panel* panel = *it;

		// Slide enabled panel up to the top of the stack.
		while (it != end() - 1)
		{
			auto next = it + 1;
			*it = *next;
			it = next;
		}

		*it = panel;
		return panel;
	}

	Panel* PanelStack::Close(std::vector<Panel*>::iterator it) noexcept
	{
		Panel* panel = *it;

		// Slide closed panel down to just before the first enabled panel.
		while (it != begin() + closedPanels)
		{
			auto prev = it - 1;
			*it = *prev;
			it = prev;
		}

		closedPanels++;
		*it = panel;
		return panel;
	}
}

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
			auto panel = m_Panels.back();
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
				if (++count >= m_Panels.size())
					break;
			}
			else if (panel->HasEnabledChanged() && m_ClosedPanels <= static_cast<size_t>(it - begin()))
			{
				if (!panel->IsEnabled())
					(void)Close(it);
				if (++count >= m_Panels.size())
					break;
			}
			else
				++it;
		}

		m_Changed = false;
	}

	void PanelStack::Push(Panel* panel)
	{
		m_Panels.push_back(panel);
	}

	void PanelStack::Close() noexcept
	{
		if (!Empty() && m_Panels.back()->IsEnabled())
		{
			// Mark for disabling so UpdateOrder doesn't try to reclose it.
			m_Panels.back()->SetEnabled(false);
			m_Changed = true;
		}
	}

	void PanelStack::RemoveClosedPanels() noexcept
	{
		for (auto it = begin(); it != begin() + m_ClosedPanels; ++it)
			delete *it;
		m_Panels.erase(begin(), begin() + m_ClosedPanels);
		m_ClosedPanels = 0;
	}

	void PanelStack::Remove(size_t index) noexcept
	{
		if (index < Size())
		{
			auto it = begin() + index;
			Panel* panel = *it;
			if (!panel->IsEnabled())
				m_ClosedPanels--;
			delete panel;
			m_Panels.erase(it);
		}
	}

	void PanelStack::Focus(size_t index) noexcept
	{
		if (index < Size())
		{
			Panel* panel = Focus(begin() + index);
			if (!panel->IsEnabled())
				m_ClosedPanels--;
			panel->SetEnabled(true);
			panel->SetFocused(true);
		}
	}

	void PanelStack::Clear() noexcept
	{
		for (auto panel : m_Panels)
			delete panel;
		m_Panels.clear();
		m_ClosedPanels = 0;
		m_Changed = false;
	}

	size_t PanelStack::Size() const noexcept
	{
		return m_Panels.size();
	}

	size_t PanelStack::GetOpenPanelCount() const noexcept
	{
		return Size() - m_ClosedPanels;
	}

	bool PanelStack::HasFocusedPanel() const noexcept
	{
		if (m_Changed)
		{
			// If changed from most recent OnImGuiRender(), then simply checking
			// if the last panel is focused might be working with outdated data.
			for (auto it = rbegin(); it != rend() - m_ClosedPanels; ++it)
				if ((*it)->IsFocused())
					return true;
			return false;
		}

		return !Empty() && m_Panels.back()->IsFocused();
	}

	bool PanelStack::Empty() const noexcept
	{
		return m_Panels.empty();
	}

	Panel* PanelStack::operator[](size_t index) noexcept
	{
		return const_cast<Panel*>((*(const PanelStack*)this)[index]);
	}

	const Panel* PanelStack::operator[](size_t index) const noexcept
	{
		return !Empty() ? m_Panels[index] : nullptr;
	}

	Panel* PanelStack::Peek() noexcept
	{
		return const_cast<Panel*>(((const PanelStack*)this)->Peek());
	}

	const Panel* PanelStack::Peek() const noexcept
	{
		return !Empty() ? m_Panels.back() : nullptr;
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
		while (it != begin() + m_ClosedPanels)
		{
			auto prev = it - 1;
			*it = *prev;
			it = prev;
		}

		m_ClosedPanels++;
		*it = panel;
		return panel;
	}
}

#pragma once

#include "Panel.h"

namespace ide
{
	// Keeps track of a list of panels. Will deallocate panels itself.
	// Orders panels with the most recently focused ones at the top (aka the back/end of the vector).
	class PanelStack
	{
	public:
		~PanelStack() noexcept;
	public:
		// Calls OnEvent for the focused panel, if present.
		void OnEvent(gbc::Event& event);

		// Updates the panel order.
		void UpdateOrder() noexcept;

		// Adds this panel to the top of the stack, treating it as if it were focused.
		void Push(Panel* panel);

		// Closes the topmost panel, but keeping it alive, if present.
		void Close() noexcept;

		// Removes all closed panels.
		void RemoveClosedPanels() noexcept;

		// Removes and deletes the panel at the given index, if present.
		void Remove(size_t index) noexcept;

		// Focuses and enables the panel at the given index, if present.
		void Focus(size_t index) noexcept;

		// Removes and deletes all panels.
		void Clear() noexcept;

		// Returns the number of focused, open, and closed panels.
		size_t Size() const noexcept;

		// Returns the number of focused and open panels.
		size_t GetOpenPanelCount() const noexcept;

		// Returns if the stack has a focused panel.
		bool HasFocusedPanel() const noexcept;

		// Returns if this stack has no panels, focused or otherwise.
		bool Empty() const noexcept;

		// Returns the panel at the given index, if present, otherwise nullptr.
		Panel* operator[](size_t index) noexcept;

		// Returns the panel at the given index, if present, otherwise nullptr.
		const Panel* operator[](size_t index) const noexcept;

		// Returns the topmost panel, if present, otherwise nullptr.
		Panel* Peek() noexcept;

		// Returns the topmost panel, if present, otherwise nullptr.
		const Panel* Peek() const noexcept;
	public: // Panel iterators if you need them.
		constexpr auto begin() noexcept { return m_Panels.begin(); }
		constexpr auto end() noexcept { return m_Panels.end(); }
		constexpr auto rbegin() noexcept { return m_Panels.rbegin(); }
		constexpr auto rend() noexcept { return m_Panels.rend(); }
		constexpr auto cbegin() const noexcept { return m_Panels.cbegin(); }
		constexpr auto cend() const noexcept { return m_Panels.cend(); }
		constexpr auto crbegin() const noexcept { return m_Panels.crbegin(); }
		constexpr auto crend() const noexcept { return m_Panels.crend(); }
		constexpr auto begin() const noexcept { return m_Panels.begin(); }
		constexpr auto end() const noexcept { return m_Panels.end(); }
		constexpr auto rbegin() const noexcept { return m_Panels.rbegin(); }
		constexpr auto rend() const noexcept { return m_Panels.rend(); }
	private:
		Panel* Focus(std::vector<Panel*>::iterator it) noexcept;
		Panel* Close(std::vector<Panel*>::iterator it) noexcept;
	private:
		std::vector<Panel*> m_Panels;
		size_t m_ClosedPanels = 0;
		bool m_Changed = false;
	};
}

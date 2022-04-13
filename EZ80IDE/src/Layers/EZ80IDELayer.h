#pragma once

// I only use this with absolute paths, so it's completely fine.
#include <xhash>
#include <filesystem>
_STD_BEGIN
template<>
struct hash<_STD filesystem::path> {
	_NODISCARD size_t operator()(const _STD filesystem::path& filepath) const noexcept {
		return hash<_STD filesystem::path::string_type>()(filepath.native());
	}
};
_STD_END

#include <gbc.h>
#include "Panels/PanelStack.h"

namespace gbc
{
	using IDEState = uint32_t;
	enum : IDEState
	{
		IDEState_None               = 0,
		IDEState_WorkspaceOpen      = 1 << 0,
		IDEState_FileOpen           = 1 << 1,
		IDEState_PopupOpen          = 1 << 2,

		// Automatically creates the ide state mask
		_IDEState_Last, IDEState_Mask = (_IDEState_Last << 1) - 3
	};

	class Panel;
	class FilePanel;
	class ExplorerPanel;

	class EZ80IDELayer : public Layer
	{
	public:
		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnUpdate(Timestep timestep) override;
		virtual void OnRender() override;
		virtual void OnImGuiRender() override;
		virtual void OnPostImGuiRender() override;
		virtual void OnEvent(Event& event) override;
	private:
		void OnWindowCloseEvent(WindowCloseEvent& event);
		void OnKeyPressEvent(KeyPressEvent& event);
	private: // State methods.
		// Checks if the current state is exactly the given state.
		constexpr bool IsState(IDEState state) const noexcept { return this->state == state; }

		// Checks if the current state has all of the include states and none of the exclude states.
		constexpr bool HasSubStates(IDEState includeStates, IDEState excludeStates = IDEState_None) const noexcept { return (state & includeStates) == includeStates && (state & excludeStates) == IDEState_None; }

		// Checks if the current state has at least one of the include states and none of the exclude states.
		constexpr bool HasAnySubStates(IDEState includeStates, IDEState excludeStates = IDEState_None) const noexcept { return !!(state & includeStates) && (state & excludeStates) == IDEState_None; }

		// Sets the current state.
		constexpr void SetState(IDEState state) noexcept { this->state = state & IDEState_Mask; }

		// Adds a state to the current state.
		constexpr void AddStates(IDEState states) noexcept { state |= states & IDEState_Mask; }

		// Removes a state from the current state.
		constexpr void RemoveStates(IDEState states) noexcept { state &= ~states & IDEState_Mask; }

		// Toggles a state in the current state.
		constexpr void ToggleStates(IDEState states) noexcept { state ^= states & IDEState_Mask; }
	private:
		IDEState state = IDEState_None;
	private: // Operations.
		// Opens a new file dialog and creates a new file panel.
		// TODO: create custom modal popup that only allows them to go up to the src directory.
		void NewFile();

		// Opens a modal popup to create a new folder.
		void NewFolder();

		// Opens a workspace, closes the current one first if one is open.
		void OpenWorkspace();

		// Saves the most recently focused file panel.
		void SaveFile();

		// Saves all open file panels.
		void SaveAll();

		// Closes the most recently focused file panel.
		void CloseFile();

		// Closes the active workspace, if present.
		void CloseWorkspace();

		// Saves a workspace's metadata and clears workspace data cache.
		void SaveAndCloseWorkspace();

		// Sets the active workspace directory. Saves the active workspace, if present.
		void SetWorkspaceDirectory(const std::filesystem::path& workspaceDirectory);

		// Gets the active workspace directory.
		constexpr const std::filesystem::path& GetWorkspaceDirectory() const noexcept { return workspaceDirectory; }
	private: // Filepath caches.
		std::filesystem::path workspaceDirectory;
		std::filesystem::path metaDataDirectory;
		std::filesystem::path srcDirectory;
		std::filesystem::path binDirectory;
		std::filesystem::path titleIDsFilepath;
		std::string imguiIniFilepathString;
	private: // Popup methods.
		// Returns true as long as the popup is open.
		bool OpenPopup(const char* name, ImGuiPopupFlags flags = ImGuiPopupFlags_None);

		// Returns true if any popup is open.
		bool IsAnyPopupOpen();

		// Returns true if the popup is open.
		bool IsPopupOpen(const char* name, ImGuiPopupFlags flags = ImGuiPopupFlags_None);

		// Closes the open popup.
		void ClosePopup(const char* name);
	private: // File panel methods.
		// Get the default title for a file panel.
		const std::string& GetFilePanelTitle(const std::filesystem::path& filepath, uint64_t id);

		// Rename a file panel.
		void RenameFilePanel(const std::filesystem::path& filepathOld, const std::filesystem::path& filepathNew);

		// Closes a file panel.
		void RemoveFilePanel(const std::filesystem::path& filepath);

		// Closes a file panel and deletes its file.
		void DeleteFilePanel(const std::filesystem::path& filepath);

		// Closes all file panels whose files are in the given directory,
		// then deletes the directory and all its contents.
		void DeleteFolder(const std::filesystem::path& filepath);

		// Focuses a file panel, and opens it if it was closed.
		bool FocusFilePanel(const std::filesystem::path& filepath);

		// Focuses a file panel if it exists, otherwise creates a new file panel and focuses it.
		void FocusOrAddFilePanel(const std::filesystem::path& filepath);
	private: // Cached title id info for file panels.
		std::unordered_map<std::filesystem::path, std::string> titleIDs;
		uint64_t nextTitleID = 0;
	private:
		void UI_Dockspace();
		void UI_MenuBar();

		template<typename T> T* AddPanel(const std::string& name);
		PanelStack panels;

		friend class ExplorerPanel;
		ExplorerPanel* explorerPanel = nullptr;

		friend class FilePanel;
		FilePanel* AddFilePanel(const std::filesystem::path& filepath, uint64_t id = 0);
		PanelStack filePanels;
	};
}

#include "EZ80IDELayer.inl"

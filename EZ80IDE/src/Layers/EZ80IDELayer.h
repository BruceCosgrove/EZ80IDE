#pragma once

#include <gbc.h>
#include "Panels/PanelStack.h"
#include "Emulation/EmulatorThread.h"

namespace ide
{
	using IDEState = uint32_t;
	enum : IDEState
	{
		IDEState_None               = 0,
		IDEState_WorkspaceOpen      = 1 << 0,
		IDEState_FileOpen           = 1 << 1,
		IDEState_FileFocused        = 1 << 2,
		IDEState_ExplorerFocused    = 1 << 3,
		IDEState_PopupOpen          = 1 << 4,
		IDEState_ModalPopup         = 1 << 5,
		IDEState_ROMLoaded          = 1 << 6,
		IDEState_EmulatorRunning    = 1 << 7,

		// Automatically creates the ide state mask
		_IDEState_Last, IDEState_Mask = (_IDEState_Last << 1) - 3
	};

	class Panel;
	class FilePanel;
	class ExplorerPanel;
	namespace emu
	{
		class LCDPanel;
	}

	class EZ80IDELayer : public gbc::Layer
	{
	public:
		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnUpdate(gbc::Timestep timestep) override;
		virtual void OnRender() override;
		virtual void OnImGuiRender() override;
		virtual void OnPostImGuiRender() override;
		virtual void OnEvent(gbc::Event& event) override;
	private:
		void OnWindowCloseEvent(gbc::WindowCloseEvent& event);
		void OnKeyPressEvent(gbc::KeyPressEvent& event);
	public: // State methods.
		// Checks if the current state is exactly the given state.
		constexpr bool IsState(IDEState state) const noexcept { return m_State == state; }

		// Checks if the current state has all of the include states and none of the exclude states.
		constexpr bool HasSubStates(IDEState includeStates, IDEState excludeStates = IDEState_None) const noexcept { return (m_State & includeStates) == includeStates && (m_State & excludeStates) == IDEState_None; }

		// Checks if the current state has at least one of the include states and none of the exclude states.
		constexpr bool HasAnySubStates(IDEState includeStates, IDEState excludeStates = IDEState_None) const noexcept { return !!(m_State & includeStates) && (m_State & excludeStates) == IDEState_None; }

		// Sets the current state.
		constexpr void SetState(IDEState state) noexcept { m_State = state & IDEState_Mask; }

		// Adds or removes the given states depending on the condition.
		constexpr void SetStates(IDEState states, bool condition) noexcept { if (condition) AddStates(states); else RemoveStates(states); }

		// Adds a state to the current state.
		constexpr void AddStates(IDEState states) noexcept { m_State |= states & IDEState_Mask; }

		// Removes a state from the current state.
		constexpr void RemoveStates(IDEState states) noexcept { m_State &= ~states & IDEState_Mask; }

		// Toggles a state in the current state.
		constexpr void ToggleStates(IDEState states) noexcept { m_State ^= states & IDEState_Mask; }
	private:
		IDEState m_State = IDEState_None;
	public: // Operations.
		// Opens a modal popup to create a new file.
		void NewFile();

		// Opens a modal popup to create a new folder.
		void NewFolder();

		// Opens a file dialog to select a new workspace, closes the current one first, if present.
		void OpenWorkspace();

		// Saves the most recently focused file panel.
		void SaveFile();

		// Saves all open file panels.
		void SaveAll();

		// Renames the selected file/folder if the explorer is focused, otherwise renames the most recently focused file panel.
		void Rename();

		// Deletes the selected file/folder if the explorer is focused.
		void Delete();

		// Closes the most recently focused file panel.
		void CloseFile();

		// Closes the active workspace, if present.
		void CloseWorkspace();
	public: // Menubar/Calculator
		bool LoadROM();
		void UnloadROM();
	public:
		// Sets the user's clipboard to a utf-8 encoded string converted from the given filepath.
		void CopyPath(const std::filesystem::path& filepath);

		// Sets the user's clipboard to a utf-8 encoded string converted from the given filepath, relative to the working directory.
		void CopyRelativePath(const std::filesystem::path& filepath);
	private:
		// Saves a workspace's metadata and clears workspace data cache.
		void SaveAndCloseWorkspace();

		// Sets the active workspace directory. Saves the active workspace, if present.
		void SetWorkspaceDirectory(const std::filesystem::path& workspaceDirectory);

		// Writes workspace metadata.
		void SerializeWorkspace();

		// Reads workspace metadata.
		void DeserializeWorkspace();
	public:
		inline const std::filesystem::path& GetWorkspaceDirectory() const noexcept { return m_WorkspaceDirectory; }
		inline const std::filesystem::path& GetMetaDataDirectory() const noexcept { return m_MetaDataDirectory; }
		inline const std::filesystem::path& GetSrcDirectory() const noexcept { return m_SrcDirectory; }
		inline const std::filesystem::path& GetBinDirectory() const noexcept { return m_BinDirectory; }
		inline const std::filesystem::path& GetCalcDirectory() const noexcept { return m_CalcDirectory; }
	private: // Filepath caches.
		std::filesystem::path m_WorkspaceDirectory;
		std::filesystem::path m_MetaDataDirectory;
		std::filesystem::path m_SrcDirectory;
		std::filesystem::path m_BinDirectory;
		std::filesystem::path m_CalcDirectory; // Stores ram files, and I want to say a copy of the rom file.

		std::filesystem::path m_MetaDataFilepath;
		std::filesystem::path m_ROMFilepath;
		std::string m_ImguiIniFilepathString;
	public: // Popup methods.
		// Returns true as long as the popup is open.
		bool OpenPopup(const char* name, ImGuiPopupFlags flags = ImGuiPopupFlags_None);

		// Returns true if any popup is open.
		bool IsAnyPopupOpen();

		// Returns true if the popup is open.
		bool IsPopupOpen(const char* name, ImGuiPopupFlags flags = ImGuiPopupFlags_None);

		// Closes the open popup.
		void ClosePopup(const char* name);
	public: // File panel methods.
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
		std::unordered_map<std::filesystem::path, std::string> m_TitleIDs;
		uint64_t m_NextTitleID = 0;
	private:
		void UI_Dockspace();
		void UI_MenuBar();

		void UI_Popup_NewFile();
		void UI_Popup_NewFolder();
		void UI_Popup_Rename();
		void UI_Popup_ConfirmDelete();

		void(EZ80IDELayer::* m_pOpenPopup)() = nullptr;

		uint32_t m_NewFileSelectedExtension = 0;
		uint32_t m_NewFilePendingExtension = 0;
		char m_NewFilenameBuffer[__std_fs_max_path];
		std::filesystem::path m_NewFileInitialDirectory;
		std::string m_NewFileInitialDirectoryString;

		bool m_RenamingFolder = false;
		bool m_RenamingOpenFile = false;

		std::string m_DeletingMessage;
		bool m_DeletingFolder = false;
	private:
		ExplorerPanel* m_pExplorerPanel = nullptr;
		emu::LCDPanel* m_pLCDPanel = nullptr;

		template<typename T> T* AddPanel(const std::string& name);
		PanelStack m_Panels;

		FilePanel* AddFilePanel(const std::filesystem::path& filepath, uint64_t id = 0);
		PanelStack m_FilePanels;
	public:
		inline emu::EmulatorThread& GetEmulatorThread() { return m_EmulatorThread; }
	private:
		emu::EmulatorThread m_EmulatorThread;
	};
}

#include "EZ80IDELayer.inl"

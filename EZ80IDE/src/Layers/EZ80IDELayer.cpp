#include "EZ80IDELayer.h"
#include "GBC/Util/Util.h"
#include "Panels/Dist/FilePanel.h"
#include "Panels/Dist/ExplorerPanel.h"
#include "Panels/Dist/LCDPanel.h"
#include "Panels/Release/RendererInfoPanel.h"
#include "Panels/Debug/ProfilingPanel.h"
#include "Panels/Debug/DemoPanel.h"
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <yaml-cpp/yaml.h>
#include <fstream>

namespace ide
{
	void EZ80IDELayer::OnAttach()
	{
		GBC_PROFILE_FUNCTION();

		ImGuiIO& io = ImGui::GetIO();
		io.IniFilename = nullptr;
		constexpr char defaultImGuiIniData[] = {
R"(
[Window][Dockspace]
Pos=0,0
Size=1920,1017
Collapsed=0

[Window][Explorer]
Pos=0,26
Size=329,991
Collapsed=0
DockId=0x00000001,0

[Window][LCD]
Pos=1592,26
Size=328,991
Collapsed=0
DockId=0x00000004,0

[Docking][Data]
DockSpace     ID=0x33675C32 Window=0x5B816B74 Pos=0,49 Size=1920,991 Split=X
  DockNode    ID=0x00000003 Parent=0x33675C32 SizeRef=1590,991 Split=X
	DockNode  ID=0x00000001 Parent=0x00000003 SizeRef=329,874 Selected=0x1E89CEB8
	DockNode  ID=0x00000002 Parent=0x00000003 SizeRef=1269,874 CentralNode=1
  DockNode    ID=0x00000004 Parent=0x33675C32 SizeRef=328,991 Selected=0x2C2434F9
)"
		};
		ImGui::LoadIniSettingsFromMemory(defaultImGuiIniData, sizeof(defaultImGuiIniData));

#if GBC_CONFIG_DEBUG
		AddPanel<DemoPanel>("Demo")->SetEnabled(false);
#endif
#if GBC_ENABLE_PROFILE_RUNTIME
		AddPanel<ProfilingPanel>("Profiling")->SetEnabled(false);
#endif
#if GBC_ENABLE_STATS
		AddPanel<RendererInfoPanel>("Renderer Info")->SetEnabled(false);
#endif

		m_pExplorerPanel = AddPanel<ExplorerPanel>("Explorer");
		m_pLCDPanel = AddPanel<emu::LCDPanel>("LCD");

		m_EmulatorThread.SetLCDTexture(m_pLCDPanel->GetLocalLCDTexture());
	}

	void EZ80IDELayer::OnDetach()
	{
		GBC_PROFILE_FUNCTION();

		SaveAndCloseWorkspace();
	}

	void EZ80IDELayer::OnUpdate(gbc::Timestep timestep)
	{
		GBC_PROFILE_FUNCTION();

		//bool viewportFocused = sceneViewportPanel->IsFocused();
		//bool viewportHovered = sceneViewportPanel->IsHovered();

		// TODO: Set this depending on state.
		//Application::Get().GetImGuiWrapper().SetBlockEvents(!sceneViewportPanel->IsFocused() && !sceneViewportPanel->IsHovered());

		m_Panels.UpdateOrder();
		SetStates(IDEState_ExplorerFocused, m_pExplorerPanel->IsFocused());

		m_FilePanels.UpdateOrder();
		SetStates(IDEState_FileFocused, m_FilePanels.HasFocusedPanel());
	}

	void EZ80IDELayer::OnRender()
	{
		GBC_PROFILE_FUNCTION();

		gbc::Renderer::SetClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		gbc::Renderer::Clear();
	}

	void EZ80IDELayer::OnImGuiRender()
	{
		GBC_PROFILE_FUNCTION();

		UI_Dockspace();

		for (auto panel : m_Panels)
			panel->OnImGuiRender();
		for (auto filePanel : m_FilePanels)
			filePanel->OnImGuiRender();

		if (m_pOpenPopup)
			(this->*m_pOpenPopup)();

		ImGui::SetNextWindowSize(ImVec2(400, 400), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowPos(ImVec2(2360, 0), ImGuiCond_FirstUseEver);
		if (ImGui::Begin("Panel Debug", nullptr, ImGuiWindowFlags_NoSavedSettings))
		{
#define DEBUG_DRAW_PANEL_STACK(name, stack) \
			ImGui::Text(name); \
			gbc::ImGuiHelper::BeginTable(name, 3, gbc::ImGuiHelper::defaultTableFlags | ImGuiTableFlags_Borders); \
			for (auto it = stack.crbegin(); it != stack.crend(); ++it) \
			{ \
				Panel* panel = *it; \
				ImGui::Text(panel->IsEnabled() ? "Enabled" : "Disabled"); \
				gbc::ImGuiHelper::NextTableColumn(); \
				gbc::ImGuiHelper::Text(panel->IsFocused() ? "Focused" : "Unfocused", panel->GetTitle().c_str()); \
				if (it != stack.crend() - 1) \
					gbc::ImGuiHelper::NextTableColumn(); \
			} \
			gbc::ImGuiHelper::EndTable();

			DEBUG_DRAW_PANEL_STACK("File Panels", m_FilePanels);
			ImGui::NewLine();
			DEBUG_DRAW_PANEL_STACK("Other Panels", m_Panels);
#undef DEBUG_DRAW_PANEL_STACK
			ImGui::End();
		}

		ImGui::SetNextWindowSize(ImVec2(400, 400), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowPos(ImVec2(2760, 0), ImGuiCond_FirstUseEver);
		if (ImGui::Begin("State Debug", nullptr, ImGuiWindowFlags_NoSavedSettings))
		{
#define DEBUG_DRAW_STATE(x) ImGui::Text(#x "=%d", !!(m_State & IDEState_##x))
			ImGui::Text("Generic");
			ImGui::Indent();
			DEBUG_DRAW_STATE(WorkspaceOpen);
			DEBUG_DRAW_STATE(FileOpen);
			DEBUG_DRAW_STATE(FileFocused);
			DEBUG_DRAW_STATE(ExplorerFocused);
			DEBUG_DRAW_STATE(PopupOpen);
			DEBUG_DRAW_STATE(ModalPopup);
			DEBUG_DRAW_STATE(ROMLoaded);
			DEBUG_DRAW_STATE(EmulatorRunning);
			ImGui::Unindent();
#undef DEBUG_DRAW_STATE
			ImGui::End();
		}
	}

	void EZ80IDELayer::OnPostImGuiRender()
	{

	}

	void EZ80IDELayer::UI_Dockspace()
	{
		ImGuiWindowFlags dockspaceFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking
			| ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize
			| ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->GetWorkPos());
		ImGui::SetNextWindowSize(viewport->GetWorkSize());
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0.0f });
		ImGui::Begin("Dockspace", nullptr, dockspaceFlags);
		ImGui::PopStyleVar(2);

		ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, { 200.0f, ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2.0f });
		ImGui::DockSpace(ImGui::GetID("Dockspace"));
		ImGui::PopStyleVar();

		UI_MenuBar();
		ImGui::End();
	}

	void EZ80IDELayer::UI_MenuBar()
	{
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Open Workspace...", "Ctrl+O", false, HasSubStates(IDEState_None, IDEState_ModalPopup)))
					OpenWorkspace();
				if (ImGui::MenuItem("New File...", "Ctrl+N", false, HasSubStates(IDEState_WorkspaceOpen, IDEState_ModalPopup)))
					NewFile();
				if (ImGui::MenuItem("New Folder...", "Ctrl+Shift+N", false, HasSubStates(IDEState_WorkspaceOpen, IDEState_ModalPopup)))
					NewFolder();
				if (ImGui::MenuItem("Save File", "Ctrl+S", false, HasSubStates(IDEState_WorkspaceOpen | IDEState_FileOpen, IDEState_ModalPopup)))
					SaveFile();
				if (ImGui::MenuItem("Save All", "Ctrl+Shift+S", false, HasSubStates(IDEState_WorkspaceOpen | IDEState_FileOpen, IDEState_ModalPopup)))
					SaveAll();
				if (ImGui::MenuItem("Rename...", "Ctrl+R", false, HasSubStates(IDEState_WorkspaceOpen, IDEState_ModalPopup)))
					Rename();
				if (ImGui::MenuItem("Close File", "Ctrl+W", false, HasSubStates(IDEState_WorkspaceOpen | IDEState_FileOpen, IDEState_ModalPopup)))
					CloseFile();
				if (ImGui::MenuItem("Close Workspace", "Ctrl+Shift+W", false, HasSubStates(IDEState_WorkspaceOpen, IDEState_ModalPopup)))
					CloseWorkspace();

				ImGui::Separator();

				if (ImGui::MenuItem("Exit", "Alt+F4"))
					gbc::Application::Get().Close();

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("View"))
			{
				if (ImGui::BeginMenu("Panels"))
				{
					for (auto panel : m_Panels)
						if (ImGui::MenuItem(panel->GetTitle().c_str(), nullptr, panel->IsEnabled()))
							panel->ToggleEnabled();
					ImGui::EndMenu();
				}

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Calculator"))
			{
				if (ImGui::MenuItem("Load ROM", nullptr, false, HasSubStates(IDEState_WorkspaceOpen, IDEState_ModalPopup | IDEState_ROMLoaded)))
				{
					if (LoadROM())
						GBC_INFO("Loaded ROM Image from \"{0}\".", m_ROMFilepath.string());
					else
						GBC_WARN("Failed to load ROM image from \"{0}\".", m_ROMFilepath.string());
				}

				if (ImGui::MenuItem("Unload ROM", nullptr, false, HasSubStates(IDEState_WorkspaceOpen | IDEState_ROMLoaded, IDEState_ModalPopup)))
				{
					GBC_INFO("Unloading ROM Image from \"{0}\"...", m_ROMFilepath.string());
					UnloadROM();
				}

				bool running = HasSubStates(IDEState_EmulatorRunning);
				if (ImGui::MenuItem("Emulation Running", nullptr, &running, HasSubStates(IDEState_WorkspaceOpen | IDEState_ROMLoaded)))
				{
					ToggleStates(IDEState_EmulatorRunning);
					if (running)
						m_EmulatorThread.Start();
					else
						m_EmulatorThread.Stop();
				}

				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}
	}

	void EZ80IDELayer::UI_Popup_NewFile()
	{
		static constexpr char title[] = "New File";
		static constexpr ImGuiPopupFlags popupFlags = ImGuiPopupFlags_NoOpenOverExistingPopup;
		static constexpr ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings;

		ImGui::OpenPopup(title, popupFlags);

		static constexpr const char* extensions[]{ ".ez80", ".inc", ".asm" };
		static bool firstFrame = true;
		static bool secondFrame = false;
		bool closed = false;
		bool create = false;

		ImGuiStyle& style = ImGui::GetStyle();
		ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, { 0.5f });
		ImGui::SetNextWindowSize(ImVec2{ 300.0f, 0.0f }, ImGuiCond_Appearing);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowTitleAlign, ImVec2{ 0.5f, ImGui::GetStyle().WindowTitleAlign.y });
		if (ImGui::BeginPopupModal(title, nullptr, windowFlags))
		{
			if (gbc::ImGuiHelper::BeginTable("NewFilePopupTable", 2, ImGuiTableFlags_None))
			{
				if (secondFrame)
					ImGui::SetKeyboardFocusHere();
				gbc::ImGuiHelper::Text("Directory", m_NewFileInitialDirectoryString.c_str());
				gbc::ImGuiHelper::NextTableColumn();

				(void)gbc::ImGuiHelper::InputText("Name", m_NewFilenameBuffer, sizeof(m_NewFilenameBuffer) / sizeof(*m_NewFilenameBuffer));
				gbc::ImGuiHelper::NextTableColumn();

				(void)gbc::ImGuiHelper::Combo("Extension", &m_NewFilePendingExtension, extensions, sizeof(extensions) / sizeof(*extensions));

				gbc::ImGuiHelper::EndTable();
			}

			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { style.ItemSpacing.x * 0.5f, style.ItemSpacing.y });
			ImVec2 buttonSize{ (ImGui::GetContentRegionAvail().x - style.FramePadding.x) * 0.5f, 0.0f };

			if (ImGui::Button("Create", buttonSize) || gbc::Input::IsKeyPressed(gbc::Keycode::Enter))
			{
				closed = true;
				create = !!*m_NewFilenameBuffer;
			}

			ImGui::SameLine();

			if (ImGui::Button("Cancel", buttonSize) || gbc::Input::IsKeyPressed(gbc::Keycode::Escape))
				closed = true;

			ImGui::PopStyleVar();

			if (closed)
			{
				ImGui::CloseCurrentPopup();
				RemoveStates(IDEState_PopupOpen | IDEState_ModalPopup);
				m_pOpenPopup = nullptr;
				firstFrame = true;
				secondFrame = false;
			}
			else if (firstFrame)
			{
				firstFrame = false;
				secondFrame = true;
			}
			else if (secondFrame)
				secondFrame = false;

			if (create)
			{
				m_NewFileSelectedExtension = m_NewFilePendingExtension;

				std::filesystem::path filepath = m_WorkspaceDirectory;
				filepath /= m_NewFileInitialDirectory;
				filepath /= (char*)m_NewFilenameBuffer;
				filepath.replace_extension(extensions[m_NewFileSelectedExtension]);
				FocusOrAddFilePanel(filepath);
			}

			ImGui::EndPopup();
		}
		ImGui::PopStyleVar();
	}

	void EZ80IDELayer::UI_Popup_NewFolder()
	{
		static constexpr char title[] = "New Folder";
		static constexpr ImGuiPopupFlags popupFlags = ImGuiPopupFlags_NoOpenOverExistingPopup;
		static constexpr ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings;

		ImGui::OpenPopup(title, popupFlags);

		static bool firstFrame = true;
		static bool secondFrame = false;
		bool closed = false;
		bool create = false;

		ImGuiStyle& style = ImGui::GetStyle();
		ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, { 0.5f });
		ImGui::SetNextWindowSize(ImVec2{ 300.0f, 0.0f }, ImGuiCond_Appearing);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowTitleAlign, ImVec2{ 0.5f, ImGui::GetStyle().WindowTitleAlign.y });
		if (ImGui::BeginPopupModal(title, nullptr, windowFlags))
		{
			if (gbc::ImGuiHelper::BeginTable("NewFolderPopupTable", 2, ImGuiTableFlags_None))
			{
				if (secondFrame)
					ImGui::SetKeyboardFocusHere();
				gbc::ImGuiHelper::Text("Directory", m_NewFileInitialDirectoryString.c_str());
				gbc::ImGuiHelper::NextTableColumn();

				(void)gbc::ImGuiHelper::InputText("Name", m_NewFilenameBuffer, sizeof(m_NewFilenameBuffer) / sizeof(*m_NewFilenameBuffer));

				gbc::ImGuiHelper::EndTable();
			}

			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { style.ItemSpacing.x * 0.5f, style.ItemSpacing.y });
			ImVec2 buttonSize{ (ImGui::GetContentRegionAvail().x - style.FramePadding.x) * 0.5f, 0.0f };

			if (ImGui::Button("Create", buttonSize) || gbc::Input::IsKeyPressed(gbc::Keycode::Enter))
			{
				closed = true;
				create = !!*m_NewFilenameBuffer;
			}

			ImGui::SameLine();

			if (ImGui::Button("Cancel", buttonSize) || gbc::Input::IsKeyPressed(gbc::Keycode::Escape))
				closed = true;

			ImGui::PopStyleVar();

			if (closed)
			{
				ImGui::CloseCurrentPopup();
				RemoveStates(IDEState_PopupOpen | IDEState_ModalPopup);
				m_pOpenPopup = nullptr;
				firstFrame = true;
				secondFrame = false;
			}
			else if (firstFrame)
			{
				firstFrame = false;
				secondFrame = true;
			}
			else if (secondFrame)
				secondFrame = false;

			if (create)
			{
				std::filesystem::path filepath = m_WorkspaceDirectory;
				filepath /= m_NewFileInitialDirectory;
				filepath /= (char*)m_NewFilenameBuffer;
				bool status = gbc::FileIO::MakeDirectoryIfNotExists(filepath);
				GBC_ASSERT(status, "Failed to create new directory.");
			}

			ImGui::EndPopup();
		}
		ImGui::PopStyleVar();
	}

	void EZ80IDELayer::UI_Popup_Rename()
	{
		static constexpr char titleFile[] = "Rename File";
		static constexpr char titleFolder[] = "Rename Folder";
		const char* title = m_RenamingFolder ? titleFolder : titleFile;

		static constexpr ImGuiPopupFlags popupFlags = ImGuiPopupFlags_NoOpenOverExistingPopup;
		static constexpr ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings;

		ImGui::OpenPopup(title, popupFlags);

		static constexpr const char* extensions[]{ ".ez80", ".inc", ".asm" };
		static bool firstFrame = true;
		static bool secondFrame = false;
		bool closed = false;
		bool rename = false;

		ImGuiStyle& style = ImGui::GetStyle();
		ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, { 0.5f });
		ImGui::SetNextWindowSize(ImVec2{ 300.0f, 0.0f }, ImGuiCond_Appearing);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowTitleAlign, ImVec2{ 0.5f, ImGui::GetStyle().WindowTitleAlign.y });
		if (ImGui::BeginPopupModal(title, nullptr, windowFlags))
		{
			if (gbc::ImGuiHelper::BeginTable("RenamePopupTable", 2, ImGuiTableFlags_None))
			{
				if (secondFrame)
					ImGui::SetKeyboardFocusHere();
				gbc::ImGuiHelper::Text("Directory", m_NewFileInitialDirectoryString.c_str());
				gbc::ImGuiHelper::NextTableColumn();

				(void)gbc::ImGuiHelper::InputText("Name", m_NewFilenameBuffer, sizeof(m_NewFilenameBuffer) / sizeof(*m_NewFilenameBuffer));

				if (!m_RenamingFolder)
				{
					gbc::ImGuiHelper::NextTableColumn();
					(void)gbc::ImGuiHelper::Combo("Extension", &m_NewFilePendingExtension, extensions, sizeof(extensions) / sizeof(*extensions));
				}

				gbc::ImGuiHelper::EndTable();
			}

			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { style.ItemSpacing.x * 0.5f, style.ItemSpacing.y });
			ImVec2 buttonSize{ (ImGui::GetContentRegionAvail().x - style.FramePadding.x) * 0.5f, 0.0f };

			if (ImGui::Button("Rename", buttonSize) || gbc::Input::IsKeyPressed(gbc::Keycode::Enter))
			{
				closed = true;
				rename = !!*m_NewFilenameBuffer; // rename if m_NewFilenameBuffer is not empty
			}

			ImGui::SameLine();

			if (ImGui::Button("Cancel", buttonSize) || gbc::Input::IsKeyPressed(gbc::Keycode::Escape))
				closed = true;

			ImGui::PopStyleVar();

			if (closed)
			{
				ImGui::CloseCurrentPopup();
				RemoveStates(IDEState_PopupOpen | IDEState_ModalPopup);
				m_pOpenPopup = nullptr;
				firstFrame = true;
				secondFrame = false;
			}
			else if (firstFrame)
			{
				firstFrame = false;
				secondFrame = true;
			}
			else if (secondFrame)
				secondFrame = false;

			if (rename)
			{
				auto filepathNew = m_WorkspaceDirectory / m_NewFileInitialDirectory;
				filepathNew /= (char*)m_NewFilenameBuffer;

				if (m_RenamingFolder)
				{
					auto filepathOld = m_WorkspaceDirectory / m_NewFileInitialDirectory;
					bool status = gbc::FileIO::RenameDirectory(filepathOld, filepathNew);
					GBC_ASSERT(status, "Failed to rename directory.");

					for (auto panel : m_FilePanels)
					{
						FilePanel* filePanel = (FilePanel*)panel;
						if (gbc::FileIO::IsAncestorOf(filepathOld, filePanel->GetFilepath()))
						{
							auto relative = gbc::FileIO::Relative(filePanel->GetFilepath(), filepathOld);
							RenameFilePanel(filePanel->GetFilepath(), filepathNew / relative);
						}
					}
				}
				else
				{
					m_NewFileSelectedExtension = m_NewFilePendingExtension;
					filepathNew.replace_extension(extensions[m_NewFileSelectedExtension]);

					if (m_RenamingOpenFile)
						RenameFilePanel(((FilePanel*)m_FilePanels.Peek())->GetFilepath(), filepathNew);
					else
					{
						bool status = gbc::FileIO::RenameFile(m_pExplorerPanel->GetSelectedFilepath(), filepathNew);
						GBC_ASSERT(status, "Failed to rename file.");
					}
				}
			}

			ImGui::EndPopup();
		}
		ImGui::PopStyleVar();
	}

	void EZ80IDELayer::UI_Popup_ConfirmDelete()
	{
		static constexpr char titleFile[] = "Delete File";
		static constexpr char titleFolder[] = "Delete Folder";
		const char* title = m_DeletingFolder ? titleFolder : titleFile;

		static constexpr ImGuiPopupFlags popupFlags = ImGuiPopupFlags_NoOpenOverExistingPopup;
		static constexpr ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings;

		ImGui::OpenPopup(title, popupFlags);

		bool closed = false;
		bool delet3 = false;

		ImGuiStyle& style = ImGui::GetStyle();
		ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, { 0.5f });
		ImGui::SetNextWindowSize(ImVec2{ 300.0f, 0.0f }, ImGuiCond_Appearing);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowTitleAlign, ImVec2{ 0.5f, ImGui::GetStyle().WindowTitleAlign.y });
		if (ImGui::BeginPopupModal(title, nullptr, windowFlags))
		{
			ImGui::Text("Are you sure you want to delete:\n%s?", m_DeletingMessage.c_str());

			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { style.ItemSpacing.x * 0.5f, style.ItemSpacing.y });
			ImVec2 buttonSize{ (ImGui::GetContentRegionAvail().x - style.FramePadding.x) * 0.5f, 0.0f };

			if (ImGui::Button("Delete", buttonSize) || gbc::Input::IsKeyPressed(gbc::Keycode::Enter))
			{
				closed = true;
				delet3 = true;
			}

			ImGui::SameLine();

			if (ImGui::Button("Cancel", buttonSize) || gbc::Input::IsKeyPressed(gbc::Keycode::Escape))
				closed = true;

			ImGui::PopStyleVar();

			if (closed)
			{
				ImGui::CloseCurrentPopup();
				RemoveStates(IDEState_PopupOpen | IDEState_ModalPopup);
				m_pOpenPopup = nullptr;
			}

			if (delet3)
			{
				if (m_pExplorerPanel->IsSelectedFilepathADirectory())
					DeleteFolder(m_pExplorerPanel->GetSelectedFilepath());
				else
					DeleteFilePanel(m_pExplorerPanel->GetSelectedFilepath());
			}

			ImGui::EndPopup();
		}
		ImGui::PopStyleVar();
	}

	void EZ80IDELayer::OnEvent(gbc::Event& event)
	{
		gbc::EventDispatcher dispatcher(event);
		dispatcher.Dispatch(this, &EZ80IDELayer::OnWindowCloseEvent);
		dispatcher.Dispatch(this, &EZ80IDELayer::OnKeyPressEvent);
		if (!m_FilePanels.HasFocusedPanel())
			dispatcher.Dispatch(&m_Panels, &PanelStack::OnEvent);
		if (!m_Panels.HasFocusedPanel())
			dispatcher.Dispatch(&m_FilePanels, &PanelStack::OnEvent);
	}

	void EZ80IDELayer::OnWindowCloseEvent(gbc::WindowCloseEvent& event)
	{
		if (IsState(IDEState_WorkspaceOpen))
			CloseWorkspace();
	}

	void EZ80IDELayer::OnKeyPressEvent(gbc::KeyPressEvent& event)
	{
		gbc::Keycode keycode = event.GetKeycode();
		gbc::Mods mods = event.GetMods();

		if (HasSubStates(IDEState_WorkspaceOpen | IDEState_FileOpen, IDEState_ModalPopup))
		{
			switch (keycode)
			{
			case gbc::Keycode::S:
				if (mods.Are(gbc::Mods_Control))
					SaveFile();
				else if (mods.Are(gbc::Mods_Control | gbc::Mods_Shift))
					SaveAll();
				break;
			case gbc::Keycode::W:
				if (mods.Are(gbc::Mods_Control))
					CloseFile();
				break;
			}
		}

		if (HasSubStates(IDEState_WorkspaceOpen | IDEState_ExplorerFocused))
		{
			switch (keycode)
			{
			case gbc::Keycode::Delete:
				if (mods.Are(gbc::Mods_None) && m_pExplorerPanel->IsSelectedFilepathDeleteAllowed())
					Delete();
			case gbc::Keycode::C:
				if (mods.Are(gbc::Mods_Shift))
					CopyPath(m_pExplorerPanel->GetSelectedFilepath());
				if (mods.Are(gbc::Mods_Shift | gbc::Mods_Alt))
					CopyRelativePath(m_pExplorerPanel->GetSelectedFilepath());
				break;
			}
		}

		if (HasSubStates(IDEState_WorkspaceOpen, IDEState_ModalPopup))
		{
			switch (keycode)
			{
			case gbc::Keycode::N:
				if (mods.Are(gbc::Mods_Control))
					NewFile();
				else if (mods.Are(gbc::Mods_Control | gbc::Mods_Shift))
					NewFolder();
				break;
			case gbc::Keycode::R:
				if (mods.Are(gbc::Mods_Control))
					Rename();
				break;
			case gbc::Keycode::W:
				if (mods.Are(gbc::Mods_Control | gbc::Mods_Shift))
					CloseWorkspace();
				break;
			}
		}

		if (HasSubStates(IDEState_None, IDEState_ModalPopup))
		{
			switch (keycode)
			{
			case gbc::Keycode::O:
				if (mods.Are(gbc::Mods_Control))
					OpenWorkspace();
				break;
			}
		}
	}

	const std::string& EZ80IDELayer::GetFilePanelTitle(const std::filesystem::path& filepath, uint64_t id)
	{
		auto it = m_TitleIDs.find(filepath);
		if (it != m_TitleIDs.end())
			return it->second;

		// Get the true id.
		uint64_t titleID;
		if (id)
		{
			titleID = id;
			if (id > m_NextTitleID)
				m_NextTitleID = id;
		}
		else
			titleID = ++m_NextTitleID;

		auto& titleIDString = m_TitleIDs[filepath];
		gbc::util::IToS(titleID, titleIDString, 36);
		titleIDString.insert(0, FilePanel::s_Identifier);
		return titleIDString;
	}

	void EZ80IDELayer::RenameFilePanel(const std::filesystem::path& filepathOld, const std::filesystem::path& filepathNew)
	{
		auto it = m_TitleIDs.find(filepathOld);
		if (it != m_TitleIDs.end())
		{
			// Remove the previous entry before adding the new
			// one because that could invalidate the former.
			std::string titleIDString = std::move(it->second);
			m_TitleIDs.erase(it);
			m_TitleIDs[filepathNew] = std::move(titleIDString);

			for (auto panel : m_FilePanels)
			{
				FilePanel* filePanel = (FilePanel*)panel;
				if (filePanel->GetFilepath() == filepathOld)
				{
					filePanel->Rename(filepathNew);
					break;
				}
			}
		}
	}

	void EZ80IDELayer::RemoveFilePanel(const std::filesystem::path& filepath)
	{
		auto it = m_TitleIDs.find(filepath);
		if (it != m_TitleIDs.end())
			m_TitleIDs.erase(it);
	}

	void EZ80IDELayer::DeleteFilePanel(const std::filesystem::path& filepath)
	{
		if (gbc::FileIO::IsAncestorOf(m_SrcDirectory, filepath) && gbc::FileIO::FileExists(filepath))
		{
			auto it = m_TitleIDs.find(filepath);
			if (it != m_TitleIDs.end())
			{
				m_TitleIDs.erase(it);

				for (auto it = m_FilePanels.begin(); it != m_FilePanels.end(); ++it)
				{
					FilePanel* filePanel = (FilePanel*)(*it);
					if (filePanel->GetFilepath() == filepath)
					{
						m_FilePanels.Remove(it - m_FilePanels.begin());
						break;
					}
				}
			}

			gbc::FileIO::Delete(filepath);
		}
	}

	void EZ80IDELayer::DeleteFolder(const std::filesystem::path& filepath)
	{
		if (gbc::FileIO::IsAncestorOf(m_SrcDirectory, filepath) && gbc::FileIO::DirectoryExists(filepath))
		{
			if (gbc::FileIO::IsEmptyDirectory(filepath))
				gbc::FileIO::Delete(filepath);
			else
			{
				for (size_t i = 0; i < m_FilePanels.Size();)
				{
					FilePanel* filePanel = (FilePanel*)m_FilePanels[i];
					auto& filePanelFilepath = filePanel->GetFilepath();
					if (gbc::FileIO::IsAncestorOf(filepath, filePanelFilepath))
					{
						m_TitleIDs.erase(filePanelFilepath);
						m_FilePanels.Remove(i);
					}
					else
						i++;
				}

				gbc::FileIO::DeleteRecursive(filepath);
			}
		}
	}

	bool EZ80IDELayer::FocusFilePanel(const std::filesystem::path& filepath)
	{
		auto it = m_TitleIDs.find(filepath);
		if (it == m_TitleIDs.end())
			return false;

		for (auto it = m_FilePanels.begin(); it != m_FilePanels.end(); ++it)
		{
			FilePanel* filePanel = (FilePanel*)(*it);
			if (filePanel->GetFilepath() == filepath)
			{
				m_FilePanels.Focus(it - m_FilePanels.begin());
				if (filePanel->HasEnabledChanged())
					AddStates(IDEState_FileOpen);
				return true;
			}
		}

		// Shouldn't happen.
		GBC_ASSERT(false, "File panel not found in file panel stack, but found in title id registry.");
		return false;
	}

	void EZ80IDELayer::FocusOrAddFilePanel(const std::filesystem::path& filepath)
	{
		if (!FocusFilePanel(filepath))
			(void)AddFilePanel(filepath);
	}

	bool EZ80IDELayer::OpenPopup(const char* name, ImGuiPopupFlags flags)
	{
		if (!IsAnyPopupOpen())
		{
			AddStates(IDEState_PopupOpen);
			ImGui::OpenPopup(name, flags | ImGuiPopupFlags_NoOpenOverExistingPopup);
			return true;
		}

		if (ImGui::IsPopupOpen(name))
			return true;

		RemoveStates(IDEState_PopupOpen);
		return false;
	}

	bool EZ80IDELayer::IsAnyPopupOpen()
	{
		return HasSubStates(IDEState_PopupOpen);
	}

	bool EZ80IDELayer::IsPopupOpen(const char* name, ImGuiPopupFlags flags)
	{
		return IsAnyPopupOpen() && ImGui::IsPopupOpen(name, flags);
	}

	void EZ80IDELayer::ClosePopup(const char* name)
	{
		if (IsAnyPopupOpen())
		{
			RemoveStates(IDEState_PopupOpen);
			if (ImGui::IsPopupOpen(name))
				ImGui::CloseCurrentPopup();
		}
	}

	FilePanel* EZ80IDELayer::AddFilePanel(const std::filesystem::path& filepath, uint64_t id)
	{
		AddStates(IDEState_FileOpen);

		// TODO: set position, size, docking
		FilePanel* panel = new FilePanel(this, filepath, id);
		m_FilePanels.Push(panel);
		return panel;
	}

	void EZ80IDELayer::NewFile()
	{
		if (!HasAnySubStates(IDEState_ModalPopup))
		{
			GBC_INFO("New File");

			AddStates(IDEState_PopupOpen | IDEState_ModalPopup);
			m_pOpenPopup = &EZ80IDELayer::UI_Popup_NewFile;

			m_NewFilePendingExtension = m_NewFileSelectedExtension;
			memset(&m_NewFilenameBuffer, 0, sizeof(m_NewFilenameBuffer));

			m_NewFileInitialDirectory = gbc::FileIO::Relative(m_pExplorerPanel->GetNewFileDirectory(), m_WorkspaceDirectory);
			m_NewFileInitialDirectoryString = m_NewFileInitialDirectory.string();
			m_NewFileInitialDirectoryString.insert(0, 1, '.');
			m_NewFileInitialDirectoryString.insert(1, 1, static_cast<char>(std::filesystem::path::preferred_separator));
			m_NewFileInitialDirectoryString += static_cast<char>(std::filesystem::path::preferred_separator);
		}
	}

	void EZ80IDELayer::NewFolder()
	{
		if (!HasAnySubStates(IDEState_ModalPopup))
		{
			GBC_INFO("New Folder");

			AddStates(IDEState_PopupOpen | IDEState_ModalPopup);
			m_pOpenPopup = &EZ80IDELayer::UI_Popup_NewFolder;

			m_NewFilePendingExtension = m_NewFileSelectedExtension;
			memset(&m_NewFilenameBuffer, 0, sizeof(m_NewFilenameBuffer));

			m_NewFileInitialDirectory = gbc::FileIO::Relative(m_pExplorerPanel->GetNewFileDirectory(), m_WorkspaceDirectory);
			m_NewFileInitialDirectoryString = m_NewFileInitialDirectory.string();
			m_NewFileInitialDirectoryString.insert(0, 1, '.');
			m_NewFileInitialDirectoryString.insert(1, 1, static_cast<char>(std::filesystem::path::preferred_separator));
			m_NewFileInitialDirectoryString += static_cast<char>(std::filesystem::path::preferred_separator);
		}
	}

	void EZ80IDELayer::OpenWorkspace()
	{
		GBC_INFO("Open Workspace");

		auto filepath = gbc::FileDialog::OpenFolder();
		if (!filepath.empty() && gbc::FileIO::DirectoryExists(filepath))
		{
			if (HasSubStates(IDEState_WorkspaceOpen))
				CloseWorkspace();
			SetWorkspaceDirectory(filepath);
		}
	}

	void EZ80IDELayer::SaveFile()
	{
		GBC_INFO("Save File");

		((FilePanel*)m_FilePanels.Peek())->Save();
	}

	void EZ80IDELayer::SaveAll()
	{
		GBC_INFO("Save All");

		for (auto panel : m_FilePanels)
			if (panel->IsEnabled())
				((FilePanel*)panel)->Save();
	}

	void EZ80IDELayer::Rename()
	{
		if (!HasAnySubStates(IDEState_ModalPopup))
		{
			std::filesystem::path filename;
			std::wstring_view extension;
			if (m_pExplorerPanel->IsFocused())
			{
				filename = m_pExplorerPanel->GetSelectedFilepath().filename();
				extension = std::filesystem::_Parse_extension(filename.native());
				m_NewFileInitialDirectory = gbc::FileIO::Relative(m_pExplorerPanel->GetNewFileDirectory(), m_WorkspaceDirectory);
				m_RenamingFolder = extension.empty();
				if (m_RenamingFolder)
					m_NewFileInitialDirectory = m_NewFileInitialDirectory.parent_path();
				else if (!m_FilePanels.Empty())
				{
					m_RenamingOpenFile = false;
					for (auto panel : m_FilePanels)
					{
						FilePanel* filePanel = (FilePanel*)panel;
						if (filePanel->GetFilepath() == m_pExplorerPanel->GetSelectedFilepath())
						{
							m_RenamingOpenFile = true;
							break;
						}
					}
				}
			}
			else if (HasSubStates(IDEState_FileOpen))
			{
				m_RenamingOpenFile = true;
				m_RenamingFolder = false;

				FilePanel* filePanel = (FilePanel*)m_FilePanels.Peek();
				filename = filePanel->GetFilepath().filename();
				extension = std::filesystem::_Parse_extension(filename.native());
				m_NewFileInitialDirectory = gbc::FileIO::Relative(filePanel->GetFilepath(), m_WorkspaceDirectory).parent_path();
			}
			else // Nothing to rename.
				return;

			GBC_INFO("Rename");

			AddStates(IDEState_PopupOpen | IDEState_ModalPopup);
			m_pOpenPopup = &EZ80IDELayer::UI_Popup_Rename;

			std::string filenameString;
			if (m_RenamingFolder)
				filenameString = filename.string();
			else
			{
				filenameString = filename.replace_extension().string();
				extension = extension.substr(1);

				m_NewFilePendingExtension = 0;
				if (extension == L"inc")
					m_NewFilePendingExtension = 1;
				else if (extension == L"asm")
					m_NewFilePendingExtension = 2;
			}

			strcpy_s(m_NewFilenameBuffer, filenameString.length() + 1, filenameString.c_str());

			m_NewFileInitialDirectoryString = m_NewFileInitialDirectory.string();
			m_NewFileInitialDirectoryString.insert(0, 1, '.');
			m_NewFileInitialDirectoryString.insert(1, 1, static_cast<char>(std::filesystem::path::preferred_separator));
			m_NewFileInitialDirectoryString += static_cast<char>(std::filesystem::path::preferred_separator);
		}
	}

	void EZ80IDELayer::Delete()
	{
		if (!HasAnySubStates(IDEState_ModalPopup))
		{
			GBC_INFO("Delete");

			AddStates(IDEState_PopupOpen | IDEState_ModalPopup);
			m_pOpenPopup = &EZ80IDELayer::UI_Popup_ConfirmDelete;

			m_DeletingFolder = m_pExplorerPanel->IsSelectedFilepathADirectory();
			m_DeletingMessage = m_pExplorerPanel->GetSelectedFilepath().filename().string();
			if (m_DeletingFolder && !gbc::FileIO::IsEmptyDirectory(m_pExplorerPanel->GetSelectedFilepath()))
				m_DeletingMessage += " and all its contents";
		}
	}

	void EZ80IDELayer::CloseFile()
	{
		GBC_INFO("Close File");

		// TODO: close with or without saving dialog
		// check box for save my answer, use it every time this happens, and do not show this again + change later in settings
		//((FilePanel*)m_FilePanels.Peek())->MarkForCloseWithoutSaving();

		m_FilePanels.Close();

		if (m_FilePanels.GetOpenPanelCount() <= 1) // off by one
			RemoveStates(IDEState_FileOpen);
	}

	void EZ80IDELayer::CloseWorkspace()
	{
		GBC_INFO("Close Folder");

		// TODO: close with or without saving dialog
		// check box for save my answer, use it every time this happens, and do not show this again + change later in settings

		SetWorkspaceDirectory({});
	}

	void EZ80IDELayer::CopyPath(const std::filesystem::path& filepath)
	{
		gbc::Input::SetClipboardString(filepath.string());
	}

	void EZ80IDELayer::CopyRelativePath(const std::filesystem::path& filepath)
	{
		gbc::Input::SetClipboardString(gbc::FileIO::Relative(filepath, m_WorkspaceDirectory).string());
	}

	void EZ80IDELayer::SaveAndCloseWorkspace()
	{
		if (HasSubStates(IDEState_WorkspaceOpen))
		{
			// Remove all closed panels so they aren't written to metadata.
			m_FilePanels.RemoveClosedPanels();

			// Save all metadata.
			SerializeWorkspace();

			// Reset title ids.
			m_TitleIDs.clear();
			m_NextTitleID = 0;

			// Save the old workspace imgui ini.
			ImGui::SaveIniSettingsToDisk(m_ImguiIniFilepathString.c_str());

			// Save all unsaved files, delete all file panels,
			// and make all file imgui windows not persist to next workspace ini.
			m_FilePanels.Clear();
		}
	}

	void EZ80IDELayer::SetWorkspaceDirectory(const std::filesystem::path& workspaceDirectory)
	{
		SaveAndCloseWorkspace();

		if (workspaceDirectory.empty())
		{
			SetState(IDEState_None);

			m_WorkspaceDirectory.clear();
			m_MetaDataDirectory.clear();
			m_ImguiIniFilepathString.clear();
			m_SrcDirectory.clear();
			m_BinDirectory.clear();
			m_CalcDirectory.clear();
			m_ROMFilepath.clear();

			m_EmulatorThread.Unload();
		}
		else
		{
			AddStates(IDEState_WorkspaceOpen);

			// Update workspace to new directory.
			m_WorkspaceDirectory = gbc::FileIO::Absolute(workspaceDirectory);

			m_SrcDirectory = workspaceDirectory / L"src";
			gbc::FileIO::MakeDirectoryIfNotExists(m_SrcDirectory);
			m_BinDirectory = workspaceDirectory / L"bin";
			gbc::FileIO::MakeDirectoryIfNotExists(m_BinDirectory);
			m_CalcDirectory = workspaceDirectory / L"calc";
			gbc::FileIO::MakeDirectoryIfNotExists(m_CalcDirectory);

			m_MetaDataDirectory = workspaceDirectory / L".ez80ide";
			gbc::FileIO::MakeHiddenDirectory(m_MetaDataDirectory);

			std::filesystem::path imguiIniFilepath = m_MetaDataDirectory / L"imgui.ini";
			m_ImguiIniFilepathString = imguiIniFilepath.string();
			if (gbc::FileIO::FileExists(imguiIniFilepath))
				ImGui::LoadIniSettingsFromDisk(m_ImguiIniFilepathString.c_str());

			m_MetaDataFilepath = m_MetaDataDirectory / L"metadata.yaml";
			DeserializeWorkspace();
		}

		m_pExplorerPanel->SetWorkspaceDirectory(m_WorkspaceDirectory);
	}

	void EZ80IDELayer::SerializeWorkspace()
	{
		std::ofstream metaDataFile(m_MetaDataFilepath);
		GBC_ASSERT(metaDataFile.is_open(), "Failed to open meta data file when closing workspace.");

		YAML::Emitter out;
		out << YAML::BeginMap;

		if (HasAnySubStates(IDEState_ROMLoaded))
			out << YAML::Key << "ROMFilepath" << YAML::Value << m_ROMFilepath.string();

		// TODO: save ram as well.

		if (!m_TitleIDs.empty())
		{
			out << YAML::Key << "TitleIDs" << YAML::Value << YAML::BeginMap;
			for (auto& [filepath, id] : m_TitleIDs)
				out << YAML::Key << filepath.string() << YAML::Value << id;
			out << YAML::EndMap;
		}

		out << YAML::EndMap;
		metaDataFile << out.c_str();
		metaDataFile.close();
	}

	void EZ80IDELayer::DeserializeWorkspace()
	{
		std::ifstream metaDataFile(m_MetaDataFilepath);
		GBC_ASSERT(metaDataFile.is_open(), "Failed to open meta data file when opening workspace.");

		YAML::Node metaData = YAML::Load(metaDataFile);
		metaDataFile.close();

		if (auto romFilepathNode = metaData["ROMFilepath"])
		{
			m_ROMFilepath = romFilepathNode.as<std::string>();

			if (gbc::FileIO::FileExists(m_ROMFilepath))
			{
				auto romFilepathString = m_ROMFilepath.string();
				m_EmulatorThread.Load(romFilepathString.c_str());
				AddStates(IDEState_ROMLoaded);
			}
			else
			{
				m_ROMFilepath.clear();
				m_EmulatorThread.Unload();
				RemoveStates(IDEState_ROMLoaded);
			}
		}

		if (auto titleIDsNode = metaData["TitleIDs"])
		{
			for (auto titleIDNode : titleIDsNode)
			{
				std::filesystem::path titleIDFilepath = titleIDNode.first.as<std::string>();

				uint64_t titleID;
				if (!gbc::util::SToI(titleIDNode.second.as<std::string>(), titleID, 36))
					titleID = 0;

				auto filePanel = AddFilePanel(titleIDFilepath, titleID);
				m_TitleIDs[std::move(titleIDFilepath)] = filePanel->GetDefaultTitle();
			}
		}
	}

	bool EZ80IDELayer::LoadROM()
	{
		auto filepath = gbc::FileDialog::OpenFile(gbc::GetFilter(gbc::FileType::rom));
		if (filepath.empty())
			return false;

		auto filepathString = filepath.string();
		if (!m_EmulatorThread.Load(filepathString.c_str()))
			return false;

		m_ROMFilepath = std::move(filepath);
		m_EmulatorThread.Start();
		AddStates(IDEState_ROMLoaded | IDEState_EmulatorRunning);
		return true;
	}

	void EZ80IDELayer::UnloadROM()
	{
		m_ROMFilepath.clear();
		m_EmulatorThread.Unload();
		RemoveStates(IDEState_ROMLoaded | IDEState_EmulatorRunning);
	}
}

#include "FilePanel.h"
#include "Layers/EZ80IDELayer.h"
#include "GBC/ImGui/ImGuiHelper.h"
#include <imgui/imgui_internal.h>
#include <unordered_map>

namespace gbc
{
	FilePanel::FilePanel(EZ80IDELayer* ez80IDELayer, const std::filesystem::path& filepath, uint64_t id)
		: Panel(ez80IDELayer, ez80IDELayer->GetFilePanelTitle(filepath, id), false), m_Filepath(filepath)
	{
		bool status = FileIO::MakeFileIfNotExists(filepath);
		GBC_ASSERT(status, "Failed to create file panel file!");
		UpdateWindowTitle();
		SetEnabled(true);
	}

	FilePanel::~FilePanel()
	{
		auto& ide = GetIDE();
		ide.RemoveFilePanel(m_Filepath);

		Save();
		SetEnabled(false);
	}

	void FilePanel::OnImGuiRender(int)
	{
		// TODO: text editor
	}

	void FilePanel::UpdateWindowTitle()
	{
		m_WindowTitle = m_Filepath.filename().string();
		m_AsteriskIndex = m_WindowTitle.length();
		auto& defaultTitle = GetDefaultTitle();
		m_WindowTitle.reserve(m_AsteriskIndex + 1 + defaultTitle.length());
		m_WindowTitle += m_UnsavedChanges ? '*' : ' ';
		m_WindowTitle += defaultTitle;
	}

	void FilePanel::Rename(const std::filesystem::path& filepath) noexcept
	{
		if (FileIO::FileExists(this->m_Filepath))
		{
			Save();
			FileIO::RenameFile(this->m_Filepath, filepath);
			this->m_Filepath = filepath;
		}
		else
		{
			this->m_Filepath = filepath;
			Save();
		}
		UpdateWindowTitle();
	}

	void FilePanel::SetEnabled(bool enabled) noexcept
	{
		Panel::SetEnabled(enabled);
		if (HasEnabledChanged())
		{
			if (enabled)
			{
				// If the window is being reopened, reallow it to save its state.
				if (ImGuiWindow* window = ImGui::FindWindowByName(m_WindowTitle.c_str()))
					window->Flags &= ~ImGuiWindowFlags_NoSavedSettings;
			}
			else
			{
				// Prevent the window from saving its state to new workspace ini files to avoid cross-workspace contamination.
				if (ImGuiWindow* window = ImGui::FindWindowByName(m_WindowTitle.c_str()))
					window->Flags |= ImGuiWindowFlags_NoSavedSettings;
			}
		}
	}

	void FilePanel::SetFocused(bool focused) noexcept
	{
		Panel::SetFocused(focused);
		if (HasFocusChanged() && focused)
			ImGui::SetWindowFocus(m_WindowTitle.c_str());
	}

	void FilePanel::OnChanged()
	{
		if (!m_UnsavedChanges)
		{
			m_UnsavedChanges = true;
			m_WindowTitle[m_AsteriskIndex] = '*';
		}
	}

	void FilePanel::Save()
	{
		if (m_UnsavedChanges)
		{
			m_UnsavedChanges = false;
			m_WindowTitle[m_AsteriskIndex] = ' ';

			//std::ofstream file(m_Filepath);
			//if (file.is_open())
			//{
			//	// TODO: write data

			//	file.close();
			//}
		}
	}
}

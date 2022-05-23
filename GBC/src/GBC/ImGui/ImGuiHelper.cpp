#include "gbcpch.h"
#include "ImGuiHelper.h"
#include <imgui/imgui_internal.h>

namespace gbc
{
	void ImGuiHelper::Init()
	{
		ImGuiIO& io = ImGui::GetIO();
		ImGuiStyle& style = ImGui::GetStyle();

		io.FontDefault = io.Fonts->AddFontFromFileTTF("vendor/CEmu/CEmu/gui/qt/resources/custom_fonts/TICELarge.ttf", 16.0f);

		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable | ImGuiConfigFlags_ViewportsEnable;
		io.ConfigWindowsMoveFromTitleBarOnly = true;

		style.WindowRounding = 0.0f;
		style.FramePadding = { 4.0f, 4.0f };
		style.WindowPadding = { 4.0f, 4.0f };
		style.WindowMenuButtonPosition = ImGuiDir_None;

		SetDarkThemeColors();
	}

	void ImGuiHelper::SetDarkThemeColors()
	{
		auto& colors = ImGui::GetStyle().Colors;

		// Start with default dark theme.
		ImGui::StyleColorsDark();

		// Window
		colors[ImGuiCol_WindowBg]           = { 0.100f, 0.105f, 0.110f, 1.000f };

		// Frame Background
		colors[ImGuiCol_FrameBg]            = { 0.165f, 0.170f, 0.175f, 1.000f };
		colors[ImGuiCol_FrameBgHovered]     = { 0.200f, 0.205f, 0.210f, 1.000f };
		colors[ImGuiCol_FrameBgActive]      = { 0.150f, 0.155f, 0.160f, 1.000f };

		// Title Background
		colors[ImGuiCol_TitleBg]            = { 0.150f, 0.155f, 0.160f, 1.000f };
		colors[ImGuiCol_TitleBgActive]      = { 0.150f, 0.155f, 0.160f, 1.000f };
		colors[ImGuiCol_TitleBgCollapsed]   = { 0.150f, 0.155f, 0.160f, 1.000f };
		
		// Checkbox
		colors[ImGuiCol_CheckMark]          = { 0.260f, 0.590f, 0.980f, 1.000f };

		// Slider
		colors[ImGuiCol_SliderGrab]         = { 0.338f, 0.338f, 0.338f, 1.000f };
		colors[ImGuiCol_SliderGrabActive]   = { 0.338f, 0.338f, 0.338f, 1.000f };

		// Buttons
		colors[ImGuiCol_Button]             = { 0.200f, 0.205f, 0.210f, 1.000f };
		colors[ImGuiCol_ButtonHovered]      = { 0.300f, 0.305f, 0.310f, 1.000f };
		colors[ImGuiCol_ButtonActive]       = { 0.150f, 0.155f, 0.160f, 1.000f };

		// Headers
		colors[ImGuiCol_Header]             = { 0.200f, 0.205f, 0.210f, 1.000f };
		colors[ImGuiCol_HeaderHovered]      = { 0.300f, 0.305f, 0.310f, 1.000f };
		colors[ImGuiCol_HeaderActive]       = { 0.150f, 0.155f, 0.160f, 1.000f };

		// Tabs
		colors[ImGuiCol_Tab]                = { 0.150f, 0.155f, 0.160f, 1.000f };
		colors[ImGuiCol_TabHovered]         = { 0.380f, 0.385f, 0.390f, 1.000f };
		colors[ImGuiCol_TabActive]          = { 0.280f, 0.285f, 0.290f, 1.000f };
		colors[ImGuiCol_TabUnfocused]       = { 0.150f, 0.155f, 0.160f, 1.000f };
		colors[ImGuiCol_TabUnfocusedActive] = { 0.200f, 0.205f, 0.210f, 1.000f };

		// Drag/Drop
		colors[ImGuiCol_DragDropTarget]     = { 0.102f, 0.337f, 0.608f, 1.000f };
	}

	bool ImGuiHelper::BeginTable(const char* id, int32_t columnCount, ImGuiTableFlags flags)
	{
		ImGuiStyle& style = ImGui::GetStyle();
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { style.ItemSpacing.x / 2.0f, style.ItemSpacing.y });

		ImGui::PushID(id);
		if (!ImGui::BeginTable(id, columnCount, flags))
		{
			ImGui::PopID();
			ImGui::PopStyleVar();
			return false;
		}

		ImGui::TableSetupColumn(nullptr, ImGuiTableColumnFlags_WidthAuto);
		ImGui::TableSetupColumn(nullptr, ImGuiTableColumnFlags_None);
		for (int32_t i = 2; i < columnCount; i++)
			ImGui::TableSetupColumn(nullptr, ImGuiTableColumnFlags_None);

		NextTableColumn();
		return true;
	}

	void ImGuiHelper::NextTableColumn()
	{
		ImGui::TableNextColumn();
	}

	void ImGuiHelper::PrevTableColumn()
	{
		ImGui::TableSetColumnIndex(ImGui::TableGetColumnIndex() - 1);
	}

	void ImGuiHelper::EndTable()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 4.0f, 2.0f });

		ImGui::EndTable();
		ImGui::PopID();

		ImGui::PopStyleVar(2);
	}

	bool ImGuiHelper::InputText(const char* label, char* buffer, size_t size, ImGuiInputTextFlags flags)
	{
		bool changed = false;
		ImGui::PushID(label);

		NextTableColumn();
		ImGui::SetNextItemWidth(-FLT_MIN);
		changed |= ImGui::InputText("", buffer, size, flags);
		PrevTableColumn();
		ImGui::Text(label);
		NextTableColumn();

		ImGui::PopID();
		return changed;
	}

	bool ImGuiHelper::DragFloat3(const char* label, float* values, float speed, float minValue, float maxValue)
	{
		bool changed = false;
		ImGui::PushID(label);

		NextTableColumn();
		ImGui::SetNextItemWidth(-FLT_MIN);
		changed |= ImGui::DragFloat3("", values, speed, minValue, maxValue);
		PrevTableColumn();
		ImGui::Text(label);
		NextTableColumn();

		ImGui::PopID();
		return changed;
	}

	bool ImGuiHelper::DragFloat2(const char* label, float* values, float speed, float minValue, float maxValue)
	{
		bool changed = false;
		ImGui::PushID(label);

		NextTableColumn();
		ImGui::SetNextItemWidth(-FLT_MIN);
		changed |= ImGui::DragFloat2("", values, speed, minValue, maxValue);
		PrevTableColumn();
		ImGui::Text(label);
		NextTableColumn();

		ImGui::PopID();
		return changed;
	}

	bool ImGuiHelper::DragFloat(const char* label, float* value, float speed, float minValue, float maxValue)
	{
		bool changed = false;
		ImGui::PushID(label);

		NextTableColumn();
		ImGui::SetNextItemWidth(-FLT_MIN);
		changed |= ImGui::DragFloat("", value, speed, minValue, maxValue);
		PrevTableColumn();
		ImGui::Text(label);
		NextTableColumn();

		ImGui::PopID();
		return changed;
	}

	bool ImGuiHelper::Combo(uint32_t* selectedItem, const char* const* names, uint32_t count)
	{
		bool changed = false;

		if (ImGui::BeginCombo("", names[*selectedItem]))
		{
			for (uint32_t i = 0; i < count; i++)
			{
				bool selected = *selectedItem == i;
				if (ImGui::Selectable(names[i], &selected) && *selectedItem != i)
				{
					*selectedItem = i;
					changed = true;
				}
				if (selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}

		return changed;
	}

	bool ImGuiHelper::Combo(const char* label, uint32_t* selectedItem, const char* const* names, uint32_t count)
	{
		bool changed = false;
		ImGui::PushID(label);

		NextTableColumn();
		ImGui::SetNextItemWidth(-FLT_MIN);
		changed |= Combo(selectedItem, names, count);
		PrevTableColumn();
		ImGui::Text(label);
		NextTableColumn();

		ImGui::PopID();
		return changed;
	}

	bool ImGuiHelper::Checkbox(const char* label, bool* value)
	{
		bool changed = false;
		ImGui::PushID(label);

		NextTableColumn();
		ImGui::SetNextItemWidth(-FLT_MIN);
		changed |= ImGui::Checkbox("", value);
		PrevTableColumn();
		ImGui::Text(label);
		NextTableColumn();

		ImGui::PopID();
		return changed;
	}

	bool ImGuiHelper::ColorEdit3(const char* label, float* values)
	{
		bool changed = false;
		ImGui::PushID(label);

		NextTableColumn();
		changed |= ImGui::ColorEdit3("", values);
		PrevTableColumn();
		ImGui::Text(label);
		NextTableColumn();

		ImGui::PopID();
		return changed;
	}

	bool ImGuiHelper::ColorEdit4(const char* label, float* values)
	{
		bool changed = false;
		ImGui::PushID(label);

		NextTableColumn();
		changed |= ImGui::ColorEdit4("", values);
		PrevTableColumn();
		ImGui::Text(label);
		NextTableColumn();

		ImGui::PopID();
		return changed;
	}

	bool ImGuiHelper::TextEdit(std::string* value)
	{
		bool changed = false;
		ImGui::PushItemWidth(-FLT_MIN);

		static constexpr size_t bufferSize = 256;
		char buffer[bufferSize]{ '\0' };
		GBC_CORE_ASSERT(value->size() < bufferSize);
		strcpy_s(buffer, sizeof(buffer), value->c_str());

		if (ImGui::InputText("", buffer, sizeof(buffer)))
		{
			*value = buffer;
			changed = true;
		}

		ImGui::PopItemWidth();
		return changed;
	}

	bool ImGuiHelper::TextEdit(const char* label, std::string* value)
	{
		bool changed = false;
		ImGui::PushID(label);

		NextTableColumn();
		changed |= TextEdit(value);
		PrevTableColumn();
		ImGui::Text(label);
		NextTableColumn();

		ImGui::PopID();
		return changed;
	}

	const ImGuiPayload* ImGuiHelper::AcceptDragDropPayload(const char* dragDropType, const std::function<bool(void*)>& acceptFunc, ImGuiDragDropFlags flags)
	{
		if (acceptFunc == nullptr)
			return ImGui::AcceptDragDropPayload(dragDropType, flags);

		ImGuiPayload& payload = ImGui::GetCurrentContext()->DragDropPayload;
		if (payload.IsDataType(dragDropType) && acceptFunc(payload.Data))
			return ImGui::AcceptDragDropPayload(dragDropType, flags);

		return nullptr;
	}

	const ImGuiPayload* ImGuiHelper::ButtonDragDropTarget(const char* buttonText, const char* dragDropType, const std::function<bool(void*)>& acceptFunc, ImGuiDragDropFlags flags)
	{
		const ImGuiPayload* payload = nullptr;

		ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, { 0.0f, 0.5f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImGui::GetStyle().Colors[ImGuiCol_Button]);
		ImGui::Button(buttonText, { ImGui::GetContentRegionAvail().x, 0.0f });
		if (ImGui::BeginDragDropTarget())
		{
			payload = AcceptDragDropPayload(dragDropType, acceptFunc, flags);
			ImGui::EndDragDropTarget();
		}
		ImGui::PopStyleColor();
		ImGui::PopStyleVar();

		return payload;
	}

	const ImGuiPayload* ImGuiHelper::ButtonDragDropTarget(const char* label, const char* buttonText, const char* dragDropType, const std::function<bool(void*)>& acceptFunc, ImGuiDragDropFlags flags)
	{
		ImGui::PushID(label);

		NextTableColumn();
		auto payload = ButtonDragDropTarget(buttonText, dragDropType, acceptFunc, flags);
		PrevTableColumn();
		ImGui::Text(label);
		NextTableColumn();

		ImGui::PopID();
		return payload;
	}
}

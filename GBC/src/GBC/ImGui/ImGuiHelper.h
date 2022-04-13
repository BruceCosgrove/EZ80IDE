#pragma once

#include "GBC/Core/Core.h"
#if GBC_ENABLE_IMGUI
#include <imgui/imgui.h>
#include <functional>
#include <string>

namespace gbc
{
	class ImGuiHelper
	{
	private:
		friend class ImGuiWrapper;
		static void Init();
	public:
		static constexpr float defaultSpeed = 0.1f;
		static constexpr ImGuiTableFlags defaultTableFlags = ImGuiTableFlags_BordersInnerV;
		static constexpr ImGuiInputTextFlags defaultTextFlags = ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_EnterReturnsTrue;
		static constexpr ImGuiDragDropFlags defaultDragDropFlags = ImGuiDragDropFlags_None;
	public:
		static void SetScenePlayColors();
		static void SetSceneEditColors();

		static bool BeginTable(const char* id, int32_t columnCount = 2, ImGuiTableFlags flags = defaultTableFlags);
		static void NextTableColumn();
		static void PrevTableColumn();
		static void EndTable();

		template<typename... Args> static void Text(const char* format, Args&&... args);
		template<typename... Args> static void Text(const char* label, const char* format, Args&&... args);

		static bool InputText(char* buffer, size_t size, ImGuiInputTextFlags flags = defaultTextFlags);
		template<size_t Size> static bool InputText(char(&buffer)[Size], ImGuiInputTextFlags flags = defaultTextFlags);

		static bool FloatEdit3(float* values, float speed = defaultSpeed, float minValue = 0.0f, float maxValue = 0.0f);
		static bool FloatEdit3(const char* label, float* values, float speed = defaultSpeed, float minValue = 0.0f, float maxValue = 0.0f);

		static bool FloatEdit2(float* values, float speed = defaultSpeed, float minValue = 0.0f, float maxValue = 0.0f);
		static bool FloatEdit2(const char* label, float* values, float speed = defaultSpeed, float minValue = 0.0f, float maxValue = 0.0f);

		static bool FloatEdit(float* value, float speed = defaultSpeed, float minValue = 0.0f, float maxValue = 0.0f);
		static bool FloatEdit(const char* label, float* value, float speed = defaultSpeed, float minValue = 0.0f, float maxValue = 0.0f);

		static bool Combo(int32_t* selectedItem, const char* const* names, int32_t count);
		static bool Combo(const char* label, int32_t* selectedItem, const char* const* names, int32_t count);
		
		static bool Checkbox(bool* value);
		static bool Checkbox(const char* label, bool* value);
		
		static bool ColorEdit3(float* values);
		static bool ColorEdit3(const char* label, float* values);
		
		static bool ColorEdit4(float* values);
		static bool ColorEdit4(const char* label, float* values);
		
		static bool TextEdit(std::string* value);
		static bool TextEdit(const char* label, std::string* value);

		static const ImGuiPayload* AcceptDragDropPayload(const char* dragDropType, const std::function<bool(void*)>& acceptFunc = nullptr, ImGuiDragDropFlags flags = defaultDragDropFlags);
		static const ImGuiPayload* ButtonDragDropTarget(const char* buttonText, const char* dragDropType, const std::function<bool(void*)>& acceptFunc = nullptr, ImGuiDragDropFlags flags = defaultDragDropFlags);
		static const ImGuiPayload* ButtonDragDropTarget(const char* label, const char* buttonText, const char* dragDropType, const std::function<bool(void*)>& acceptFunc = nullptr, ImGuiDragDropFlags flags = defaultDragDropFlags);

		template<typename... Args> static bool ConfirmAction(const char* name, bool* action, const char* messageFormat, Args&&... args);
		template<typename... Args> static bool Inform(const char* name, const char* messageFormat, Args&&... args);
	};
}

#include "ImGuiHelper.inl"
#endif

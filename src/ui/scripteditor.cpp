#include "scripteditor.h"
#include "texteditor.h"
#include <fstream>
#include <streambuf>

static TextEditor E;
static const char* file_name = "scripts/level1.lua";

void InitializeScriptEditor(void)
{
	auto lang = TextEditor::LanguageDefinition::Lua();
	E.SetLanguageDefinition(lang);

	{
		std::ifstream t(file_name);
		if (t.good())
		{
			std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
			E.SetText(str);
		}
	}
}

void UpdateScriptEditor(void)
{
	auto cpos = E.GetCursorPosition();
	ImGui::Begin("Text Editor Demo", nullptr, ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_MenuBar);
	ImGui::SetWindowSize(ImVec2(800, 600), ImGuiCond_FirstUseEver);
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Save"))
			{
				auto textToSave = E.GetText();
				/// save text....
			}
			// if (ImGui::MenuItem("Quit", "Alt-F4"))
			// 	break;
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Edit"))
		{
			bool ro = E.IsReadOnly();
			if (ImGui::MenuItem("Read-only mode", nullptr, &ro))
				E.SetReadOnly(ro);
			ImGui::Separator();

			if (ImGui::MenuItem("Undo", "ALT-Backspace", nullptr, !ro && E.CanUndo()))
				E.Undo();
			if (ImGui::MenuItem("Redo", "Ctrl-Y", nullptr, !ro && E.CanRedo()))
				E.Redo();

			ImGui::Separator();

			if (ImGui::MenuItem("Copy", "Ctrl-C", nullptr, E.HasSelection()))
				E.Copy();
			if (ImGui::MenuItem("Cut", "Ctrl-X", nullptr, !ro && E.HasSelection()))
				E.Cut();
			if (ImGui::MenuItem("Delete", "Del", nullptr, !ro && E.HasSelection()))
				E.Delete();
			if (ImGui::MenuItem("Paste", "Ctrl-V", nullptr, !ro && ImGui::GetClipboardText() != nullptr))
				E.Paste();

			ImGui::Separator();

			if (ImGui::MenuItem("Select all", nullptr, nullptr))
				E.SetSelection(TextEditor::Coordinates(), TextEditor::Coordinates(E.GetTotalLines(), 0));

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("View"))
		{
			if (ImGui::MenuItem("Dark palette"))
				E.SetPalette(TextEditor::GetDarkPalette());
			if (ImGui::MenuItem("Light palette"))
				E.SetPalette(TextEditor::GetLightPalette());
			if (ImGui::MenuItem("Retro blue palette"))
				E.SetPalette(TextEditor::GetRetroBluePalette());
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}

	ImGui::Text("%6d/%-6d %6d lines  | %s | %s | %s | %s", cpos.mLine + 1, cpos.mColumn + 1, E.GetTotalLines(),
		E.IsOverwrite() ? "Ovr" : "Ins",
		E.CanUndo() ? "*" : " ",
		E.GetLanguageDefinition().mName.c_str(), file_name);

	E.Render("TextEditor");
	ImGui::End();
}
#include "scripteditor.h"
#include "TextEditor.h"
extern "C" {
    #include "memory/linearallocator.h"
}
#include <fstream>
#include <streambuf>

static struct ScriptEditorContext
{
    // Defining it as pointer since its default constructor calls
    // functions that require ImGUI to be already initialized
    TextEditor* editor;
    std::string* text;
    const char* file_name = "scripts/level1.lua";
} C;

void InitializeScriptEditor(void)
{
    void* text_editor_address = LinearMalloc(sizeof(TextEditor));
    C.editor = new (text_editor_address) TextEditor();
    C.editor->SetLanguageDefinition(TextEditor::LanguageDefinition::Lua());
    C.editor->SetTabSize(4);
    C.editor->SetCompletePairedGlyphs(true);
    {
        std::ifstream t(C.file_name);
        if (t.good())
        {
            void* file_buffer = LinearMalloc(Kilobytes(512)); // TODO: replace with something that can grow
            C.text = new(file_buffer) std::string((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
            C.editor->SetText(*C.text);
        }
    }
}

void UpdateScriptEditor(void)
{
    ImGui::Begin("Text Editor Demo", nullptr, ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_MenuBar);
    ImGui::SetWindowSize(ImVec2(800, 600), ImGuiCond_FirstUseEver);
     if (ImGui::BeginMenuBar())
     {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Save"))
            {
                auto text = C.editor->GetText();
                // TODO: Save file contents
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Edit"))
        {
            if (ImGui::MenuItem("Undo", "Ctrl+Z", nullptr, C.editor->CanUndo()))
                C.editor->Undo();
            if (ImGui::MenuItem("Redo", "Ctrl-Y", nullptr, C.editor->CanRedo()))
                C.editor->Redo();

            ImGui::Separator();

            if (ImGui::MenuItem("Copy", "Ctrl-C", nullptr, C.editor->AnyCursorHasSelection()))
                C.editor->Copy();
            if (ImGui::MenuItem("Cut", "Ctrl-X", nullptr, C.editor->AnyCursorHasSelection()))
                C.editor->Cut();
            if (ImGui::MenuItem("Paste", "Ctrl-V", nullptr, ImGui::GetClipboardText() != nullptr))
                C.editor->Paste();

            ImGui::Separator();

            if (ImGui::MenuItem("Select all", "Ctrl+A", nullptr))
                C.editor->SelectAll();

            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
     }

    // TODO: Move this to the bottom as it's distracting to have it on top.
    // Currently replacing the calls to Text and pEditor->Render
    // results in screen flickering due to some bug in TextEditor.
    int line, column;
    C.editor->GetCursorPosition(line, column);
    ImGui::Text("Ln %3d, Col %-2d | %s | %s%s", line + 1, column + 1,
        C.editor->GetLanguageDefinitionName(), C.file_name,
        C.editor->CanUndo() ? "*" : " ");

    C.editor->Render("TextEditor");
    ImGui::End();
}
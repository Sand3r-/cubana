#include "scripteditor.h"
#include "TextEditor.h"
extern "C" {
    #include "log/log.h"
    #include "script/scripting.h"
    #include "memory/linearallocator.h"
    #include "file.h"
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

static void NewFile()
{
    C.file_name = "Untitled";
    C.text->clear();
    C.editor->SetText(*C.text);
}

static void SaveFile(const char* filename = "Untitled")
{
    File file = FileOpen(filename, "w");
    FileWrite(&file, (void*)C.editor->GetText().c_str(), sizeof(char), C.editor->GetText().size());
    FileClose(&file);
}

static void OpenFile(const char* file_name)
{
    C.file_name = file_name;
    File file = FileOpen(file_name, "r");
    size_t size = FileRead(&file, (void*)C.text->data(), sizeof(char), C.text->size());
    FileClose(&file);
    C.text->resize(size);
    C.editor->SetText(*C.text);
}

static void Execute()
{
    ExecuteScript(C.editor->GetText().c_str());
}

static void HandleKeyboardInputs()
{
    ImGuiIO& io = ImGui::GetIO();
    auto alt = io.KeyAlt;
    auto ctrl = io.KeyCtrl;
    auto shift = io.KeyShift;
    auto super = io.KeySuper;

    auto isAltOnly = alt && !ctrl && !shift && !super;
    auto isCtrlOnly = ctrl && !alt && !shift && !super;
    auto isShiftOnly = shift && !alt && !ctrl && !super;

    io.WantCaptureKeyboard = true;
    io.WantTextInput = true;

    if (isCtrlOnly && ImGui::IsKeyPressed(ImGuiKey_S))
        SaveFile(C.file_name);
    else if (isCtrlOnly && ImGui::IsKeyPressed(ImGuiKey_O))
        OpenFile("Untitled");
    else if (isCtrlOnly && ImGui::IsKeyPressed(ImGuiKey_N))
        NewFile();
    else if (ImGui::IsKeyPressed(ImGuiKey_F5))
        Execute();
}

static void DrawStatusBar()
{
    constexpr float textWidth = 300;
    int line, column;
    C.editor->GetCursorPosition(line, column);
    ImGui::SameLine(ImGui::GetContentRegionAvail().x - textWidth);
    ImGui::AlignTextToFramePadding();
    ImGui::Text("Ln %3d, Col %-2d | %s | %s%s", line + 1, column + 1,
        C.editor->GetLanguageDefinitionName(), C.file_name,
        C.editor->CanUndo() ? "*" : " ");
}

void UpdateScriptEditor(void)
{
    if (ImGui::Begin("Script Editor", nullptr, ImGuiWindowFlags_MenuBar))
    {
        ImGui::SetWindowSize(ImVec2(800, 600), ImGuiCond_FirstUseEver);
        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("New", "Ctrl+N"))
                    NewFile();
                if (ImGui::MenuItem("Save", "Ctrl+S"))
                    SaveFile();
                ImGui::MenuItem("Save As", "Ctrl+Shift+S", nullptr, false);
                if (ImGui::MenuItem("Open", "Ctrl+O"))
                    OpenFile("Untitled");
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
        try
        {
            C.editor->Render("TextEditor");
        }
        catch (...) // Any exception
        {
            const char* file = "recovered_file.txt";
            L_FATAL("Script Editor crashed. Saving backup to %s", file);
            SaveFile(file);
            exit(EXIT_FAILURE);
        }
        DrawStatusBar();
        HandleKeyboardInputs();
    }
    ImGui::End();
}
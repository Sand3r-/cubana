#include "scripteditor.h"
#include "TextEditor.h"
#include "nfd.h"
extern "C" {
    #include "culibc.h"
    #include "file.h"
    #include "log/log.h"
    #include "memory/linearallocator.h"
    #include "os/crash_handler.h"
    #include "platform.h"
    #include "script/scripting.h"
}
#include <fstream>
#include <streambuf>

static struct ScriptEditorContext
{
    // Defining it as pointer since its default constructor calls
    // functions that require ImGUI to be already initialized
    TextEditor* editor;
    std::string* text;
    char          file_name[MAX_PATH] = "scripts/level1.lua";
    char recovery_file_name[MAX_PATH] = "recovered_file.txt";
    const char*          DEFAULT_NAME = "Untitled";
} C;

static void SaveFile(const char* filename);

// Custom terminate handler
void SaveFileOnCrash()
{
    L_FATAL("Saving currently opened script to %s", C.recovery_file_name);
    SaveFile(C.recovery_file_name);
}

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
    NFD_Init();
    RegisterCrashCallback(SaveFileOnCrash);
}

static b8 GetPathFromFileDialog(char* out, u64 size, b8 save = false)
{
    nfdchar_t* path;
    nfdfilteritem_t filterItem[1] = { { "Source files", "lua,glsl,json" } };
    nfdresult_t result;
    if (save)
        result = NFD_SaveDialog(&path, filterItem, ArrayCount(filterItem), cu_getcwd(), C.DEFAULT_NAME);
    else
        result = NFD_OpenDialog(&path, filterItem, ArrayCount(filterItem), cu_getcwd());
    if (result == NFD_CANCEL)
    {
        L_INFO("User pressed cancel.");
        return false;
    }
    else if (result == NFD_ERROR)
    {
        L_ERROR("Error: %s\n", NFD_GetError());
        return false;
    }

    cu_strlcpy(out, path, size);
    NFD_FreePath(path);
    return true;
}

static void NewFile()
{
    cu_strlcpy(C.file_name, C.DEFAULT_NAME, MAX_PATH);
    C.text->clear();
    C.editor->SetText(*C.text);
}

static void SaveFile(const char* filename = nullptr)
{

    // If neither argument is not-null nor was the path chosen, abort
    if (!filename && !GetPathFromFileDialog(C.file_name, MAX_PATH, true))
        return;
    // If the file was created using "New" and is called Untitled,
    // but the user has canceled choosing file name, abort saving.
    else if (filename && !cu_strcmp(filename, C.DEFAULT_NAME) && !GetPathFromFileDialog(C.file_name, MAX_PATH, true))
        return;
    // If the name was provided, set it as a current file.
    else if (filename)
        cu_strlcpy(C.file_name, filename, MAX_PATH);

    File file = FileOpen(C.file_name, "w");
    FileWrite(&file, (void*)C.editor->GetText().c_str(), sizeof(char), C.editor->GetText().size());
    FileClose(&file);
}

static void OpenFile(const char* filename = nullptr)
{
    if (filename)
        cu_strlcpy(C.file_name, filename, MAX_PATH);
    else if (!GetPathFromFileDialog(C.file_name, MAX_PATH))
        return;

    File file = FileOpen(C.file_name, "r");
    s64 size = FileSize(&file);
    C.text->resize(size);
    FileRead(&file, (void*)C.text->data(), sizeof(char), C.text->size());
    FileClose(&file);
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
        OpenFile();
    else if (isCtrlOnly && ImGui::IsKeyPressed(ImGuiKey_N))
        NewFile();
    else if (ctrl && shift && ImGui::IsKeyPressed(ImGuiKey_R))
        OpenFile(C.recovery_file_name);
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
                if (ImGui::MenuItem("Open", "Ctrl+O"))
                    OpenFile();
                if (ImGui::MenuItem("Save", "Ctrl+S"))
                    SaveFile(C.file_name);
                if (ImGui::MenuItem("Save As", "Ctrl+Shift+S"))
                    SaveFile();
                if (ImGui::MenuItem("Open recovered file", "Ctrl+Shift+R"))
                    OpenFile(C.recovery_file_name);
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
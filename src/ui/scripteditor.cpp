#include "scripteditor.h"
#include "TextEditor.h"
#include "nfd.h"
extern "C" {
    #include "cld_parser.h"
    #include "culibc.h"
    #include "buffer.h"
    #include "file.h"
    #include "log/log.h"
    #include "memory/arena.h"
    #include "os/crash_handler.h"
    #include "platform.h"
    #include "script/scripting.h"
}
#include <fstream>
#include <streambuf>

static struct ScriptEditorContext
{
    // Defining editor as pointer since its default constructor calls
    // functions that require ImGUI to be already initialized
    TextEditor* editor;
    TextEditor* help_window;
    LuaBindDoc* imgui_docs;
    std::string* text;
    std::string* help_text;
    char          file_name[MAX_PATH] = "scripts/level1.lua";
    char recovery_file_name[MAX_PATH] = "recovered_file.txt";
    const char*          DEFAULT_NAME = "Untitled";
    bool is_help_window_open;
} C;

static void SaveFile(const char* filename);

// Custom terminate handler
void SaveFileOnCrash()
{
    L_FATAL("Saving currently opened script to %s", C.recovery_file_name);
    SaveFile(C.recovery_file_name);
}

void InitializeScriptEditor(Arena* arena)
{
    // Initialise the main script editor
    void* text_editor_address = (void*) PushStruct(arena, TextEditor);
    C.editor = new (text_editor_address) TextEditor();
    C.editor->SetTabSize(4);
    C.editor->SetCompletePairedGlyphs(true);
    void* str_buffer = PushStruct(arena, std::string);
    with_arena(arena)
    {
        Buffer buffer = BufferFromFile(arena, C.file_name);
        C.text = new(str_buffer) std::string((char*)buffer.ptr, buffer.length);
    }
    C.editor->SetText(*C.text);

    // Initialise help windows used for viewing source code of a C function called from lua
    // Used for example for ImGui.
    void* help_window_address = (void*) PushStruct(arena, TextEditor);
    C.help_window = new (help_window_address) TextEditor();
    C.help_window->SetLanguageDefinition(TextEditor::LanguageDefinition::Lua());
    C.help_window->SetReadOnlyEnabled(true);
    str_buffer = PushStruct(arena, std::string);
    with_arena(arena)
    {
        // TODO: replace this hard-coded string with path found in *.cld file
        // once some sort of resource system will be in place.
        // Also, probably put this into a loop so that there can be several
        // *.cld files to benefit from.
        Buffer buffer = BufferFromFile(arena, "../../external/imgui/imgui.h");
        C.help_text = new(str_buffer) std::string((char*)buffer.ptr, buffer.length);
    }
    C.help_window->SetText(*C.help_text);

    C.imgui_docs = LoadLuaDoc(arena, "assets/lua_docs/imgui.cld");

    TextEditor::LanguageDefinition& lang_def = TextEditor::LanguageDefinition::Lua();
    lang_def.RegisterIdentifiers(C.imgui_docs->names, C.imgui_docs->num);
    C.editor->SetLanguageDefinition(lang_def);

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
    SaveFile(C.file_name);
    ExecuteScriptFile(C.file_name);
}

static void ShowHelp()
{
    std::string word = C.editor->GetCurrentIdentifier();
    LuaBindDoc* docs = C.imgui_docs;

    // Find the index of the function with the given name
    int function_index = -1;
    for (int i = 0; i < docs->num; i++)
    {
        if (!cu_strcmp(docs->names[i], word.c_str()))
        {
            function_index = i;
            break;
        }
    }

    static int last_function_index = -1;
    static int overload_index = 0;
    // If a function was found, set view at a given line
    if (function_index != -1)
    {
        // Compute index to the lines array, +1 because the first element is
        // the number of overloads
        int line_index = overload_index + 1;
        int line_number = docs->lines[function_index][line_index];
        
        C.help_window->SetViewAtLine(line_number,
            TextEditor::SetViewAtLineMode::LastVisibleLine);
        C.is_help_window_open = true;
        
        // Cycle through the available overloads for the next call
        int overloads_num = docs->lines[function_index][0];
        overload_index = (overload_index + 1) % overloads_num;
    }
    else
    {
        overload_index = 0;
    }
    
    last_function_index = function_index;
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
    else if (ImGui::IsKeyPressed(ImGuiKey_F1))
        ShowHelp();
    else if (ImGui::IsKeyPressed(ImGuiKey_DownArrow) ||
             ImGui::IsKeyPressed(ImGuiKey_UpArrow))
        C.is_help_window_open = false;
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
    float window_width;
    ImVec2 window_pos;
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
        window_width = ImGui::GetWindowWidth();
        window_pos = ImGui::GetWindowPos();
    }
    ImGui::End();

    if (C.is_help_window_open)
    {
        int flags = ImGuiWindowFlags_NoTitleBar |
                    ImGuiWindowFlags_NoScrollbar |
                    ImGuiWindowFlags_NoBackground |
                    ImGuiWindowFlags_NoResize;
        if (ImGui::Begin("Help window", nullptr, flags))
        {
            const int help_window_height = 80;
            ImGui::SetWindowSize(ImVec2(window_width, help_window_height), 0);
            window_pos.y -= help_window_height;
            ImGui::SetWindowPos(window_pos);
            const ImVec2 temp;
            C.help_window->Render("HelpWindow", false, temp, false,
                ImGuiWindowFlags_NoScrollbar);
        }
        ImGui::End();
    }
}
#include "level_loader.h"
#include "culibc.h"
#include "file.h"
#include "log/log.h"
#include "memory/thread_scratch.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef enum TokenType
{
    TokenType_EndOfFile,

    TokenType_Label,
    TokenType_ObjectName,
    TokenType_Number,

    TokenType_Unknown

} TokenType;

typedef struct Token
{
    TokenType type;
    u64 length;
    char* string;
} Token;

typedef struct Tokenizer
{
    char* at;
} Tokenizer;

void DEBUG_PrintLevelData(LevelData *level) {
    printf("Objects: %d\n", level->count);
    for (int i = 0; i < level->count; i++) {
        printf("Obj: %s\n", level->objects[i].name);
        printf("  Pos: %.2f %.2f %.2f\n", level->objects[i].position.x, level->objects[i].position.y, level->objects[i].position.z);
        printf("  Dim: %.2f %.2f %.2f\n", level->objects[i].dimensions.x, level->objects[i].dimensions.y, level->objects[i].dimensions.z);
        printf("  Col: %.2f %.2f %.2f\n", level->objects[i].colour.x, level->objects[i].colour.y, level->objects[i].colour.z);
    }
}

inline bool IsEndOfLine(char c) {
    return (c == '\n' || c == '\r');
}

inline bool IsWhitespace(char c) {
    return (c == ' ' || c == '\t' || c == '\n' || c == '\r');
}
static void EatAllWhitespace(Tokenizer *tokenizer) {
    while (IsWhitespace(*tokenizer->at)) {
        tokenizer->at++;
    }
}

inline bool IsAlpha(char c) {
    return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'));
}

inline bool IsNumber(char c)
{
    bool result = ((c >= '0') && (c <= '9')) || c == '-';
    return result;
}

inline bool IsFpNumber(char c)
{
    bool result = IsNumber(c) || c == '.';

    return result;
}

static Token GetToken(Tokenizer* tokenizer)
{
    EatAllWhitespace(tokenizer);

    Token token = {
        .length = 1,
        .string = tokenizer->at
    };

    char c = tokenizer->at[0];
    tokenizer->at++;

    if (!c)
    {
        token.type = TokenType_EndOfFile;
    }
    else if (IsAlpha(c))
    {
        if (strncmp(tokenizer->at - 1, "obj", 3) == 0 ||
            strncmp(tokenizer->at - 1, "pos", 3) == 0 ||
            strncmp(tokenizer->at - 1, "col", 3) == 0 ||
            strncmp(tokenizer->at - 1, "dim", 3) == 0)
        {
            token.type = TokenType_Label;
            token.length = 3;
            tokenizer->at += 3;
        }
        else
        {
            token.type = TokenType_ObjectName;
            while (IsAlpha(tokenizer->at[0]) ||
                IsNumber(tokenizer->at[0]) ||
                (tokenizer->at[0] == '.') ||
                (tokenizer->at[0] == '_'))
            {
                tokenizer->at++;
            }

            token.length = tokenizer->at - token.string;
        }
    }
    else if (IsFpNumber(c))
    {
        token.type = TokenType_Number;
        while (IsFpNumber(tokenizer->at[0]))
        {
            tokenizer->at++;
        }

        token.length = tokenizer->at - token.string;
    }
    else
    {
        token.type = TokenType_Unknown;
    }

    return token;
}

static void CopyTokenToString(Arena* arena, Token* token, char** string)
{
    *string = PushArray(arena, char, token->length + 1);
    cu_strlcpy(*string, token->string, token->length + 1);
}

static float TokenToFloat(Arena* arena, Token* token)
{
    char* line_str;
    CopyTokenToString(arena, token, &line_str);
    return atof(line_str);
}

LevelData* LoadTextLevelData(Arena* arena, const char* filename)
{
    LevelData* level = NULL;
    Arena* scratch = GetScratchArena();
    with_arena(scratch)
    {
        char* data = (char*)CStringFromFile(scratch, filename);
        if (!data)
        {
            L_WARN("Parsing %s failed, file not loaded", filename);
            ArenaMarkerRollback(marker); // defined by with_arena
            return NULL;
        }

        level = PushStruct(arena, LevelData);

        // Pass 1 - collect data needed for allocation
        int count = 0;
        Tokenizer tokenizer = {
            .at = data
        };

        bool parsing = true;
        while (parsing)
        {
            Token token = GetToken(&tokenizer);

            switch (token.type)
            {
                case TokenType_EndOfFile:
                {
                    parsing = false;
                } break;

                case TokenType_ObjectName:
                {
                    count++;
                } break;
            }
        }

        level->objects = PushArray(arena, ObjectData, count);

        // Pass 2 - fill the structure
        tokenizer.at = data;
        parsing = true;
        while (parsing)
        {
            Token token = GetToken(&tokenizer);
            if (token.type == TokenType_EndOfFile)
            {
                parsing = false;
                break;
            }
            else if (token.type == TokenType_Label)
            {
                bool is_pos_label = cu_strncmp(token.string, "pos", 3) == 0;
                bool is_dim_label = cu_strncmp(token.string, "dim", 3) == 0;
                bool is_col_label = cu_strncmp(token.string, "col", 3) == 0;
                if (cu_strncmp(token.string, "obj", 3) == 0)
                {
                    Token name_token = GetToken(&tokenizer);
                    if (name_token.type != TokenType_ObjectName)
                    {
                        L_WARN("Expected object name after 'obj'");
                    }
                    else
                    {
                        CopyTokenToString(arena, &name_token, &level->objects[level->count++].name);
                    }
                }
                else if (is_pos_label || is_dim_label || is_col_label)
                {
                    Token number_token_1 = GetToken(&tokenizer);
                    Token number_token_2 = GetToken(&tokenizer);
                    Token number_token_3 = GetToken(&tokenizer);
                    if (number_token_1.type == TokenType_Number &&
                        number_token_2.type == TokenType_Number &&
                        number_token_3.type == TokenType_Number)
                    {
                        if (is_pos_label)
                        {
                            level->objects[level->count - 1].position.x = TokenToFloat(scratch, &number_token_1);
                            level->objects[level->count - 1].position.y = TokenToFloat(scratch, &number_token_2);
                            level->objects[level->count - 1].position.z = TokenToFloat(scratch, &number_token_3);
                        }
                        else if (is_dim_label)
                        {
                            level->objects[level->count - 1].dimensions.x = TokenToFloat(scratch, &number_token_1);
                            level->objects[level->count - 1].dimensions.y = TokenToFloat(scratch, &number_token_2);
                            level->objects[level->count - 1].dimensions.z = TokenToFloat(scratch, &number_token_3);
                        }
                        else if (is_col_label)
                        {
                            level->objects[level->count - 1].colour.x = TokenToFloat(scratch, &number_token_1);
                            level->objects[level->count - 1].colour.y = TokenToFloat(scratch, &number_token_2);
                            level->objects[level->count - 1].colour.z = TokenToFloat(scratch, &number_token_3);
                        }
                    }
                    else
                    {
                        L_WARN("Expected 3 floating point numbers after '%s'", token.string);
                    }
                }
            }
        }
    }
    // DEBUG_PrintLevelData(level);
    return level;
}


#include "cld_parser.h"
#include "file.h"
#include "culibc.h"
#include "log/log.h"
#include "memory/arena.h"
#include "memory/thread_scratch.h"
#include <string.h>
#include <stdlib.h>

typedef enum TokenType
{
    TokenType_EndOfFile,

    TokenType_HeaderFile,
    TokenType_FunctionName,
    TokenType_LineNumber,

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

inline bool IsEndOfLine(char c)
{
    bool result = ((c == '\n') ||
                   (c == '\r'));

    return result;
}

inline bool IsWhitespace(char c)
{
    bool result = ((c == ' ') ||
                   (c == '\t') ||
                   (c == '\n') ||
                   (c == '\r'));

    return result;
}

static void EatAllWhitespace(Tokenizer* tokenizer)
{
    for(;;)
    {
        if(IsWhitespace(tokenizer->at[0]))
        {
            tokenizer->at++;
        }
        else
        {
            break;
        }
    }
}

inline bool IsAlpha(char c)
{
    bool result = ((c >= 'a') && (c <= 'z')) ||
                  ((c >= 'A') && (c <= 'Z'));

    return result;
}

inline bool IsNumber(char c)
{
    bool result = ((c >= '0') && (c <= '9'));

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
        token.type = TokenType_FunctionName;

        while (IsAlpha(tokenizer->at[0]) ||
               IsNumber(tokenizer->at[0]) ||
               (tokenizer->at[0] == '.') ||
               (tokenizer->at[0] == '_'))
        {
            if (tokenizer->at[0] == '.')
            {
                token.type = TokenType_HeaderFile;
            }
            tokenizer->at++;
        }

        token.length = tokenizer->at - token.string;
    }
    else if (IsNumber(c))
    {
        token.type = TokenType_LineNumber;
        while (IsNumber(tokenizer->at[0]))
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

static int TokenToInt(Arena* arena, Token* token)
{
    char* line_str;
    CopyTokenToString(arena, token, &line_str);
    return atoi(line_str);
}

LuaBindDoc* LoadLuaDoc(Arena* arena, const char* filename)
{
    LuaBindDoc* doc = NULL;
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

        doc = PushStruct(arena, LuaBindDoc);

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

                case TokenType_FunctionName:
                {
                    count++;
                } break;
            }
        }
        doc->names = PushArray(arena, char*, count);
        doc->lines = PushArray(arena, int*, count);

        // Pass 2 - fill the structure
        tokenizer.at = data;
        parsing = true;
        while (parsing)
        {
            Token token = GetToken(&tokenizer);

            switch (token.type)
            {
                case TokenType_EndOfFile:
                {
                    parsing = false;
                } break;

                case TokenType_HeaderFile:
                {
                    CopyTokenToString(arena, &token, &doc->source);
                } break;

                case TokenType_FunctionName:
                {
                    CopyTokenToString(arena, &token, &doc->names[doc->num]);
                } break;

                case TokenType_LineNumber:
                {
                    // Do a test run throguh numbers to find alloc size
                    Tokenizer temp_tokenizer = {
                        .at = tokenizer.at
                    };

                    int array_size = 2;
                    while (GetToken(&temp_tokenizer).type == TokenType_LineNumber)
                    {
                        array_size++;
                    }

                    // Fill the table
                    int index = doc->num++;
                    doc->lines[index] = PushArray(arena, int, array_size);
                    doc->lines[index][0] = array_size - 1;
                    doc->lines[index][1] = TokenToInt(scratch, &token);

                    for (int i = 2; i < array_size; i++)
                    {
                        token = GetToken(&tokenizer);
                        doc->lines[index][i] = TokenToInt(scratch, &token);
                    }
                } break;
            }
        }
    }
    return doc;
}

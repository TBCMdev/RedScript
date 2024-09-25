#include "lexer.h"
#include "lang.h"

#ifdef __cplusplus
std::vector<std::string> retrieveInbuiltFiles()
{
    std::vector<std::string> ret;
    std::filesystem::directory_iterator iterator(REDSCRIPT_PATH);

    for(std::filesystem::directory_entry dir : iterator)
        ret.push_back(dir.path().stem().string());

    return ret;
}
lex_error _preprocess(ltoken** tokens, int tCount, std::string dirPath, std::string& contentOut, int* lenOut)
{
    lex_error __STACK_TRACE = {0, 0, 0, 0, 0};
    int _At = -1;
    //TODO
    // TODO: make all token dereferencing most performant
    std::vector<std::string> _IncludedFiles, _InbuiltFiles = retrieveInbuiltFiles();
    std::unordered_map<std::string, std::string> _Macros;
    while(++_At < tCount)
    {
        ltoken& t = (*tokens)[_At];

        switch(t._Type)
        {
            case HASHTAG:
            {
                ltoken& name = (*tokens)[++_At];

                if(seq(name._Repr, "use"))
                {
                    if(_At + 1 >= tCount)
                        T_COMPILE_ERROR(EXPECTED_VALUE, *tokens);
                    std::string name = (*tokens)[++_At]._Repr;

                    if(std::count(_IncludedFiles.begin(), _IncludedFiles.end(), name) > 0)
                        T_COMPILE_ERROR(FILE_ALREADY_INCLUDED, *tokens);

                    std::string path;

                    if(std::count(_InbuiltFiles.begin(), _InbuiltFiles.end(), name) > 0)
                        path = REDSCRIPT_PATH "\\" + name + ".rsc";
                    else path = std::filesystem::absolute(dirPath + "\\" + name + ".rsc").string();

                    _IncludedFiles.push_back(name);

                    if(!std::filesystem::exists(path))
                        T_COMPILE_ERROR(FILE_DOESNT_EXIST, *tokens);

                    // copied from main, not good practice.
                    char* _FileBuffer;
                    if(sfread(path.data(), &_FileBuffer) == -1)
                        T_COMPILE_ERROR(FILE_READ_ERROR, *tokens);

                    int _FileBufferLen = strlen(_FileBuffer);
                    ltoken* _newtokens = (ltoken*) malloc(sizeof(ltoken) * INITIAL_GEN_CAPACITY);
                    int* ftokenCount = (int*) malloc(sizeof(int));
                    
                    std::string mainFileDirectory = std::filesystem::absolute(name).parent_path().string();
                    lex_error _Status = _lex(&_newtokens, _FileBuffer, _FileBufferLen, ftokenCount);

                    if(_Status.ec < 0)
                    {
                        elprint(_Status, _FileBuffer);
                        exit(_Status.ec);
                    }
                    _Status = _preprocess(&_newtokens, *ftokenCount, mainFileDirectory, contentOut, lenOut); 

                    if(_Status.ec < 0)
                    {
                        elprint(_Status, _FileBuffer);
                        exit(_Status.ec);
                    }

                    if(_Status.ec < 0)
                    {
                        elprint(_Status, _FileBuffer);
                        exit(_Status.ec);
                    }
                    *tokens = (ltoken*) realloc(*tokens, sizeof(ltoken) * (tCount + *ftokenCount));
                    if(*tokens == nullptr)
                        T_COMPILE_ERROR(MEMORY_ERROR, *tokens);
                    memmove(*tokens + *ftokenCount, *tokens, sizeof(ltoken) * tCount);
                    memmove(*tokens, _newtokens, sizeof(ltoken) * (*ftokenCount));

                    tCount += *ftokenCount;
                    _At += *ftokenCount;
                    
                    int lineCount = 0;
                    int c = -1;
                    while(++c < _FileBufferLen)
                        if (_FileBuffer[c] == '\n') lineCount++;
                    for(int i = *ftokenCount; i < tCount; i++)
                    {
                        ltoken& t = (*tokens)[i];
                        t._Trace.at += _FileBufferLen;
                        t._Trace.line += lineCount;
                    }
                    
                    contentOut.insert(0, _FileBuffer);
                }
                else if (seq(name._Repr, "macro"))
                {
                    std::string name = (*tokens)[++_At]._Repr;
                    std::string value = (*tokens)[++_At]._Repr;

                    _Macros.insert({name, value});

                    int _Caret = _At;

                    ltoken* d_toks = *tokens;

                    while(_Caret < tCount)
                    {
                        ltoken& at = d_toks[++_Caret];

                        if (name == at._Repr)
                        {
                            at._Repr = name.data();


                            _lex(&d_toks[_Caret], at._Repr, name.length(), nullptr);
                        }
                    }

                }
                break;
            }
            default:
                break;
        }
    }

    *lenOut = tCount;
    return __STACK_TRACE;
}
#endif

lex_error _lex(ltoken** _gout, char *content, int clen, int *lenOut)
{
    lex_error __STACK_TRACE = {0, 0, 0, 0, 0};

    int _TokenCount = 0;

    int _GenLength = INITIAL_GEN_CAPACITY;

    LEX_IFFAIL(*_gout);

    for (int i = 0; i < clen; i++)
    {
        char c = content[i];
        __STACK_TRACE.cpos += i - __STACK_TRACE.at > 0 ? i - __STACK_TRACE.at : 1;
        __STACK_TRACE.at = i;

        if(c == '\t') continue;
        if (c == '\n')
        {
            __STACK_TRACE.lineindex = i;
            __STACK_TRACE.line++;
            __STACK_TRACE.cpos = 0;
            continue;
        }

        if (_TokenCount >= _GenLength)
        {
            _GenLength *= 2;
            *_gout = (ltoken*)realloc(*_gout, sizeof(ltoken) * _GenLength);
        }
        if(c == '/' && i + 1 < clen)
        {
            if(content[i + 1] == '*')
            {
                i ++;
                while(++i < clen)
                {
                    __STACK_TRACE.cpos += i - __STACK_TRACE.at > 0 ? i - __STACK_TRACE.at : 1;
                    __STACK_TRACE.at = i;
                    if(content[i] == '*' &&
                    i + 1 < clen && content[i + 1] == '/') 
                    {
                        i ++;
                        break;
                    }
                    if(content[i] == '\t') continue;
                    if (content[i] == '\n')
                    {
                        __STACK_TRACE.lineindex = i;
                        __STACK_TRACE.line++;
                        __STACK_TRACE.cpos = 0;
                        continue;
                    }
                }
                if(i == clen - 1)
                    LEX_ERROR(CLOSING_TOKEN_NOT_FOUND);
                continue;
            }
            else if (content[i + 1] == '/') 
            {
                while(i + 1 < clen && (c = content[++i]) != '\n')
                    __STACK_TRACE.cpos += i - __STACK_TRACE.at > 0 ? i - __STACK_TRACE.at : 1;
                continue;
            }
        }
        else if (isalpha(c) || c == '_')
        {
            bool isSelector = false;
            int l = i;
            char n;
            int _S = 0;

            while (i + 1 < clen && isalpha((n = content[i + 1])) || n == '_')
                i++;

            int size = i + 1 - l;
            char *_Word = (char *)malloc((size + 1) * sizeof(char));
            LEX_IFFAIL(_Word);
_stradd:
            strncpy(_Word + (_S * sizeof(char)), &content[l], size);

            if (content[i + 1] == ':' && content[i + 2] == ':')
            {
                i+=2;
                _S += size;
                while (i + 1 < clen && isalpha((n = content[i + 1])) || n == '_')
                    i++;
                size = i + 1 - l;
                _Word = (char*)realloc(_Word, (size + 1) * sizeof(char));
                LEX_IFFAIL(_Word);
                isSelector = true;
                goto _stradd;
                
            }
            _Word[size] = '\0';

            ltoken t = {_Word, WORD, 0, __STACK_TRACE};

            int _ExtraInfo = -1;
            token_type _t = UNKNOWN;

            if(isSelector)
            {
                // this::thing::
            }else
            {
                if ((_ExtraInfo = istypedef(_Word)) != -1)
                    t._Type = TYPE_DEF;
                else if ((_t = iskwd(_Word, &_ExtraInfo)) != UNKNOWN)
                    t._Type = _t;
            }
            

            t._Info = _ExtraInfo;

            (*_gout)[_TokenCount++] = t;
        }
        else if (c == '"' || c == '\'')
        {
            char n;
            // error here
            const char _End = c == '"' ? '"' : '\'';

            int l = i + 1;

            while (i + 1 < clen && (n = content[i + 1]) != _End)
            {
                i++;
                if (n == '\\')
                {
                    if (i < clen)
                    {
                        n = content[++i];
                        if (n == _End)
                            continue;
                    }
                }
            }
            i++;
            if (n != _End){
                LEX_ERROR(STRING_LITERAL_NOT_CLOSED);
            }

            int size = i - l;
            char* _Word = (char*)malloc((size + 1) * sizeof(char));
            LEX_IFFAIL(_Word);

            strncpy(_Word, &content[l], size);

            _Word[size] = '\0';

            (*_gout)[_TokenCount++] = {_Word, STRING_LITERAL, 0, __STACK_TRACE};
        }
        else if (IS_OP(c) || c == '=')
        {
            int mallocSize = 2;
            if(i + 1 < clen && (IS_OP(content[i + 1]) || content[i + 1] == '=')) 
            {
                i++;
                mallocSize = 3;
            }
            char *op = (char *)malloc(sizeof(char) * mallocSize);
            LEX_IFFAIL(op);

            op[0] = c;
            if(mallocSize == 2) op[1] = '\0';
            else
            {
                op[1] = content[i];
                op[2] = '\0';
            }

            if(c == '=' && mallocSize == 2)
                (*_gout)[_TokenCount++] = {op, ASSIGN, c, __STACK_TRACE};
            else
                (*_gout)[_TokenCount++] = {op, mallocSize == 2 ? OPERATOR : COMPARISON_OPERATOR, c, __STACK_TRACE};
        }
        else if (isdigit(c))
        {
            char n;

            int l = i;
            int isFloat = 0;

            while (i + 1 < clen && (isdigit((n = content[i + 1])) || n == '.'))
            {
                if (n == '.')
                {
                    if (isFloat)
                        LEX_ERROR(INVALID_NUMERICAL_NOTATION);
                    isFloat = 1;
                }
                i++;
            }

            i++;

            int size = i - l;
            char *_Num = (char *)malloc((size + 1) * sizeof(char));
            LEX_IFFAIL(_Num);
            
            strncpy(_Num, &content[l], size);
            _Num[size] = '\0';

            LEX_IFFAIL(_Num);

            (*_gout)[_TokenCount++] = {_Num, isFloat ? FLOAT_LITERAL : INTEGER_LITERAL, 0, __STACK_TRACE};
        }
        else if (c == '@')
        {
            char* ch;
            if(content[i+2] == '[')
            {
                int x = i+3;

                if(content[x] == ']')
                    LEX_ERROR(EXPECTED_VALUE);
                
                while(x < clen && content[++x] != ']');

                if(x == clen) LEX_ERROR(CLOSING_TOKEN_NOT_FOUND);

                ch = (char*)malloc(sizeof(char) * ((x - i) + 3));

            }else ch = (char *)malloc(sizeof(char) * 3);
            
            LEX_IFFAIL(ch);
            
            ch[0] = c;
            ch[1] = content[++i];
            ch[2] = '\0';
            (*_gout)[_TokenCount++] = {ch, SELECTOR, c, __STACK_TRACE};

        }
        else if (c != ' ')
        {
            char *ch = (char *)malloc(sizeof(char) * 2);
            LEX_IFFAIL(ch);
            ch[0] = c;
            ch[1] = '\0';

            token_type type = CHARACTER;
            switch(c)
            {
                case '(':
                    type = OPEN_BR;
                    break;
                case ')':
                    type = CLOSED_BR;
                    break;
                case '{':
                    type = OPEN_CBR;
                    break;
                case '}':
                    type = CLOSED_CBR;
                    break;
                case '!':
                    type = NEGATOR;
                    break;
                case ':':
                    type = COLON;
                    break;
                case ';':
                    type = SEMI_COLON;
                    break;
                case '#':
                    type = HASHTAG;
                    break;
                case ',':
                    type = COMMA;
                    break;
                case '[':
                    type = OPEN_SQBR;
                    break;
                case ']':
                    type = CLOSED_SQBR;
                    break;
               
            }

            (*_gout)[_TokenCount++] = {ch, type, c, __STACK_TRACE};
        }
    }
    
    *_gout = (ltoken*)realloc(*_gout, sizeof(ltoken) * _TokenCount);

    LEX_IFFAIL(*_gout);
 
    *lenOut = _TokenCount;
 
    return __STACK_TRACE;
}
#include "lexer.h"
#include "lang.h"
lex_error _lex(ltoken *_gout, char *content, int clen, int *lenOut)
{
    lex_error __STACK_TRACE = {0, 0, 0, 0, 0};

    int _TokenCount = 0;

    int _GenLength = INITIAL_GEN_CAPACITY;

    LEX_IFFAIL(_gout);

    for (int i = 0; i < clen; i++)
    {
        char c = content[i];
        __STACK_TRACE.cpos += i - __STACK_TRACE.at > 0 ? i - __STACK_TRACE.at : 1;
        __STACK_TRACE.at = i;

        if(c == '\t') continue;
        if (c == '\n')
        {
            // this line is for ember, as we dont have semi colons.
            // _gout[_TokenCount++] = {NULL, NEWLINE, 0, __STACK_TRACE};

            __STACK_TRACE.lineindex = i;
            __STACK_TRACE.line++;
            __STACK_TRACE.cpos = 0;
            continue;
        }

        if (i == _GenLength)
        {
            _GenLength *= 2;
            _gout = (ltoken*)realloc(_gout, _GenLength);
        }
        if (isalpha(c) || c == '_')
        {
            int l = i;
            char n;

            while (i + 1 < clen && isalpha((n = content[i + 1])) || n == '_')
                i++;

            int size = i + 1 - l;
            char *_Word = (char *)malloc((size + 1) * sizeof(char));
            LEX_IFFAIL(_Word);
            strncpy(_Word, &content[l], size);
            _Word[size] = '\0';

            ltoken t = {_Word, WORD, 0, __STACK_TRACE};

            int _ExtraInfo = -1;
            token_type _t = UNKNOWN;

            if ((_ExtraInfo = istypedef(_Word)) != -1)
                t._Type = TYPE_DEF;
            else if ((_t = iskwd(_Word, &_ExtraInfo)) != UNKNOWN)
                t._Type = _t;
            else if (isinb(_Word, &_ExtraInfo))
                t._Type = INB_FUNC_DEF;

            t._Info = _ExtraInfo;
            _gout[_TokenCount++] = t;
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

            _gout[_TokenCount++] = {_Word, STRING_LITERAL, 0, __STACK_TRACE};
        }
        else if (IS_OP(c))
        {
            char *op = (char *)malloc(sizeof(char) * 2);
            op[0] = c;
            op[1] = '\0';

            _gout[_TokenCount++] = {op, OPERATOR, c, __STACK_TRACE};
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
            strncpy(_Num, &content[l], size);
            _Num[size] = '\0';

            LEX_IFFAIL(_Num);

            _gout[_TokenCount++] = {_Num, isFloat ? FLOAT_LITERAL : INTEGER_LITERAL, 0, __STACK_TRACE};
        }
        else if (c == '@')
        {
            char *ch = (char *)malloc(sizeof(char) * 3);
            ch[0] = c;
            ch[1] = content[++i];
            ch[2] = '\0';
            _gout[_TokenCount++] = {ch, SELECTOR, c, __STACK_TRACE};

        }
        else if (c != ' ')
        {
            char *ch = (char *)malloc(sizeof(char) * 2);
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
                    break; // ...etc 
                case ':':
                    type = COLON;
                    break;
                case ';':
                    type = SEMI_COLON;
                    break;
                case '=':
                    type = ASSIGN;
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

            _gout[_TokenCount++] = {ch, type, c, __STACK_TRACE};
        }
    }
    *lenOut = _TokenCount;
    return __STACK_TRACE;
}
#ifndef ERROR_H
#define ERROR_H

#include <stdio.h>

#define MEMORY_ERROR -1
#define STRING_LITERAL_NOT_CLOSED -2
#define INVALID_NUMERICAL_NOTATION -3

#define UNKNOWN_TYPE -4
#define EXPECTED_VALUE -5
#define EXPECTED_OPERATOR -6
#define UNEXPECTED_TOKEN -7
#define EXPECTED_SEMICOLON -8
#define EXPECTED_TYPE -9
#define NAME_ALREADY_EXISTS -10
#define EXPECTED_FUNCTION_BODY -11
#define UNSUPPORTED_FEATURE -12
#define UNDEFINED_TOKEN_IDENTIFIER -13
#define NO_ENTRY_POINT -14
#define FUNCTION_REDECLARATION -15
#define NOT_SUPPORTED -16
#define INVALID_TOKEN_LOCATION -17
#define COMPARISON_TYPE_MISMATCH -18
#define CLOSING_TOKEN_NOT_FOUND -19
#define FILE_ALREADY_INCLUDED -20
#define FILE_DOESNT_EXIST -21
#define FILE_READ_ERROR -22
#define INCLUDED_FILE_EMPTY -23
typedef struct lex_error_t
{
    int ec; // error code
    int at; // char in whole file.
    int lineindex; // index of '\n' char.
    int line = 1; // line index
    int cpos; // caret pos
} lex_error;


void elprint(lex_error, char*);
void errstr(int, char*, int);
void warn(const char*);
// #define BLK "\e[0;30m"
// #define RED "\e[0;31m"
// #define GRN "\e[0;32m"
// #define YEL "\e[0;33m"
// #define BLU "\e[0;34m"
// #define MAG "\e[0;35m"
// #define CYN "\e[0;36m"
// #define WHT "\e[0;37m"

// //Regular bold text
// #define BBLK "\e[1;30m"
// #define BRED "\e[1;31m"
// #define BGRN "\e[1;32m"
// #define BYEL "\e[1;33m"
// #define BBLU "\e[1;34m"
// #define BMAG "\e[1;35m"
// #define BCYN "\e[1;36m"
// #define BWHT "\e[1;37m"

// //Regular underline text
// #define UBLK "\e[4;30m"
// #define URED "\e[4;31m"
// #define UGRN "\e[4;32m"
// #define UYEL "\e[4;33m"
// #define UBLU "\e[4;34m"
// #define UMAG "\e[4;35m"
// #define UCYN "\e[4;36m"
// #define UWHT "\e[4;37m"

// //Regular background
// #define BLKB "\e[40m"
// #define REDB "\e[41m"
// #define GRNB "\e[42m"
// #define YELB "\e[43m"
// #define BLUB "\e[44m"
// #define MAGB "\e[45m"
// #define CYNB "\e[46m"
// #define WHTB "\e[47m"

// //High intensty background 
// #define BLKHB "\e[0;100m"
// #define REDHB "\e[0;101m"
// #define GRNHB "\e[0;102m"
// #define YELHB "\e[0;103m"
// #define BLUHB "\e[0;104m"
// #define MAGHB "\e[0;105m"
// #define CYNHB "\e[0;106m"
// #define WHTHB "\e[0;107m"

// //High intensty text
// #define HBLK "\e[0;90m"
// #define HRED "\e[0;91m"
// #define HGRN "\e[0;92m"
// #define HYEL "\e[0;93m"
// #define HBLU "\e[0;94m"
// #define HMAG "\e[0;95m"
// #define HCYN "\e[0;96m"
// #define HWHT "\e[0;97m"

// //Bold high intensity text
// #define BHBLK "\e[1;90m"
// #define BHRED "\e[1;91m"
// #define BHGRN "\e[1;92m"
// #define BHYEL "\e[1;93m"
// #define BHBLU "\e[1;94m"
// #define BHMAG "\e[1;95m"
// #define BHCYN "\e[1;96m"
// #define BHWHT "\e[1;97m"

// //Reset
// #define reset "\e[0m"
// #define CRESET "\e[0m"
// #define COLOR_RESET "\e[0m"

#endif //ERROR_H

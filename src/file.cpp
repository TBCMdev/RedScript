#include <stdlib.h>
#include <sys\types.h>
#include <sys\stat.h>
#include "file.h"

#define _CRT_INTERNAL_NONSTDC_NAMES 1
#if !defined(S_ISREG) && defined(S_IFMT) && defined(S_IFREG)
#define S_ISREG(m) (((m) & S_IFMT) == S_IFREG)
#endif
#if !defined(S_ISDIR) && defined(S_IFMT) && defined(S_IFDIR)
#define S_ISDIR(m) (((m) & S_IFMT) == S_IFDIR)
#endif

int isf(char *path)
{
    struct stat st;
    if (stat(path, &st) < 0)
    {
        return 0;
    }
    return S_ISREG(st.st_mode);
}

int sfread(char *path, char **_buff)
{
    int buffSize = 128;

    FILE *_Hwnd = fopen(path, "r");
    if (!_Hwnd)
    {
        fclose(_Hwnd);
        return -1;
    }

    char *buff = (char *)malloc(buffSize * sizeof(char));

    size_t bytes_read = 0;
    char c;
    while ((c = getc(_Hwnd)) != EOF)
    {
        if (bytes_read == buffSize)
        {
            buffSize *= 2;
            char *_nBuff = (char*)realloc(buff, buffSize * sizeof(char));
            if (_nBuff == NULL)
            {
                free(_nBuff);
                fclose(_Hwnd);
                return -1;
            }
            buff = _nBuff;
        }
        buff[bytes_read++] = c;
    }
    if(bytes_read < buffSize)
    {
        char *_nBuff = (char*)realloc(buff, bytes_read * sizeof(char));
        if (_nBuff == NULL)
        {
            free(_nBuff);
            fclose(_Hwnd);
            return -1;
        }
        buff = _nBuff;
    }

    buff[bytes_read] = '\0';

    *_buff = buff;

    fclose(_Hwnd);
    return EXIT_SUCCESS;
}
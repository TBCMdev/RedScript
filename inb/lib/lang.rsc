/*
COPYRIGHT (C) TBCM 2024, All Rights Reserved.
lang.rsc: the file that must be included for all redscript programs.
          It houses all inbuilt function definitions along with program constants, important macros
          and more.
*/

#macro SUCCESS 1
#macro FAILURE 0

int msg(selector__: string, message__: string) __inbuilt;
int kill(selector__: string, message__: string) __inbuilt;
// int give(selector__: string, item__: nselector, quantity__: int = 1, nbt__: json = null) __inbuilt;

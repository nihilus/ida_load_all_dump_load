// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include <stdio.h>
#include <tchar.h>
#include <windows.h>
#include <psapi.h>
#include <commdlg.h>
#include <commctrl.h>
#include "resource.h"

void * x_alloc(size_t size);
void   x_free(void *memory);
void * x_realloc(void *memory, size_t size);

BOOL SetPrivilege(
    HANDLE hToken,          // access token handle
    LPCTSTR lpszPrivilege,  // name of privilege to enable/disable
    BOOL bEnablePrivilege   // to enable or disable privilege
    ) ;

// TODO: reference additional headers your program requires here

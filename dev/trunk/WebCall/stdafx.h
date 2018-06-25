// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

#define _CRT_SECURE_NO_DEPRECATE

// Required for VS 2008 (fails on XP and Win2000 without this fix)
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0500
#endif

// Windows Header Files:
#include <windows.h>

// C RunTime Header Files
/*
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
*/

#include <atlbase.h>
#include <atlstr.h>

#include <list>
#include <mutex>
#include "WebCallWebSocket.h"

#include <objbase.h>
#include "..\DuiLib\UIlib.h"	
using namespace DuiLib;

#include "resource.h"
#include "UIMessage.h"


#define RESOURCE_PATH _T( "skin\\default")

// TODO: reference additional headers your program requires here

void SetRegKey(CString name, CString strValue);
void SetRegKey(CString name, DWORD value);
CString ReadRegKeyString(CString name, CString default = "");
DWORD ReadRegKeyDWORD(CString name, DWORD default = 0);

extern std::string utf8ModuleFilename;
extern std::string utf8Dir;
extern std::string utf8AppDataDir;
extern std::string utf8ModuleName;

void InitLog4cplus();
void UnInitLog4cplus();

extern std::list<WebCallWSclient *> g_WSClientSet;
extern std::recursive_mutex g_WSClientMtx;
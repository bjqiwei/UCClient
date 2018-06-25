// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#pragma once

#ifndef STRICT
#define STRICT
#endif

#include "targetver.h"

#define _ATL_APARTMENT_THREADED

#define _ATL_NO_AUTOMATIC_NAMESPACE

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// some CString constructors will be explicit


#define ATL_NO_ASSERT_ON_DESTROY_NONEXISTENT_WINDOW

#include "resource.h"
#include <atlbase.h>
#include <atlcom.h>
#include <atlctl.h>
#define _WTL_NO_CSTRING
#include <atlstr.h>
#include <string>

extern std::string utf8ModuleFilename;
extern std::string utf8Dir;
extern std::string utf8AppDataDir;
extern std::string utf8ModuleName;

void InitLog4cplus();
void UnInitLog4cplus();
std::string getVersion();

using namespace ATL;
void SetRegKey(CString name, CString strValue);
void SetRegKey(CString name, DWORD value);
CString ReadRegKeyString(CString name, CString default = "");
DWORD ReadRegKeyDWORD(CString name, DWORD default = 0);
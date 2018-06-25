// dllmain.cpp : Implementation of DllMain.

#include "stdafx.h"
#include "resource.h"
#include "WebSocketPlugin_i.h"
#include "dllmain.h"
#include "xdlldata.h"
#include <sstream>
#include <DbgHelp.h>
#include <ShlObj.h>
#include "codingHelper.h"

CWebSocketPluginModule _AtlModule;

#define CALL_FIRST 1  
#define CALL_LAST 0

BOOL CALLBACK MiniDumpCallback(PVOID pParam, const PMINIDUMP_CALLBACK_INPUT   pInput,
	PMINIDUMP_CALLBACK_OUTPUT pOutput)
{


	if (pInput == NULL)
		return FALSE;

	if (pOutput == NULL)
		return FALSE;

	return TRUE;
}

LONG WINAPI
VectoredHandler(struct _EXCEPTION_POINTERS *ExceptionInfo)
{

	if (ExceptionInfo->ExceptionRecord->ExceptionCode == 0x406D1388
		|| ExceptionInfo->ExceptionRecord->ExceptionCode == 0xE06D7363
		|| ExceptionInfo->ExceptionRecord->ExceptionCode == 0x000006BA
		|| ExceptionInfo->ExceptionRecord->ExceptionCode == 0x8001010D
		|| ExceptionInfo->ExceptionRecord->ExceptionCode == 0x80010108
		|| ExceptionInfo->ExceptionRecord->ExceptionCode == 0x40010006
		|| ExceptionInfo->ExceptionRecord->ExceptionCode == 0x80000003
		|| ExceptionInfo->ExceptionRecord->ExceptionCode == 0x80000004
		|| ExceptionInfo->ExceptionRecord->ExceptionCode == 0xC0000095
		|| ExceptionInfo->ExceptionRecord->ExceptionCode == 0x4001000A
		|| ExceptionInfo->ExceptionRecord->ExceptionCode == 0x40080201
		|| ExceptionInfo->ExceptionRecord->ExceptionCode == 0x0000071A
		)
	{
	}
	else
	{
		DWORD pId = GetCurrentProcessId();
		//::MessageBox(nullptr, "", "", MB_OK);
		std::wstringstream oss;
		oss << Utf82Unicode(utf8AppDataDir) << L"\\" << pId << L"." << Utf82Unicode(getVersion()) << L".dmp";
		std::wstring dmpFile = oss.str();
		HANDLE lhDumpFile = CreateFileW(dmpFile.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

		MINIDUMP_EXCEPTION_INFORMATION loExceptionInfo;
		loExceptionInfo.ExceptionPointers = ExceptionInfo;
		loExceptionInfo.ThreadId = GetCurrentThreadId();
		loExceptionInfo.ClientPointers = TRUE;
		if (lhDumpFile != INVALID_HANDLE_VALUE)
		{
			MINIDUMP_CALLBACK_INFORMATION mci;

			mci.CallbackRoutine = MiniDumpCallback;
			mci.CallbackParam = NULL;     // this example does not use the context

			MiniDumpWriteDump(GetCurrentProcess(), pId, lhDumpFile, MINIDUMP_TYPE(
				MiniDumpWithDataSegs |
				MiniDumpWithUnloadedModules |
				MiniDumpWithProcessThreadData),
				&loExceptionInfo,
				NULL,
				NULL);
			CloseHandle(lhDumpFile);
			//MessageBox(NULL, "Stop", "" ,MB_OK);
		}
	}
	/*std::string info = "你所使用的坐席程序发生了一个意外错误，请将此窗口截图和文件\r\n";
	info += dmpFile;
	info += "提交给开发人员，并重启程序。";

	std::string caption = "坐席插件崩溃:";
	caption += CloopenAgentBase::GetVersion();*/
	//MessageBox(NULL, info.c_str() , caption.c_str() , MB_OK);
	return EXCEPTION_CONTINUE_SEARCH;
}

// DLL Entry Point
extern "C" BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
#ifdef _MERGE_PROXYSTUB
	if (!PrxDllMain(hInstance, dwReason, lpReserved))
		return FALSE;
#endif
	hInstance;
	static PVOID h1 = NULL;

	if (dwReason == DLL_PROCESS_ATTACH)
	{
		WCHAR wszPath[MAX_PATH + 1];
		GetModuleFileNameW(hInstance, wszPath, MAX_PATH);
		utf8ModuleFilename = Unicode2Utf8(wszPath);
		// log4cplus initialize;
		utf8ModuleName = utf8ModuleFilename;
		utf8ModuleName = utf8ModuleName.substr(utf8ModuleName.rfind("\\")+1, utf8ModuleName.rfind(".") - (utf8ModuleName.rfind("\\")+1));
		utf8Dir = utf8ModuleFilename.substr(0, utf8ModuleFilename.rfind("\\") + 1);

		std::string logpath = ReadRegKeyString("LogPath");
		utf8AppDataDir = ASCII2UTF_8(logpath+"\\");
		std::wstring wstrAppDataDir = Utf82Unicode(utf8AppDataDir);
		SHCreateDirectoryExW(NULL, wstrAppDataDir.c_str(), nullptr);

		if (h1 == NULL)
		{
			h1 = AddVectoredExceptionHandler(CALL_LAST, VectoredHandler);
		}
		InitLog4cplus();

	}
	else if (dwReason == DLL_THREAD_DETACH)
	{

	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		if (h1) {
			RemoveVectoredExceptionHandler(h1);
			h1 = NULL;
		}
		UnInitLog4cplus();
	}
	return _AtlModule.DllMain(dwReason, lpReserved); 
}

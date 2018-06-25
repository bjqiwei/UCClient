// WebCall.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "MainWnd.h"

#pragma comment( lib, "DbgHelp" )


#define CALL_FIRST 1  
#define CALL_LAST 0

BOOL CALLBACK MiniDumpCallback(PVOID pParam, const PMINIDUMP_CALLBACK_INPUT   pInput,
	PMINIDUMP_CALLBACK_OUTPUT pOutput)

{
	// Callback implementation

	if (pInput == NULL)
		return FALSE;

	if (pOutput == NULL)
		return FALSE;

	/*if (pInput->CallbackType == ModuleCallback)
	{

	std::wstring fullPaht = pInput->Module.FullPath;
	if (fullPaht.find(L"npECBrowserPlugin.dll") != std::wstring::npos || fullPaht.find(L"ECsdk.dll") != std::wstring::npos) {

	}
	else{
	pOutput->ModuleWriteFlags &= (~ModuleWriteDataSeg);
	}

	return TRUE;
	}*/

	return TRUE;

}

LONG WINAPI
VectoredHandler(
	struct _EXCEPTION_POINTERS *ExceptionInfo
)
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
		oss << Utf82Unicode(utf8AppDataDir) << L"\\" << pId << L"." << Utf82Unicode(CPjSipSDK::getVersion()) << L".dmp";
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


int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPSTR /*lpCmdLine*/, int nCmdShow)
{
	CMainWnd::m_SingletonEvent = CreateEvent(NULL, FALSE, FALSE, "90C4B4FE-0795-4442-B387-CBE245838328");
	if (GetLastError() == ERROR_ALREADY_EXISTS) {
		SetEvent(CMainWnd::m_SingletonEvent);
		CloseHandle(CMainWnd::m_SingletonEvent);
		return 0;
	}

	WCHAR wszPath[MAX_PATH];
	GetModuleFileNameW(hInstance, wszPath, MAX_PATH);
	utf8ModuleFilename = Unicode2Utf8(wszPath);
	// log4cplus initialize;
	utf8ModuleName = utf8ModuleFilename;
	utf8ModuleName = utf8ModuleName.substr(utf8ModuleName.rfind("\\"), utf8ModuleName.rfind(".") - utf8ModuleName.rfind("\\"));
	utf8Dir = utf8ModuleFilename.substr(0, utf8ModuleFilename.rfind("\\") + 1);

	std::string logpath = ReadRegKeyString("LogPath");
	utf8AppDataDir = ASCII2UTF_8(logpath);
	std::wstring wstrAppDataDir = Utf82Unicode(utf8AppDataDir);
	SHCreateDirectoryExW(NULL, wstrAppDataDir.c_str(), nullptr);

	HANDLE exceptioH = nullptr;
	exceptioH = AddVectoredExceptionHandler(CALL_LAST, VectoredHandler);

	utf8AppDataDir = ReadRegKeyString("LogPath");

	InitLog4cplus();

	CDuiString str = CPaintManagerUI::GetInstancePath();

	CPaintManagerUI::SetInstance(hInstance);
#if 1
	CPaintManagerUI::SetResourcePath(CPaintManagerUI::GetInstancePath());
	CPaintManagerUI::SetResourceZip(_T("default.zip"));
#else
	CPaintManagerUI::SetResourcePath(CPaintManagerUI::GetInstancePath() + RESOURCE_PATH);
#endif

	HRESULT Hr = ::CoInitialize(NULL);
	if (FAILED(Hr)) return 0;

	CMainWnd *pMainWnd = new CMainWnd();
	if (pMainWnd == NULL) { return 0; }
	pMainWnd->SetIcon(IDI_WEBCALL);
	pMainWnd->Create(NULL, _T("软电话程序"), UI_WNDSTYLE_DIALOG, 0, 0, 0, 0, 0, NULL);
	pMainWnd->CenterWindow();
	pMainWnd->ShowWindow(true);

	CPaintManagerUI::MessageLoop();
	CPaintManagerUI::Term();

	if (CMainWnd::m_sington.joinable()) {
		SetEvent(CMainWnd::m_SingletonEvent);
		CloseHandle(CMainWnd::m_SingletonEvent);
		CMainWnd::m_sington.join();
	}


	::CoUninitialize();
	if (exceptioH) {
		RemoveVectoredExceptionHandler(exceptioH);
		exceptioH = nullptr;
	}
	return 0;
}
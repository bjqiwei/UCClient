// stdafx.cpp : source file that includes just the standard includes
// UCClient.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"
#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>
#include <log4cplus/fileappender.h>
#include "codingHelper.h"

// TODO: reference any additional headers you need in STDAFX.H
// and not in this file


std::string utf8ModuleFilename;
std::string utf8Dir;
std::string utf8AppDataDir;
std::string utf8ModuleName;


static std::atomic_ulong g_Log4cplusReferce = 0;

void InitLog4cplus()
{
	if (g_Log4cplusReferce.fetch_add(1) != 0)
		return;

	std::string logFile;
	logFile = UTF_82ASCII(utf8AppDataDir + utf8ModuleName);
	log4cplus::initialize();
	log4cplus::LogLevel LogLevel = ReadRegKeyDWORD("LogLevel", 20000);
	{
		log4cplus::SharedAppenderPtr _append(new log4cplus::TimeBasedRollingFileAppender(
			logFile, log4cplus::DAILY,
			"%y-%m-%d.log",
			30,
			std::ios_base::binary | std::ios_base::app));

		_append->setName(utf8ModuleName);
		std::string pattern = "%D{%y/%m/%d %H:%M:%S.%Q} [%t] %-5p %c{3} %x -  %m;line:%L %n";
		std::auto_ptr<log4cplus::Layout> _layout(new log4cplus::PatternLayout(pattern));

		_append->setLayout(_layout);
		log4cplus::Logger root = log4cplus::Logger::getRoot();

		root.addAppender(_append);
		root.setLogLevel(LogLevel>= log4cplus::INFO_LOG_LEVEL? LogLevel - log4cplus::INFO_LOG_LEVEL:LogLevel);
	}
	{
		log4cplus::Logger pjsip = log4cplus::Logger::getInstance("pjsip.log");
		pjsip.setLogLevel(LogLevel);

		log4cplus::Logger wslog = log4cplus::Logger::getInstance("wslog");
		wslog.setLogLevel(LogLevel);
	}
}

void UnInitLog4cplus()
{
	//if (g_Log4cplusReferce.fetch_sub(1) == 1);
		//log4cplus::unInitialize();
}

std::string getVersion()
{
	static std::string result;
	if (!result.empty())
		return result;

	result = "0.0.0.0";
	VS_FIXEDFILEINFO *pVerInfo = NULL;
	DWORD dwTemp, dwSize, dwHandle = 0;
	BYTE *pData = NULL;
	UINT uLen;
	std::wstring wstrModule = Utf82Unicode(utf8ModuleFilename);
	dwSize = GetFileVersionInfoSizeW(wstrModule.c_str(), &dwTemp);

	pData = new BYTE[dwSize];

	GetFileVersionInfoW(wstrModule.c_str(), dwHandle, dwSize, pData);
	VerQueryValue(pData, "\\", (void **)&pVerInfo, &uLen);


	DWORD verMS = pVerInfo->dwFileVersionMS;
	DWORD verLS = pVerInfo->dwFileVersionLS;

	int ver[4];
	ver[0] = HIWORD(verMS);
	ver[1] = LOWORD(verMS);
	ver[2] = HIWORD(verLS);
	ver[3] = LOWORD(verLS);

	std::stringstream ss;
	ss << ver[0] << "." << ver[1] << "." << ver[2] << "." << ver[3];
	result = ss.str();
	//LOG4CPLUS_INFO(log, "result:" << result);

	delete[] pData;
	return result;
}


void SetRegKey(CString name, CString strValue)
{
	CRegKey shKey;

	LONG lResult = shKey.Create(HKEY_CURRENT_USER, "SoftWare\\ytx\\UCClient");
	log4cplus::Logger log = log4cplus::Logger::getInstance("UCClient");
	if (lResult != ERROR_SUCCESS)
	{
		LOG4CPLUS_ERROR(log, _T("Create UCClient RegKey failed."));
	}
	else {
		shKey.SetStringValue(name, strValue, REG_SZ);
		LOG4CPLUS_DEBUG(log, __FUNCTION__ " "<< name << ":" << strValue);
	}
	return;
}

void SetRegKey(CString name, DWORD value)
{
	CRegKey shKey;

	LONG lResult = shKey.Create(HKEY_CURRENT_USER, "SoftWare\\ytx\\UCClient");
	log4cplus::Logger log = log4cplus::Logger::getInstance("UCClient");
	if (lResult != ERROR_SUCCESS)
	{
		LOG4CPLUS_ERROR(log, _T("Create UCClient RegKey failed."));
	}
	else {
		shKey.SetDWORDValue(name, value);
		LOG4CPLUS_DEBUG(log, __FUNCTION__ " " << name << ":" << value);
	}
}


CString ReadRegKeyString(CString name, CString default)
{
	CRegKey shKey;
	CString strValue = default;
	ULONG dwLen = MAX_PATH;
	LONG lResult = ERROR_SUCCESS;
	log4cplus::Logger log = log4cplus::Logger::getInstance("UCClient");

	lResult	= shKey.Open(HKEY_CURRENT_USER, "SoftWare\\ytx\\UCClient", KEY_READ);

	if (lResult == ERROR_SUCCESS){
		lResult = shKey.QueryStringValue(name, strValue.GetBuffer(dwLen), &dwLen);
	}

	if (lResult != ERROR_SUCCESS){
		lResult = shKey.Open(HKEY_USERS, ".DEFAULT\\SoftWare\\ytx\\UCClient", KEY_READ);
		if (lResult == ERROR_SUCCESS) {
			dwLen = MAX_PATH;
			lResult = shKey.QueryStringValue(name, strValue.GetBuffer(dwLen), &dwLen);
		}
	}

	strValue.ReleaseBuffer();

	if(lResult != ERROR_SUCCESS) {
		LOG4CPLUS_ERROR(log, _T("Open UCClient RegKey failed."));
	}
	else {
		LOG4CPLUS_DEBUG(log, __FUNCTION__" " << name << ":" << strValue);
	}

	return strValue;
}

DWORD ReadRegKeyDWORD(CString name, DWORD default)
{
	CRegKey shKey;
	DWORD dwValue = default;
	LONG lResult = ERROR_SUCCESS;
	log4cplus::Logger log = log4cplus::Logger::getInstance("UCClient");

	lResult = shKey.Open(HKEY_CURRENT_USER, "SoftWare\\ytx\\UCClient", KEY_READ);
	if (lResult == ERROR_SUCCESS){
		lResult = shKey.QueryDWORDValue(name, dwValue);
	}

	if (lResult != ERROR_SUCCESS)
	{
		lResult = shKey.Open(HKEY_USERS, ".DEFAULT\\SoftWare\\ytx\\UCClient", KEY_READ);
		if (lResult == ERROR_SUCCESS) {
			lResult = shKey.QueryDWORDValue(name, dwValue);
		}
	}

	if(lResult != ERROR_SUCCESS) {
		LOG4CPLUS_ERROR(log, _T("Open UCClient RegKey failed."));
	}
	else {
		
		LOG4CPLUS_DEBUG(log, __FUNCTION__ " " << name << ":" << dwValue);
	}
	return dwValue;
}
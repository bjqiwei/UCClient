// dllmain.h : Declaration of module class.

class CWebSocketPluginModule : public ATL::CAtlDllModuleT< CWebSocketPluginModule >
{
public :
	DECLARE_LIBID(LIBID_WebSocketPluginLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_WEBSOCKETPLUGIN, "{00A2CED3-0C19-495E-B281-5C88F1DC862F}")
};

extern class CWebSocketPluginModule _AtlModule;

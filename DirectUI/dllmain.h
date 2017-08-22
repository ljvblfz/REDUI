// dllmain.h : 模块类的声明。

class CDirectUIModule : public CAtlDllModuleT< CDirectUIModule >
{
public :
	DECLARE_LIBID(LIBID_DirectUILib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_DIRECTUI, "{D798300B-04C9-47EA-AABC-B7AA5FCE3192}")
};

extern class CDirectUIModule _AtlModule;

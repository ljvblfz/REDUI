#ifndef __APILOADER_H__
#define __APILOADER_H__

//////////////////////////////////////////////////////////////////////////
// public API Helper
class ApiLoader
{
	HMODULE hmod;
public:
	ApiLoader(LPCWSTR dllfile, LPCWSTR dllfile2=NULL) : hmod(::LoadLibraryW(dllfile))
	{
		if (hmod==NULL && dllfile2)
			hmod = ::LoadLibraryW(dllfile2);
	}
	~ApiLoader()
	{
		if (hmod)
		{
			::FreeLibrary(hmod);
			hmod = NULL;
		}
	}

	bool IsValid() { return hmod!=NULL; }

	FARPROC operator()(LPCSTR procname) { return (hmod && procname) ? ::GetProcAddress(hmod, procname) : NULL; }
};


#endif // __APILOADER_H__
#ifndef __MENU_H__
#define __MENU_H__

#pragma once

#include <atluser.h>
#include <atlcoll.h>
#include <atlstr.h>

#define MENU_ID_BASE	0x0100
#define MENU_ID_MAX		(MENU_ID_BASE + 1000)

//class 

class PathMenu
{
	CMenu _menu;
	CAtlArray<PathMenu> _submenus;


	static UINT _get_id(bool reset=false)
	{
		static UINT _id = MENU_ID_BASE;
		if (reset || _id == MENU_ID_MAX)
			_id = MENU_ID_BASE;
		return _id ++;
	}

	static CStringW _get_full_path(LPCWSTR szPath)
	{
		static WCHAR _root[MAX_PATH] = L"";
		if (_root[0] == 0)
		{
			::GetModuleFileNameW(NULL, _root, MAX_PATH);
			LPWSTR p = _root + lstrlenW(_root);
			while (*p != L'\\') p--;
			ATLASSERT(*p == L'\\');
			lstrcpyW(p, L"\\demo");
		}

		// NULL means that we will get DEMO root path.
		if (szPath==NULL || *szPath==0) return _root;

		CStringW strPath = _root;
		if (*szPath != L'\\') strPath += L"\\";
		strPath += szPath;
		return strPath;
	}

	//static FILETIME _get_file_create_time(LPCTSTR szFile)
	//{
	//	HANDLE h = ::CreateFile(szFile, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_BACKUP_SEMANTICS, NULL);
	//	FILETIME creation_filetime = {0};
	//	if (h)
	//	{
	//		::GetFileTime(h, &creation_filetime, NULL, NULL);
	//		::CloseHandle(h);
	//	}
	//	return creation_filetime;
	//}

	//static FILETIME _get_favorite_folder_time()
	//{
	//	static FILETIME _ft = {0};
	//	static bool bInit = false;
	//	if (!bInit)
	//	{
	//		bInit = true;
	//		CString fold;
	//		if (FAILED(SHGetFolderPath(NULL, CSIDL_FAVORITES, NULL, SHGFP_TYPE_CURRENT, fold.GetBufferSetLength(_MAX_PATH))))
	//		{
	//			fold.ReleaseBuffer();
	//			return _ft;
	//		}
	//		fold.ReleaseBuffer();
	//		_ft = _get_file_create_time(fold);
	//	}
	//	return _ft;
	//}

	struct _Item
	{
		UINT id;
		CStringW name;
		CStringW relative_path;
	};
	CAtlArray<_Item> _items;

public:
	~PathMenu()
	{
		if (_menu.IsMenu())
			_menu.DestroyMenu();
		_submenus.RemoveAll();
		_items.RemoveAll();
	}

	operator HMENU() { return (HMENU)_menu; }

	BOOL Popup(HWND hWnd, int x, int y)
	{
		//Load();
		return _menu.TrackPopupMenuEx(TPM_LEFTALIGN/*|TPM_VCENTERALIGN*/|TPM_LEFTBUTTON|TPM_TOPALIGN, x, y, hWnd);
	}

	LPCWSTR GetRelativePathById(UINT id)
	{
		for (size_t i=0; i<_items.GetCount(); i++)
		{
			if (_items[i].id == id)
				return _items[i].relative_path;
		}

		for (size_t i=0; i<_submenus.GetCount(); i++)
		{
			LPCWSTR p = _submenus[i].GetRelativePathById(id);
			if (p) return p;
		}

		return NULL;
	}

	CStringW RelativePathToPath(LPCWSTR rPath)
	{
		if (rPath) return _get_full_path(rPath);
		return NULL;
	}

	bool Load(LPCWSTR szFolder=NULL, LPCWSTR szParent=NULL)
	{
		if (szParent==NULL && szFolder==NULL)
			_get_id(true);

		CStringW folder;
		if (szParent && *szParent!=0)
		{
			folder = szParent;
			folder += L"\\";
		}
		folder += szFolder;

		CStringW path = _get_full_path(folder);
		DWORD dw = ::GetFileAttributesW(path);
		if (dw==INVALID_FILE_ATTRIBUTES || (dw&FILE_ATTRIBUTE_DIRECTORY)==0) return false; // DEMO 文件夹不存在

		if (_menu.IsMenu())
			_menu.DestroyMenu();
		_submenus.RemoveAll();
		_items.RemoveAll();

		if (!_menu.CreatePopupMenu())
			return false;

		// 枚举子文件夹和文件
		// 首先处理所有的子文件夹
		WIN32_FIND_DATAW fd;
		HANDLE hFind = ::FindFirstFileW(path + L"\\*.*", &fd);
		if (hFind != INVALID_HANDLE_VALUE)
		{
			do
			{
				CStringW tmp = fd.cFileName;
				if ((fd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY) && tmp!=_T(".") && tmp!=_T(".."))
				{
					PathMenu& sub = _submenus[_submenus.Add()];
					sub.Load(fd.cFileName, folder);
					_menu.AppendMenu(MF_ENABLED, sub, fd.cFileName);
				}
			} while (::FindNextFile(hFind, &fd));
			::FindClose(hFind);
		}

		// 接着处理所有XML文件
		hFind = ::FindFirstFile(path + L"\\*.xml", &fd);
		if (hFind != INVALID_HANDLE_VALUE)
		{
			do
			{
				if ((fd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY) == 0)
				{
					CString tmp = fd.cFileName;
					_Item& item = _items[_items.Add()];
					item.id = _get_id();
					item.name = tmp.Left(tmp.GetLength()-4);
					if (folder.GetLength()>0)
						item.relative_path = folder + L"\\";
					item.relative_path += fd.cFileName;

					_menu.AppendMenu(MF_ENABLED|MF_STRING, item.id, item.name);
				}
			} while (::FindNextFile(hFind, &fd));
			::FindClose(hFind);
		}

		return true;
	}
};

#endif // __MENU_H__
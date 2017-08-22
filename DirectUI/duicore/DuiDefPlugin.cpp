#include "StdAfx.h"
#include "DuiDefPlugin.h"
#include "DuiVisual.h"
#include "DuiLayoutMode.h"

DefPlugin _system_default_plugin;
PluginData g_defPlugin = {NULL, NULL};


DefPlugin::DefPlugin()
{
	g_defPlugin.cb = (fnPluginCallback)&DefPlugin::_PluginEntry;
	g_defPlugin.data = this;
	m_hPlugin = DuiHandle<PluginData>(&g_defPlugin);
}

DefPlugin::~DefPlugin()
{
	m_hPlugin = NULL;
	g_defPlugin.cb = NULL;
	g_defPlugin.data = NULL;
}

LPCWSTR DefPlugin::GetNormalName( LPCWSTR szAlias, LPCWSTR szNameType )
{
	static const struct
	{
		LPCWSTR type; // xml|null == xml
		LPCWSTR normalName;
		LPCWSTR alias1;
		LPCWSTR alias2;
	} _name_map[] =
	{
		//{ L"xml",		L"Anchor",			L"a",	L"link"		},
		//{ L"xml",		L"Font",			L"f",	NULL		},
		//{ L"xml",		L"Bold",			L"b",	L"strong"	},
		//{ L"xml",		L"Italic",			L"i",	NULL		},
		//{ L"xml",		L"Underline",		L"u",	NULL		},
		//{ L"xml",		L"Strike",			L"s",	NULL		},
		//{ L"xml",		L"WhiteSpace",		L"w",	NULL		},
		//{ L"xml",		L"Paragraph",		L"p",	NULL		},
		//{ L"xml",		L"LineBreak",		L"br",	NULL		},
		//{ L"xml",		L"HorizontalLine",	L"hr",	NULL		},
		//{ L"xml",		L"ActiveX",			L"Object",	L"Embed"},
		//{ L"xml",		L"Edit",			L"TextBox",	L"Text"	},
		//{ L"xml",		L"List.Header",		L"Header",	NULL	},
		//{ L"xml",		L"List.Footer",		L"Footer",	NULL	},
		{ NULL,			NULL,				NULL,	NULL		}
	};

	LPCWSTR type = szNameType;
	if (type==NULL)
		type = L"xml";

	for (int i=0; i<sizeof(_name_map)/sizeof(_name_map[0])-1; i++)
	{
		if (lstrcmpiW(type, _name_map[i].type) == 0)
		{
			if (lstrcmpiW(szAlias, _name_map[i].normalName)==0 ||
				(_name_map[i].alias1 && lstrcmpiW(szAlias, _name_map[i].alias1)==0) ||
				(_name_map[i].alias2 && lstrcmpiW(szAlias, _name_map[i].alias2)==0))
			{
				return _name_map[i].normalName;
			}
		}
	}

	// 不在表里
	if (lstrcmpiW(type, L"xml") == 0) // XML 标签命名系统
	{
		for (ControlExtensionEntry** ppEntry=&__pctrlextEntryFirst; ppEntry < &__pctrlextEntryLast; ppEntry++)
		{
			if (*ppEntry != NULL)
			{
				ControlExtensionEntry* pEntry = *ppEntry;
				if (lstrcmpiW(pEntry->szName, szAlias)==0 ||
					(pEntry->szAlias && lstrcmpiW(pEntry->szAlias, szAlias)==0) ||
					(pEntry->szAlias2 && lstrcmpiW(pEntry->szAlias2, szAlias)==0))
				{
					return pEntry->szName;
				}
			}
		}
		return szAlias; // 对于XML标签，总是返回原名称
	}
	else
		return SetHandled(FALSE), szAlias;
}

int DefPlugin::Test( int n1, int n2, int n3 )
{
	return n1 + n2 - n3;
}

IVisual* DefPlugin::QueryVisual( LPCWSTR szVisualName )
{
	return VisualFromName(szVisualName);
}

BOOL DefPlugin::PaintBorder( HDE hde, HDC hdc, RECT rcBorder )
{
	return GetDefaultVisual()->OnPaintBorder(hde, hdc, rcBorder);
}

BOOL DefPlugin::PaintFocus( HDE hde, HDC hdc, RECT rcFocus, HRGN hrgn )
{
	return GetDefaultVisual()->OnPaintFocus(hde, hdc, rcFocus, hrgn);
}

BOOL DefPlugin::PaintBackground( HDE hde, HDC hdc, RECT rcBkgnd )
{
	return GetDefaultVisual()->OnPaintBackground(hde, hdc, rcBkgnd);
}

BOOL DefPlugin::PaintForeground( HDE hde, HDC hdc, RECT rcFrgnd )
{
	return GetDefaultVisual()->OnPaintForeground(hde, hdc, rcFrgnd);
}

ILayout* DefPlugin::QueryLayout( LPCWSTR szLayoutName )
{
	return LayoutFromName(szLayoutName);
}

BOOL DefPlugin::OnUpdateLayout( HDE hde )
{
	return GetDefaultLayout()->OnUpdateLayout(hde);
}

//UINT DefPlugin::GetControlFlags( HDE hde )
//{
//	static const struct 
//	{
//		LPCWSTR tagname;
//		UINT flags;
//	} _flags_map[] =
//	{
//		{ L"Button",		DUIFLAG_TABSTOP	},
//		{ L"Option",		DUIFLAG_TABSTOP	},
//		{ L"CheckBox",		DUIFLAG_TABSTOP	},
//		{ L"Anchor",		DUIFLAG_SETCURSOR|DUIFLAG_TABSTOP	},
//		{ L"ActiveX",		DUIFLAG_NOFOCUSFRAME | DUIFLAG_NOCHILDREN	},
//		{ L"Edit",			DUIFLAG_NOFOCUSFRAME | DUIFLAG_NOCHILDREN | DUIFLAG_TABSTOP	}, // | ((m_dwProps&TXTBIT_MULTILINE) ? DUIFLAG_WANTRETURN : 0
//		{ L"xaml",			DUIFLAG_NOFOCUSFRAME | DUIFLAG_NOCHILDREN	},
//
//		{ NULL,				0}
//	};
//}

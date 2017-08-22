// MainDlg.cpp : implementation of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"

#include "MainDlg.h"
extern CMainDlg* gpDlg;



CComBSTR GetFileVersion(BSTR path);

CMainDlg::CMainDlg() : m_bstrPath(L"default.xml")
{
}

BOOL CMainDlg::PreTranslateMessage(MSG* pMsg)
{
	return CWindow::IsDialogMessage(pMsg);
}

BOOL CMainDlg::OnIdle()
{
	return FALSE;
}

LRESULT CMainDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	_pAtlModule->Lock();
	// center the dialog on the screen
	CenterWindow();

	// set icons
	HICON hIcon = (HICON)::LoadImage(_AtlBaseModule.GetResourceInstance(), MAKEINTRESOURCE(IDR_MAINFRAME), 
		IMAGE_ICON, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON), LR_DEFAULTCOLOR);
	SetIcon(hIcon, TRUE);
	HICON hIconSmall = (HICON)::LoadImage(_AtlBaseModule.GetResourceInstance(), MAKEINTRESOURCE(IDR_MAINFRAME), 
		IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), LR_DEFAULTCOLOR);
	SetIcon(hIconSmall, FALSE);

	// register object for message filtering and idle updates
	g_msgLoop.AddMessageFilter(this);
	g_msgLoop.AddIdleHandler(this);

	UIAddChildWindowContainer(m_hWnd);

	// 禁用关闭按钮
	HMENU hMenu = ::GetSystemMenu(m_hWnd, FALSE);
	if (hMenu)
	{
		::RemoveMenu(hMenu, SC_CLOSE, MF_BYCOMMAND);
	}

	CComBSTR text;
	GetWindowText(&text);
	text.Append(L" - ");
	text.Append(GetFileVersion(NULL));
	text.Append(L"(CORE: ");
	text.Append(GetFileVersion(L"redui"));
	text.Append(L")");
	SetWindowText(text);

	m_wndShow.Reset();
	if (m_wndShow.m_bstrXml.Length() == 0)
		LoadUrl(L"demo/default.xml", FALSE);
	m_wndShow.UpdateXml();

	DoDataExchange();

	gpDlg = this;
	return TRUE;
}

LRESULT CMainDlg::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	gpDlg = NULL;

	// unregister message filtering and idle updates
	g_msgLoop.RemoveMessageFilter(this);
	g_msgLoop.RemoveIdleHandler(this);

	if (m_wndShow.IsWindow())
	{
		m_wndShow.DestroyWindow();
		m_wndShow.m_hWnd = NULL;
	}

	return 0;
}

LRESULT CMainDlg::OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: Add validation code 
	if (::GetFocus() == GetDlgItem(IDC_EDIT_EXPRESSION))
	{
		BOOL b=TRUE;
		return OnBnClickedBtnCalc(0,0,NULL,b);
	}
	CloseDialog(IDOK);
	return 0;
}

LRESULT CMainDlg::OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	//CloseDialog(wID);
	return 0;
}

void CMainDlg::CloseDialog(int nVal)
{
	DestroyWindow();
	if (_pAtlModule->Unlock() == 0)
		::PostQuitMessage(nVal);
}

LRESULT CMainDlg::OnBtnRenderBnClicked(WORD wNotifyCode, WORD wID, HWND hWndCtl)
{
	DoDataExchange(TRUE);
	m_wndShow.Reset();
	m_wndShow.UpdateXml();
	return 0;
}

BOOL CDuiShowWin::PreTranslateMessage( MSG* pMsg )
{
	return IsDirectUIMessage(pMsg);
}

//LRESULT CDuiShowWin::OnCreate( UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/ )
//{
//	g_msgLoop.AddMessageFilter(this);
//	// TODO: 在此添加消息处理程序代码和/或调用默认值
//	//CComPtr<IStream> stm;
//	//::StreamFromUrl(L"default.xml", &stm, NULL);
//	//::BstrFromStream(stm, &m_bstrXml);
//	//LoadFromXml(m_bstrXml);
//	////LoadFromUrl(_T("default.xml"));
//
//	return 0;
//}

LRESULT CDuiShowWin::OnDestroy( UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled )
{
	bHandled = FALSE;
	g_msgLoop.RemoveMessageFilter(this);
	UnsubclassWindow();
	return 0;
}

void CDuiShowWin::Reset()
{
	RECT rc = {50, 50, 500, 500};
	if (!IsWindow())
	{
		//Create(NULL, rc, NULL, CFrameWinTraits::GetWndStyle(0), CFrameWinTraits::GetWndExStyle(0));

		CDuiWindow win;
		HWND hwnd = win.Create(NULL, rc, NULL, CFrameWinTraits::GetWndStyle(0), CFrameWinTraits::GetWndExStyle(0));
		if (win.IsWindow())
		{
			SubclassWindow(win.m_hWnd);
			g_msgLoop.AddMessageFilter(this);
			// 禁用关闭按钮
			HMENU hMenu = ::GetSystemMenu(hwnd, FALSE);
			if (hMenu)
			{
				::RemoveMenu(hMenu, SC_CLOSE, MF_BYCOMMAND);
			}
		}
		//if (::IsWindow(hwnd))
		//	SubclassWindow(hwnd);

		if (IsWindow())
			ShowWindow(TRUE);
		return;
	}

	if (!IsWindowVisible())
		ShowWindow(TRUE);
	if (IsIconic())
		ShowWindow(SW_RESTORE);
	if (!IsWindowEnabled())
		EnableWindow(TRUE);
}

void CDuiShowWin::UpdateXml()
{
	RECT rc;
	GetWindowRect(&rc);

	BOOL bLayerOld = (GetWindowLong(GWL_EXSTYLE) & WS_EX_LAYERED);
	LoadFromXml(m_bstrXml);
	BOOL bLayer = (GetWindowLong(GWL_EXSTYLE) & WS_EX_LAYERED);

	if (bLayer != bLayerOld)
	{
		if (bLayer)
		//if ((GetWindowLong(GWL_EXSTYLE) & WS_EX_LAYERED) == 0)
		{
			SetWindowLong(GWL_STYLE, GetWindowLong(GWL_STYLE)&~WS_OVERLAPPEDWINDOW);
			SetWindowLong(GWL_EXSTYLE, GetWindowLong(GWL_EXSTYLE)&~(WS_EX_APPWINDOW | WS_EX_WINDOWEDGE));
		}
		else
		{
			SetWindowLong(GWL_STYLE, GetWindowLong(GWL_STYLE)|WS_OVERLAPPEDWINDOW);
			SetWindowLong(GWL_EXSTYLE, GetWindowLong(GWL_EXSTYLE)|(WS_EX_APPWINDOW | WS_EX_WINDOWEDGE));
		}
	}

	ShowWindow(TRUE);
	MoveWindow(&rc);
	if (bLayer!=bLayerOld)
	{
		SendMessage(WM_USER+888);
		//RECT rcClient;
		//GetClientRect(&rcClient);
		//SendMessage(WM_SIZE, SIZE_RESTORED, MAKELPARAM((rcClient.right-rcClient.left), (rcClient.bottom-rcClient.top)));
	}
}

LRESULT CMainDlg::OnDropFiles(HDROP hdrop)
{
	UINT iCnt = ::DragQueryFileW(hdrop, 0xFFFFFFFF, NULL, 0);
	if (iCnt == 0) return 0;

	OLECHAR file[MAX_PATH] = L"";
	::DragQueryFileW(hdrop, 0, file, MAX_PATH);

	CComPtr<IStream> stm;
	::StreamFromUrl(file, &stm, NULL);
	m_wndShow.m_bstrXml.Empty();
	CComBSTR bstr;
	if (::BstrFromStream(stm, &bstr))
	{
		m_wndShow.m_bstrXml = bstr;
		m_bstrPath = file;
		//m_wndCurrentPath.SetWindowText(file);
		DoDataExchange();
	}

	return 0;
}

void CMainDlg::LoadUrl( LPCOLESTR szUrl, BOOL bUpdatePath/*=TRUE*/ )
{
	CComPtr<IStream> stm;
	::StreamFromUrl(szUrl, &stm, NULL);
	m_wndShow.m_bstrXml.Empty();
	CComBSTR bstr;
	if (::BstrFromStream(stm, &bstr))
	{
		if (bUpdatePath)
		{
			//if (StrCmpNIW(szUrl, L"demo", 4)==0 && (szUrl[4]==L'\\' || szUrl[4]==L'/'))
			//	m_bstrPath = szUrl + 5;
			//else
				m_bstrPath = szUrl;
		}
		m_wndShow.m_bstrXml = bstr;
		if (IsWindow())
			DoDataExchange();
	}
}
LRESULT CMainDlg::OnNMClickSyslink(int /*idCtrl*/, LPNMHDR pNMHDR, BOOL& bHandled)
{
	// TODO: 在此添加控件通知处理程序代码
	PNMLINK pNMLink = (PNMLINK)pNMHDR;
	if (lstrcmpiW(pNMLink->item.szID, L"helpcenter") == 0)
	{
		static WCHAR cmd[MAX_PATH+256] = L"";
		if (cmd[0] == 0)
		{
			lstrcpyW(cmd, L"mshta.exe res://");
			LPWSTR p = cmd + lstrlenW(cmd);
			::GetModuleFileNameW(NULL, p, MAX_PATH);
			lstrcatW(p, L"/helpcenter.htm");
		}

		STARTUPINFOW si = {sizeof(STARTUPINFOW)};
		PROCESS_INFORMATION pi = {0};
		BOOL b = CreateProcessW(NULL, cmd, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
		if (pi.hThread) CloseHandle(pi.hThread);
		if (pi.hProcess) CloseHandle(pi.hProcess);
	}
	else if (pNMLink->item.szUrl[0] != 0)
	{
		::ShellExecuteW(NULL, L"open", pNMLink->item.szUrl, NULL, NULL, SW_SHOWNORMAL);
	}

	return 0;
}

//LRESULT CMainDlg::OnEnSetfocusEditExpression(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//{
//	// TODO: 在此添加控件通知处理程序代码
//	GetDlgItem(IDC_BTN_CALC).SendMessage(DM_SETDEFID, IDC_BTN_CALC);
//
//	return 0;
//}

LRESULT CMainDlg::OnBnClickedBtnCalc(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: 在此添加控件通知处理程序代码
	DoDataExchange(TRUE, IDC_EDIT_EXPRESSION);
	CComVariant v;
	m_wndShow.CalcExpression(m_bstrExpression, &v);
	if (SUCCEEDED(v.ChangeType(VT_BSTR)))
		m_bstrResult = v.bstrVal;
	else
		m_bstrResult = L"No results.";

	DoDataExchange(FALSE, IDC_EDIT_RESULT);

	return 0;
}

LRESULT CMainDlg::OnBnClickedBtnLoad(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: 在此添加控件通知处理程序代码
	RECT rc;
	GetDlgItem(IDC_BTN_LOAD).GetWindowRect(&rc);
	POINT pt = {rc.left, rc.bottom};
	//ClientToScreen(&pt);
	if (m_menu.Load())
		m_menu.Popup(m_hWnd, pt.x, pt.y);
	else
		MessageBox(L"No XML Files.", L"Information", MB_OK | MB_ICONINFORMATION);

	return 0;
}

LRESULT CMainDlg::OnBnClickedBtnSave(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: 在此添加控件通知处理程序代码
	DoDataExchange(TRUE);

	CDestPathDlg dlg(this);
	dlg.m_strPath = m_bstrPath; //m_wndShow.m_bstrXml;
	if (dlg.DoModal() == IDOK)
	{
		MessageBox(L"Succeeded to save!\n\nYou can load it from menu later.", L"Operation", MB_OK | MB_ICONINFORMATION);
	}
	
	return 0;
}

LRESULT CMainDlg::OnMenu(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	LPCWSTR rpath = m_menu.GetRelativePathById(wID);
	if (rpath)
	{
		m_bstrPath = rpath;
		DoDataExchange(FALSE, IDC_STATIC_CURRENT_PATH);
		//GetDlgItem(IDC_STATIC_CURRENT_PATH).RedrawWindow();
		//m_wndCurrentPath.SetWindowText(rpath);
		//m_wndCurrentPath.RedrawWindow();
		///*m_wndCurrentPath.*/UpdateWindow();
		LoadUrl(m_menu.RelativePathToPath(rpath), FALSE);
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////////
CDestPathDlg::CDestPathDlg(CMainDlg* dlg) : m_mainDlg(dlg)
{
	ATLASSERT(m_mainDlg);
}

LRESULT CDestPathDlg::OnInitDialog( UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/ )
{
	DoDataExchange();
	return TRUE;
}

#define mbWarning L"Warning", MB_OK | MB_ICONWARNING
#define mbError L"Error", MB_OK | MB_ICONERROR
LRESULT CDestPathDlg::OnOK( WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/ )
{
	DoDataExchange(TRUE);

	CStringW file = m_strPath;
	file.Trim();
	if (file.GetLength()==0)
	{
		MessageBox(L"No file name!", mbError);
		return 0;
	}
	if (file.Right(4).CompareNoCase(L".xml") != 0)
		file += L".xml";
	DWORD dwAtt = ::GetFileAttributesW(file);
	if (dwAtt==INVALID_FILE_ATTRIBUTES || (dwAtt & FILE_ATTRIBUTE_DIRECTORY))
		file = m_mainDlg->m_menu.RelativePathToPath(file);
	int index = file.ReverseFind(L'\\');
	
	WCHAR pDir[MAX_PATH];
	lstrcpyW(pDir, file);
	pDir[index] = 0;
	LPWSTR pFile = pDir + index + 1;

	// auto create directory
	int result = ::SHCreateDirectory(NULL, pDir);
	if (result == ERROR_BAD_PATHNAME)
	{
		MessageBox(L"Bad file path!", mbError);
		return 0;
	}

	DWORD dw = ::GetFileAttributesW(file);
	if (dw!=INVALID_FILE_ATTRIBUTES && (dw&FILE_ATTRIBUTE_DIRECTORY)==0 && (dw&FILE_ATTRIBUTE_ARCHIVE))
	{
		if (IDOK != MessageBox(L"File exists! Overwrite?", L"Overwrite confirm", MB_OKCANCEL | MB_ICONQUESTION))
			return 0;
	}

	// 转换内容到UTF-8
	int size = ::WideCharToMultiByte(CP_UTF8, 0, m_mainDlg->m_wndShow.m_bstrXml, (int)m_mainDlg->m_wndShow.m_bstrXml.Length(), NULL, 0, NULL, NULL);
	if (size<=0)
	{
		MessageBox(L"Failed to translate file content!", mbError);
		return 0;
	}
	LPBYTE pBuf = new BYTE[size + 3]; // 3 means UTF-8 BOM
	if (pBuf==NULL)
	{
		MessageBox(L"Out of memory.", mbError);
		return 0;
	}
	pBuf[0] = 0xEF;
	pBuf[1] = 0xBB;
	pBuf[2] = 0xBF;
	::WideCharToMultiByte(CP_UTF8, 0, m_mainDlg->m_wndShow.m_bstrXml, (int)m_mainDlg->m_wndShow.m_bstrXml.Length(), (LPSTR)pBuf+3, size, NULL, NULL);

	// 开始写文件
	HANDLE hFile = ::CreateFileW(file, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		MessageBox(L"Failed to create/open file! You can try to change to other file name.", mbError);
		delete[] pBuf;
		return 0;
	}

	DWORD dwWrite;
	::WriteFile(hFile, pBuf, (DWORD)(size+3), &dwWrite, NULL);

	delete[] pBuf;
	::CloseHandle(hFile);

	EndDialog(IDOK);
	return 0;
}

LRESULT CDestPathDlg::OnCancel( WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/ )
{
	EndDialog(IDCANCEL);
	return 0;
}
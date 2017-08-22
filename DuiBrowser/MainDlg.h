// MainDlg.h : interface of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once
#include <atlddx.h>
#include <atlcrack.h>

#include "Menu.h"

#include "../DirectUI/DirectUIWindow.h"

extern CMessageLoop g_msgLoop;

class CDuiShowWin : public CWindowImpl<CDuiShowWin, CDuiWindow, CFrameWinTraits>
	, public CMessageFilter
{
public:
	CComBSTR m_bstrXml;

	BEGIN_MSG_MAP(CDuiShowWin)
		//MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
	END_MSG_MAP()

	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

	void Reset();
	void UpdateXml();
};


class CMainDlg : public CDialogImpl<CMainDlg>, public CUpdateUI<CMainDlg>,
	public CMessageFilter, public CIdleHandler,
	public CWinDataExchange<CMainDlg>
{
public:
	CDuiShowWin m_wndShow;
	PathMenu m_menu;
	CStatic m_wndOutput;CComBSTR m_bstrExpression;CComBSTR m_bstrResult;
	//CStatic m_wndCurrentPath;
	CComBSTR m_bstrPath;




public:
	enum { IDD = IDD_MAINDLG };

	CMainDlg();

	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnIdle();

	BEGIN_UPDATE_UI_MAP(CMainDlg)
	END_UPDATE_UI_MAP()

	BEGIN_MSG_MAP(CMainDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MSG_WM_DROPFILES(OnDropFiles)
		COMMAND_ID_HANDLER(IDOK, OnOK)
		COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
		COMMAND_HANDLER_EX(IDC_BTN_RENDER, BN_CLICKED, OnBtnRenderBnClicked)
		NOTIFY_HANDLER(IDC_SYSLINK, NM_CLICK, OnNMClickSyslink)
		NOTIFY_HANDLER(IDC_SYSLINK, NM_RETURN, OnNMClickSyslink)
		//COMMAND_HANDLER(IDC_EDIT_EXPRESSION, EN_SETFOCUS, OnEnSetfocusEditExpression)
		COMMAND_HANDLER(IDC_BTN_CALC, BN_CLICKED, OnBnClickedBtnCalc)
		COMMAND_HANDLER(IDC_BTN_LOAD, BN_CLICKED, OnBnClickedBtnLoad)
		COMMAND_HANDLER(IDC_BTN_SAVE, BN_CLICKED, OnBnClickedBtnSave)
		COMMAND_RANGE_HANDLER(MENU_ID_BASE, MENU_ID_MAX, OnMenu)
	END_MSG_MAP()

	BEGIN_DDX_MAP(CMainDlg)
		DDX_TEXT(IDC_EDIT_XML, m_wndShow.m_bstrXml)
		DDX_CONTROL_HANDLE(IDC_STATIC_OUTPUT, m_wndOutput)
		DDX_TEXT(IDC_EDIT_EXPRESSION, m_bstrExpression)
		DDX_TEXT(IDC_EDIT_RESULT, m_bstrResult)
		DDX_TEXT(IDC_STATIC_CURRENT_PATH, m_bstrPath)
		//DDX_CONTROL/*_HANDLE*/(IDC_STATIC_CURRENT_PATH, m_wndCurrentPath)
	END_DDX_MAP()

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

	void CloseDialog(int nVal);
	LRESULT OnBtnRenderBnClicked(WORD wNotifyCode, WORD wID, HWND hWndCtl);
	LRESULT OnDropFiles(HDROP hdrop);

	void LoadUrl(LPCOLESTR szUrl, BOOL bUpdatePath=TRUE);
	LRESULT OnNMClickSyslink(int /*idCtrl*/, LPNMHDR pNMHDR, BOOL& /*bHandled*/);
	//LRESULT OnEnSetfocusEditExpression(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedBtnCalc(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedBtnLoad(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedBtnSave(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnMenu(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
};

class CDestPathDlg : public ATL::CDialogImpl<CDestPathDlg>,
	public CWinDataExchange<CDestPathDlg>
{
public:
	CMainDlg* m_mainDlg;
	CComBSTR m_strPath;
public:

public:
	enum { IDD = IDD_SAVETODLG };

	CDestPathDlg(CMainDlg* dlg);

	BEGIN_MSG_MAP(CDestPathDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(IDOK, OnOK)
		COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
	END_MSG_MAP()

	BEGIN_DDX_MAP(CDestPathDlg)
		DDX_TEXT_LEN(IDC_EDIT_PATH, m_strPath, 128)
	END_DDX_MAP()

	// Handler prototypes (uncomment arguments if needed):
	//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
};

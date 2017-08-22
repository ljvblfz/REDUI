#include "stdafx.h"
#include "DuiBase.h"
#include "DuiD3DImpl.h"
#include "Dui3DObject.h"
#include "Dui3DAnimation.h"
#include "DuiStyleParser.h"


//////////////////////////////////////////////////////////////////////////

#define DeviceBuildResource(restype) \
	restype* pRes = restype::New(this); \
	if (pRes==NULL) return TRUE; \
	__if_exists(restype::GetDispatch) { if (ppDisp) { *ppDisp = pRes->GetDispatch(); (*ppDisp)->AddRef(); } } \
	__if_exists(restype::ParseAttribute) { pRes->ParseAttribute(pNode); } \
	__if_exists(restype::IsChildNode) { \
		for (CMarkupNode child=n->GetChild(); child.IsValid(); child=child.GetSibling()) \
			pRes->IsChildNode(&child); \
	}


//////////////////////////////////////////////////////////////////////////

bool d3d::IsValid(bool bAutoCreate/*=false*/)
{
	if (bAutoCreate)
		g_d3d.D3DCreate();

	return g_d3d.isValid();
}

void d3d::OnThreadFinish()
{
	g_d3d.ThreadClear();
}

COLORREF d3d::KeyColor()
{
	return KEYCOLOR_RGB;
}

HWND d3d::SetActiveScriptWindow( HWND hwnd )
{
	HWND h = gt.hwndActiveScript;
	gt.hwndActiveScript = hwnd;
	return h;
}

//////////////////////////////////////////////////////////////////////////

HRESULT variant2time(VARIANT* val, float& f)
{
	if (val==NULL || val->vt==VT_EMPTY || val->vt==VT_ERROR) return E_INVALIDARG;

	// 首先尝试字符串转换
	if (val->vt == VT_BSTR)
	{
		CStrArray strs;
		if (SplitStringToArray(val->bstrVal, strs, L":") && strs.GetSize()>=1)
		{
			// 最多只有冒号分离的3项
			float h=0, m=0, s=0;
			int count = strs.GetSize();
			// 读秒
			if (count >= 1)
			{
				CComVariant v = strs[count-1];
				if (SUCCEEDED(v.ChangeType(VT_R4)))
					s = v.fltVal;
			}
			// 读分
			if (count >= 2)
			{
				CComVariant v = strs[count-2];
				if (SUCCEEDED(v.ChangeType(VT_R4)))
					m = v.fltVal;
			}
			// 读时
			if (count >= 3)
			{
				CComVariant v = strs[count-3];
				if (SUCCEEDED(v.ChangeType(VT_R4)))
					h = v.fltVal;
			}
			// 求和
			f = h * 3600.f + m * 60.f + s;
			return S_OK;
		}
		return E_INVALIDARG;
	}

	// 再尝试直接赋值 FLOAT
	CComVariant v;
	if (SUCCEEDED(v.ChangeType(VT_R4, val)))
		return (f = v.fltVal), S_OK;
	return E_INVALIDARG;
}


//////////////////////////////////////////////////////////////////////////
BOOL GdiPath::RelativePoint::Parse( LPCOLESTR szx, LPCOLESTR szy )
{
	ATLASSERT(szx && szy && *szx && *szy);
	flags = 0;

	if (szx[0]==L'#' || szx[0]==L'*' || szx[0]==L'&')
	{
		xDir = TRUE;
		szx++;
	}
	if (!TryLoad_long_FromString(szx, x))
	{
		if (TryLoad_float_FromString(szx, fx))
			xPercent = TRUE;
		else return FALSE;
	}

	if (szy[0]==L'#' || szy[0]==L'*' || szy[0]==L'&')
	{
		yDir = TRUE;
		szy++;
	}
	if (!TryLoad_long_FromString(szy, y))
	{
		if (TryLoad_float_FromString(szy, fy))
			yPercent = TRUE;
		else return FALSE;
	}

	return TRUE;
}

long GdiPath::RelativePoint::GetX( const RECT& rc )
{
	if (xPercent)
	{
		if (xDir) return rc.right - (long)((rc.right - rc.left) * fx);
		else return rc.left + (long)((rc.right - rc.left) * fx);
	}
	else
	{
		if (xDir) return rc.right - x;
		else return rc.left + x;
	}
}

long GdiPath::RelativePoint::GetY( const RECT& rc )
{
	if (yPercent)
	{
		if (yDir) return rc.bottom - (long)((rc.bottom - rc.top) * fy);
		else return rc.top + (long)((rc.bottom - rc.top) * fy);
	}
	else
	{
		if (yDir) return rc.bottom - y;
		else return rc.top + y;
	}
}

//////////////////////////////////////////////////////////////////////////

GdiPath::GdiPath() : _timestamp(0), _path_type(pt_unknown)
{

}

/*
 *	路径描述串语法：
 *		[op [#|*|&]x [#|*|&]y] [op [#|*|&]x [#|*|&]y] ...
 *	其中 op 用一个字母来表示操作，M(MoveTo)、L(LineTo)、B(BezierTo)、C(CloseFigure)
 *	x y 是相对于左上角的相对值，例如 “1 2” 表示相对左上偏离 (1,2)的位置。如果需要相对于另一个方向，则在数字前加前缀 #|*|& 的任何一个，表示相对右边或下边
 */
BOOL GdiPath::Parse( LPCOLESTR strPath )
{
	_timestamp = timeGetTime();
	_path_type = pt_unknown;

	_points.RemoveAll();
	_types.RemoveAll();

	if (strPath==NULL) return FALSE;

	static LPCOLESTR token = L", ()\t\r\n";

	CStrArray strs;
	if (SplitStringToArray(strPath, strs, token))
	{
		// 特殊处理固定裁剪方案
		int num = strs.GetSize();
		if (num == 0) return TRUE;
		strs[0].MakeLower();
		int len = strs[0].GetLength();

		//if (strs[0].CompareNoCase(L"roundrect")==0)
		//{
		//	long corner = 10; // 默认圆角半径
		//	if (num>1)
		//		TryLoad_long_FromString(strs[1], corner);
		//	CStringW szRoundRect = L"M(0,%d) B((0,c3),(c3,0),(%d,0)) L(#%d,0) B((#c3,0),(#0,c3),(#0,%d)) L(#0,#%d) B((#0,#c3),(#c3,#0),(#%d,#0)) L(%d,#0) B((c3,#0),(0,#c3),(0,#%d))";
		//	//szRoundRect.Format(L"M(0,%d) B((0,0),(0,0),(%d,0)) L(#%d,0) B((#0,0),(#0,0),(#0,%d)) L(#0,#%d) B((#0,#0),(#0,#0),(#%d,#0)) L(%d,#0) B((0,#0),(0,#0),(0,#%d))",
		//	//	corner,corner,corner,corner,corner,corner,corner,corner);
		//	CStringW szCorner, szCorner3;
		//	szCorner.Format(L"%d", corner);
		//	szCorner3.Format(L"%d", (long)(corner/3));
		//	szRoundRect.Replace(L"%d", szCorner);
		//	szRoundRect.Replace(L"c3", szCorner3);
		//	SplitStringToArray(szRoundRect, strs, token);
		//	num = strs.GetSize();
		//}

		if (len >= 2 && CStringW(L"roundrect").Find(strs[0]) >= 0) // [ro]undrect
		{
			RelativePoint pt(10,10);
			if (num>=2 && !TryLoad_long_FromString(strs[1], pt.x)) return FALSE;
			pt.y = pt.x;
			if (num>=3 && !TryLoad_long_FromString(strs[2], pt.y)) return FALSE;

			_points.Add(pt);
			_path_type = pt_roundrect;
			return TRUE;
		}

		else if (len>=2 && CStringW(L"rectangle").Find(strs[0]) >= 0) // [re]ctangle
		{
			RelativePoint topleft, rightbottom;
			rightbottom.xDir = rightbottom.yDir = TRUE;

			if (num>=2 && !topleft.Parse(strs[1], num>=3?strs[2]:L"0")) return FALSE;
			if (num>=4 && !rightbottom.Parse(strs[3], num>=5?strs[4]:L"#0")) return FALSE;

			_points.Add(topleft);
			_points.Add(rightbottom);
			_path_type = pt_rectangle;
			return TRUE;
		}

		else if (len>=2 && num>=5 && CStringW(L"polygon").Find(strs[0])>=0) // [po]lygon
		{
			RelativePoint pt1, pt2;
			if (!pt1.Parse(strs[1], strs[2]) || !pt2.Parse(strs[3], strs[4])) return FALSE;
			_points.Add(pt1);
			_points.Add(pt2);

			for (int i=5; i+1<num; i+=2)
			{
				RelativePoint pt;
				if (!pt.Parse(strs[i], strs[i+1])) return _points.RemoveAll(), FALSE;
				_points.Add(pt);
			}
			_path_type = pt_polygon;
			return TRUE;
		}

		else if (len>=1 && CStringW(L"ellipse").Find(strs[0])>=0) // [e]llipse
		{
			RelativePoint topleft, rightbottom;
			rightbottom.xDir = rightbottom.yDir = TRUE;

			if (num>=2 && !topleft.Parse(strs[1], num>=3?strs[2]:L"0")) return FALSE;
			if (num>=4 && !rightbottom.Parse(strs[3], num>=5?strs[4]:L"#0")) return FALSE;

			_points.Add(topleft);
			_points.Add(rightbottom);
			_path_type = pt_ellipse;
			return TRUE;
		}

		else if (len>=2 && num>=5 && CStringW(L"chord").Find(strs[0])>=0) // [ch]ord
		{
			RelativePoint pt1, pt2, topleft, rightbottom;
			rightbottom.xDir = rightbottom.yDir = TRUE;

			if (!pt1.Parse(strs[1], strs[2]) || !pt2.Parse(strs[3], strs[4])) return FALSE;
			if (num>=6 && !topleft.Parse(strs[5], num>=7?strs[6]:L"0")) return FALSE;
			if (num>=8 && !rightbottom.Parse(strs[7], num>=9?strs[8]:L"#0")) return FALSE;

			_points.Add(topleft);
			_points.Add(rightbottom);
			_points.Add(pt1);
			_points.Add(pt2);
			_path_type = pt_chord;
			return TRUE;
		}

		else if (len>=2 && num>=5 && CStringW(L"pie").Find(strs[0])>=0) // [pi]e
		{
			RelativePoint pt1, pt2, topleft, rightbottom;
			rightbottom.xDir = rightbottom.yDir = TRUE;

			if (!pt1.Parse(strs[1], strs[2]) || !pt2.Parse(strs[3], strs[4])) return FALSE;
			if (num>=6 && !topleft.Parse(strs[5], num>=7?strs[6]:L"0")) return FALSE;
			if (num>=8 && !rightbottom.Parse(strs[7], num>=9?strs[8]:L"#0")) return FALSE;

			_points.Add(topleft);
			_points.Add(rightbottom);
			_points.Add(pt1);
			_points.Add(pt2);
			_path_type = pt_pie;
			return TRUE;
		}

		// polydraw
		for (int i=0; i<num; i++)
		{
			BYTE type;
			RelativePoint pt, pt2, pt3;
			strs[i].MakeLower();
			if (CStringW(L"moveto").Find(strs[i])>=0 && i+2<num && pt.Parse(strs[i+1], strs[i+2]))
			{
				i += 2;
				type = PT_MOVETO;
				_points.Add(pt);
				_types.Add(type);
			}
			else if (CStringW(L"lineto").Find(strs[i])>=0 && i+2<num && pt.Parse(strs[i+1], strs[i+2]))
			{
				i += 2;
				type = PT_LINETO;
				_points.Add(pt);
				_types.Add(type);
			}
			else if (CStringW(L"bezierto").Find(strs[i])>=0 && i+6<num &&
				pt.Parse(strs[i+1], strs[i+2]) && pt2.Parse(strs[i+3], strs[i+4]) && pt3.Parse(strs[i+5], strs[i+6]))
			{
				i += 6;
				type = PT_BEZIERTO;
				_types.Add(type);
				_types.Add(type);
				_types.Add(type);
				_points.Add(pt);
				_points.Add(pt2);
				_points.Add(pt3);
			}
			else if (CStringW(L"close").Find(strs[i])>=0 || strs[i]==L"end")
			{
				int nt = _types.GetSize();
				if (nt>0 && (_types[nt-1]==PT_LINETO || _types[nt-1]==PT_BEZIERTO))
					_types[_types.GetSize()-1] |= PT_CLOSEFIGURE;
			}
			else
			{
				_points.RemoveAll();
				_types.RemoveAll();
				return FALSE;
			}
		}
		_path_type = pt_polydraw;

		// 确保最后一个画操作是关闭的
		for (int i=_types.GetSize()-1; i>=0; i--)
		{
			BYTE b = _types[i] & ~PT_CLOSEFIGURE;
			if (b==PT_LINETO || b==PT_BEZIERTO)
			{
				_types[i] |= PT_CLOSEFIGURE;
				break;
			}
		}
	}
	return TRUE;
}

BOOL GdiPath::Draw( HDC hdc, RECT rc )
{
	int n = _points.GetSize();
	if (hdc==NULL || n==0) return FALSE;

	switch (_path_type)
	{
	case pt_rectangle: return Draw_Rectangle(hdc, rc);
	case pt_roundrect: return Draw_RoundRect(hdc, rc);
	case pt_polygon: return Draw_Polygon(hdc, rc);
	case pt_ellipse: return Draw_Ellipse(hdc, rc);
	case pt_chord: return Draw_Chord(hdc, rc);
	case pt_pie: return Draw_Pie(hdc, rc);
	case pt_polydraw: return Draw_Custom(hdc, rc);
	}
	return FALSE;
}

BOOL GdiPath::Draw_Custom( HDC hdc, RECT rc )
{
	int n = _points.GetSize();

	POINT* ppt = new POINT[n];
	if (ppt==NULL) return FALSE;

	for (int i=0; i<n; i++)
	{
		ppt[i].x = _points[i].GetX(rc);
		ppt[i].y = _points[i].GetY(rc);
	}

	BOOL b = ::PolyDraw(hdc, ppt, _types.GetData(), n);
	//b = ::FillPath(hdc);

	delete[] ppt;
	return b;
}

BOOL GdiPath::Draw_Rectangle( HDC hdc, RECT rc )
{
	return ::Rectangle(hdc, _points[0].GetX(rc), _points[0].GetY(rc), _points[1].GetX(rc), _points[1].GetY(rc));
}

BOOL GdiPath::Draw_RoundRect( HDC hdc, RECT rc )
{
	return ::RoundRect(hdc, rc.left, rc.top, rc.right, rc.bottom, _points[0].GetX(rc), _points[0].GetY(rc));
}

BOOL GdiPath::Draw_Polygon( HDC hdc, RECT rc )
{
	int n = _points.GetSize();

	POINT* ppt = new POINT[n];
	if (ppt==NULL) return FALSE;

	for (int i=0; i<n; i++)
	{
		ppt[i].x = _points[i].GetX(rc);
		ppt[i].y = _points[i].GetY(rc);
	}

	BOOL b = ::Polygon(hdc, ppt, n);

	delete[] ppt;
	return b;
}

BOOL GdiPath::Draw_Ellipse( HDC hdc, RECT rc )
{
	return ::Ellipse(hdc, _points[0].GetX(rc), _points[0].GetY(rc), _points[1].GetX(rc), _points[1].GetY(rc));
}

BOOL GdiPath::Draw_Chord( HDC hdc, RECT rc )
{
	return ::Chord(hdc, _points[0].GetX(rc), _points[0].GetY(rc), _points[1].GetX(rc), _points[1].GetY(rc),
						_points[2].GetX(rc), _points[2].GetY(rc), _points[3].GetX(rc), _points[3].GetY(rc));
}

BOOL GdiPath::Draw_Pie( HDC hdc, RECT rc )
{
	return ::Pie(hdc,	_points[0].GetX(rc), _points[0].GetY(rc), _points[1].GetX(rc), _points[1].GetY(rc),
						_points[2].GetX(rc), _points[2].GetY(rc), _points[3].GetX(rc), _points[3].GetY(rc));
}

//////////////////////////////////////////////////////////////////////////

Device* Device::New( HWND hwnd )
{
	iDevice* dev = NULL;
	if (d3d::IsValid(true))
	{
		dev = NEW iDevice(hwnd);
		if (dev && !dev->isValid)
		{
			delete dev;
			dev = NULL;
		}
	}
	return dev;
}

void Device::Dispose()
{
	delete (iDevice*)this;
}

void Device::Render()
{
	iDevice* dev = (iDevice*)this;
	if (dev->isValid)
		dev->render();
}

void Device::Reset()
{
	iDevice* dev = (iDevice*)this;
	dev->reset();
}

HDC Device::GetDC()
{
	iDevice* pThis = (iDevice*)this;
	if (pThis->m_toptexture == NULL)
	{
		CRect rc;
		::GetClientRect(pThis->m_hwnd, &rc);
		pThis->m_toptexture = NEW iQuadObject(pThis, rc.left, rc.right, rc.Width(), rc.Height(), NULL);
		if (pThis->m_toptexture==NULL || !pThis->m_toptexture->isValid)
			return NULL;
	}
	return pThis->m_toptexture->m_gdi.getDC();
}

void Device::ReleaseDC( HDC hdc )
{
	iDevice* pThis = (iDevice*)this;
	if (pThis->m_toptexture)
		pThis->m_toptexture->m_gdi.releaseDC(hdc);
}

BOOL Device::CreateCaret( HBITMAP hBitmap, int nWidth, int nHeight, GdiLayer* pOwner )
{
	iDevice* pThis = (iDevice*)this;
	//if (!pThis->isLayered) return ::CreateCaret(pThis->m_hwnd, hBitmap, nWidth, nHeight);

	if (nWidth<=0) nWidth = ::GetSystemMetrics(SM_CXBORDER);
	if (nHeight<=0) nHeight = ::GetSystemMetrics(SM_CYBORDER);
	pThis->m_caret.owner = (iQuadObject*)pOwner;
	pThis->m_caret.cx = nWidth;
	pThis->m_caret.cy = nHeight;
	pThis->m_caret.bValid = TRUE;
	pThis->invalidate();
	return TRUE;
}

BOOL Device::DestroyCaret()
{
	iDevice* pThis = (iDevice*)this;
	//if (!pThis->isLayered) return ::DestroyCaret();

	ShowCaret(FALSE);
	pThis->m_caret.bValid = FALSE;
	pThis->invalidate();
	return TRUE;
}

const UINT BlindID = 2010;
BOOL Device::ShowCaret( BOOL bShow )
{
	iDevice* pThis = (iDevice*)this;
	//if (!pThis->isLayered) return bShow ? ::ShowCaret(pThis->m_hwnd) : ::HideCaret(pThis->m_hwnd);

	if (bShow)
	{
		pThis->m_caret.bShow = TRUE;
		pThis->m_caret.bShowing = TRUE;
		::SetTimer(pThis->m_hwnd, BlindID, GetCaretBlinkTime(), NULL);
	}
	else
	{
		pThis->m_caret.bShow = FALSE;
		pThis->m_caret.bShowing = FALSE;
		::KillTimer(pThis->m_hwnd, BlindID);
	}
	pThis->invalidate();
	return TRUE;
}

BOOL Device::SetCaretPos( int X, int Y )
{
	iDevice* pThis = (iDevice*)this;
	//if (!pThis->isLayered) return ::SetCaretPos(X,Y);

	pThis->m_caret.x = X;
	pThis->m_caret.y = Y;
	pThis->invalidate();
	return TRUE;
}

BOOL Device::IsBlindTimer(UINT nID)
{
	iDevice* pThis = (iDevice*)this;
	if (/*!pThis->isLayered ||*/ nID!=BlindID) return FALSE;

	pThis->m_caret.bShowing = !pThis->m_caret.bShowing;
	pThis->invalidate();
	return TRUE;
}

BOOL Device::IsLayeredWindow()
{
	iDevice* pThis = (iDevice*)this;
	return pThis->isLayered;
}

void Device::AddChild( HWND hWnd )
{
	iDevice* pThis = (iDevice*)this;
	if (!::IsChild(pThis->m_hwnd, hWnd))
	//if (::GetParent(hWnd) != pThis->m_hwnd)
		return;

	iControl* c = NULL;
	if (pThis->m_controls)
		c = pThis->m_controls->find(hWnd);

	if (c==NULL)
	{
		c = NEW iControl(pThis, hWnd);
	}
}

void Device::RemoveChild( HWND hWnd )
{
	iDevice* pThis = (iDevice*)this;
	if (!::IsChild(pThis->m_hwnd, hWnd))
	//if (::GetParent(hWnd) != pThis->m_hwnd)
		return;

	iControl* c = (iControl*)::GetPropW(hWnd, AtomString(atom_child_window));
	if (c) delete c;
}

HWND Device::Handle()
{
	iDevice* pThis = (iDevice*)this;
	return pThis->m_hwnd;
}

void Device::ResizeChild( HWND hWnd )
{
	iDevice* pThis = (iDevice*)this;
	if (!::IsChild(pThis->m_hwnd, hWnd))
	//if (::GetParent(hWnd) != pThis->m_hwnd)
		return;

	iControl* c = (iControl*)::GetPropW(hWnd, AtomString(atom_child_window));
	if (c)
	{
		c->resize();
		pThis->invalidate();
	}
}

HWND Device::GetGhostWindow()
{
	if (!::IsWindow(gt.hwndGhost))
	{
		WNDCLASSW wc;
		memset(&wc, 0, sizeof(wc));
		wc.lpfnWndProc = (WNDPROC)DefWindowProc;
		wc.lpszClassName = L"redui_ghost";
		ATOM atom = RegisterClassW(&wc);
		gt.hwndGhost = ::CreateWindowExW(0, MAKEINTRESOURCEW(atom), L"", WS_VISIBLE,0,0,100,100, NULL/*HWND_MESSAGE*/, 0,NULL,0);
	}
	return gt.hwndGhost;
}

void Device::Relayout()
{
	((iDevice*)this)->relayout();
}

void Device::SetWindowObject( IDispatch* disp )
{
	iDevice* pThis = (iDevice*)this;
	pThis->m_window_object = disp;
}

BOOL WINAPI CalcScriptExpressionW(HWND hwndDirectUI, LPCWSTR lpszExpression, LPVARIANT lpVarResult);

void Device::SetScriptObject( HANDLE handle )
{
	iDevice* pThis = (iDevice*)this;
	pThis->m_hscript = handle;
#if 0
	//CComVariant vv;
	//CalcScriptExpressionW(pThis->m_hwnd, L"alert('hello')", &vv);
	HRESULT hr;
 	CScript script(handle);
 	CComPtr<IDispatch> disp;
 	hr = script.ParseProcedure(L"alert('hello')", &disp/*, TRUE*/);
	if (disp)
	{
		CComVariant v = GetDispatch();
		DISPID didThis = DISPID_THIS;
		DISPPARAMS dp = {&v, &didThis, 1, 1};
		//DISPPARAMS dp = {NULL, NULL, 0, 0};
		CComVariant vRet;
		hr = disp->Invoke(0, IID_NULL, LOCALE_SYSTEM_DEFAULT, DISPATCH_METHOD, &dp, &vRet, NULL, NULL);
	}
#endif
}

HRESULT Device::GetStoryBoards(VARIANT* val)
{
	iDevice* pThis = (iDevice*)this;
	CDispatchArray* pArr = CDispatchArray::New(val);
	if (pArr==NULL) return E_OUTOFMEMORY;

	iStoryBoard* sb = pThis->m_stories;
	while (sb)
	{
		pArr->Add(sb->GetDispatch(), sb->Name);
		sb = sb->Next();
	}
	return S_OK;
}

HRESULT Device::PutNewStoryBoard( VARIANT* val )
{
	if (val==NULL || val->vt!=VT_DISPATCH || val->pdispVal==NULL) return E_INVALIDARG;
	StoryBoard* sb = DISP_CAST(val->pdispVal, StoryBoard);
	if (sb)
		return sb->AddTo(GetDispatch()), S_OK;
	return E_INVALIDARG;
}

BOOL Device::IsChildNode( HANDLE pNode, IDispatch** ppDisp/*=NULL*/ )
{
	if (pNode==NULL) return FALSE;
	iDevice* pThis = (iDevice*)this;
	CMarkupNode* n = (CMarkupNode*)pNode;
	LPCOLESTR name = n->GetName();
	
	// 3D 特效代码（HLSL)，可有src/id属性，如果src和正文都存在，最终将合并成一个特效文件
	if (lstrcmpiW(name, L"Effect") == 0)
	{
		LPCOLESTR ename = n->GetAttributeValue(L"name", L"id");
		LPCOLESTR url = n->GetAttributeValue(L"src");
		CStdString tempid;
		if (ename==NULL && url && *url!=0)
		{
			LPCOLESTR p = url + lstrlenW(url);
			while (p>=url && *p!=L'/' && *p!=L'\\') p--;
			CStdString str = p+1;
			int nStart = 0;
			tempid = str.Tokenize(L".\0", nStart);
			ename = tempid;
		}

		LPCOLESTR data = NULL;
		CMarkupNode cont_node = n->GetChild(FALSE);
		while (cont_node.IsValid() && !cont_node.IsTextBlock()) cont_node = cont_node.GetSibling(FALSE);
		if (cont_node.IsTextBlock())
		{
			data = cont_node.GetValue();
		}
		Effect* e = Effect::New(this, url, ename, data);
		//if (ppResult) *ppResult = e;
	}

	// 3D 模型资源
	else if (lstrcmpiW(name, L"Model") == 0)
	{
		iModelBase::createModelByNode(n, pThis, NULL, ppDisp);
	}

	// 3D 场景资源，里面定义了各种模型等纯 3D 对象
	else if (lstrcmpiW(name, L"Scene") == 0)
	{
		DeviceBuildResource(Scene)
	}

	// 3D 动画资源
	else if (lstrcmpiW(name, L"StoryBoard") == 0)
	{
		DeviceBuildResource(StoryBoard)
		//StoryBoard* sb = StoryBoard::New(this);
		//if (sb==NULL) return TRUE;
		//sb->ParseAttribute(pNode);

		//for (CMarkupNode child=n->GetChild(); child.IsValid(); child=child.GetSibling())
		//	sb->IsChildNode(&child);
	}

	// unknown tagname
	else
		return FALSE;

	return TRUE;
}

Scene* Device::FindScene( LPCOLESTR nameorindex )
{
	iDevice* device = (iDevice*)this;
	if (device->m_scenes==NULL) return NULL;

	if (HIWORD(nameorindex)==0) // index
	{
		return (Scene*)device->m_scenes->getAt((DWORD)LOWORD(nameorindex));
	}

	iScene* s = device->m_scenes;
	while (s && lstrcmpiW(s->Name, nameorindex)!=0) s = s->Next();
	return s;
}

BOOL Device::FindNamedObject(LPCOLESTR name, DWORD findType/* =FRT_ALL */, IDispatch** ppDisp/* =NULL */)
{
	iDevice* device = (iDevice*)this;

	bool findEffect = (findType==FRT_ALL || (findType & FRT_EFFECT));
	bool findStory = (findType==FRT_ALL || (findType & FRT_STORY));
	bool findScene = (findType==FRT_ALL || (findType & FRT_SCENE));
	bool findModel = (findType==FRT_ALL || (findType & FRT_MODEL));
	bool findEntity = (findType==FRT_ALL || (findType & FRT_ENTITY));
	// 暂不支持 iImageResource，因为暂时没有支持自动化
	//if (findType==FRT_ALL || (findType & FRT_IMAGE))
	//{
	//	iImageResource* img = device->m_images;
	//	while (img)
	//	{
	//		img = img->Next();
	//	}
	//}

	if (findEffect)
	{
		iEffect* e = iEffect::findTechnique(device->m_effects, name);
		if (e)
		{
			if (ppDisp)
			{
				*ppDisp = e->GetDispatch();
				e->GetDispatch()->AddRef();
			}
			return TRUE;
		}
	}

	if (findStory)
	{
		iStoryBoard* story = device->m_stories;
		while (story)
		{
			if (lstrcmpiW(story->Name, name) == 0)
			{
				if (ppDisp)
				{
					*ppDisp = story->GetDispatch();
					story->GetDispatch()->AddRef();
				}
				return TRUE;
			}
			story = story->Next();
		}
	}

	if (findScene)
	{
		iScene* scene = device->m_scenes;
		while (scene)
		{
			if (lstrcmpiW(scene->Name, name) == 0)
			{
				if (ppDisp)
				{
					*ppDisp = scene->GetDispatch();
					scene->GetDispatch()->AddRef();
				}
				return TRUE;
			}
			scene = scene->Next();
		}
	}

	if (findModel || findEntity)
	{
		iModelBase* m = device->m_models;
		while (m)
		{
			// 所有实体对象一定是从根模型中创建的，且实体都保存在根模型中
			if (findEntity)
			{
				iEntity* e = m->m_entities;
				while (e)
				{
					struct findEntitySelector
					{
						iEntity* ret;
						LPCOLESTR name;

						static BOOL proc(iEntity* pEntity, LPVOID data)
						{
							findEntitySelector* s = (findEntitySelector*)data;
							if (lstrcmpiW(pEntity->m_Name, s->name) == 0)
							{
								s->ret = pEntity;
								return FALSE;
							}
							return TRUE;
						}
					};

					findEntitySelector selector = {NULL, name};
					e->enumAllEntities(&findEntitySelector::proc, &selector);
					if (selector.ret)
					{
						if (ppDisp)
						{
							*ppDisp = selector.ret->GetDispatch();
							selector.ret->GetDispatch()->AddRef();
						}
						return TRUE;
					}

					e = e->TList<iEntity>::Next();
				}
			}

			if (findModel)
			{
				struct findSelector
				{
					iModelBase* retModel;
					LPCOLESTR name;

					static BOOL proc(iModelBase* pModel, LPVOID data)
					{
						findSelector* s = (findSelector*)data;
						if (lstrcmpiW(pModel->m_name, s->name) == 0)
						{
							s->retModel = pModel;
							return FALSE;
						}
						return TRUE;
					}
				};

				findSelector selector = {NULL, name};
				m->enumAllModels(&findSelector::proc, &selector);
				if (selector.retModel)
				{
					if (ppDisp)
					{
						*ppDisp = selector.retModel->getDispatch();
						selector.retModel->getDispatch()->AddRef();
					}
					return TRUE;
				}
			}

			m = m->TList<iModelBase>::Next();
		}
	}

	return FALSE;
}

HRESULT Device::GetCurrentScene( VARIANT* val )
{
	iDevice* device = (iDevice*)this;
	CComVariant v;
	if (device->m_runtime.get_scene())
		v = device->m_runtime.get_scene()->GetDispatch();
	return v.Detach(val);
}

HRESULT Device::GetCurrentEntity( VARIANT* val )
{
	iDevice* device = (iDevice*)this;
	CComVariant v;
	if (device->m_runtime.get_entity())
		v = device->m_runtime.get_entity()->GetDispatch();
	return v.Detach(val);
}

HRESULT Device::GetCurrentModel( VARIANT* val )
{
	iDevice* device = (iDevice*)this;
	CComVariant v;
	if (device->m_runtime.get_model())
		v = device->m_runtime.get_model()->getDispatch();
	return v.Detach(val);
}

HRESULT Device::GetCurrentMaterial( VARIANT* val )
{
	iDevice* device = (iDevice*)this;
	CComVariant v;
	if (device->m_runtime.get_material())
		v = device->m_runtime.get_material()->GetDispatch();
	return v.Detach(val);
}

HRESULT Device::GetCurrentEffect( VARIANT* val )
{
	iDevice* device = (iDevice*)this;
	CComVariant v;
	if (device->m_runtime.get_effect())
		v = device->m_runtime.get_effect()->GetDispatch();
	return v.Detach(val);
}

HRESULT Device::GetCurrentTextureAspectRatio( VARIANT* val )
{
	iDevice* device = (iDevice*)this;
	float ar = 1.0f;
	CComPtr<IDirect3DTexture9> tex;
	//IDirect3DBaseTexture9* bt = device->m_runtime.get_texture();
	//if (bt)
	//{
	//	tex = bt;
	//}
	//else 
	if (device->m_runtime.scene)
		tex = device->m_runtime.scene->find_texture(NULL/*, true*/);
	if (tex.p)
	{
		D3DSURFACE_DESC sd;
		tex->GetLevelDesc(0, &sd);
		ar = (float)sd.Width / (float)sd.Height;
	}
	CComVariant v = ar;
	return v.Detach(val);
}

//void Device::RegisterNamedObject( LPCOLESTR name, IDispatch* disp )
//{
//	iDevice* pThis = (iDevice*)this;
//	iNamedObject* no = pThis->findNamedObject(name);
//	if (no) no->disp = disp;
//	else
//	{
//		no = NEW iNamedObject;
//		if (no)
//		{
//			no->name = name;
//			no->disp = disp;
//			no->addToList(&pThis->m_namedObjects);
//		}
//	}
//}
//
//void Device::UnRegisterNamedObject( IDispatch* disp )
//{
//	if (disp==NULL) return;
//	iDevice* pThis = (iDevice*)this;
//	iNamedObject* no = pThis->m_namedObjects;
//	while (no)
//	{
//		if (no->disp.p == disp)
//		{
//			no->removeFromList();
//			no->Dispose();
//			return;
//		}
//		no = no->Next();
//	}
//}

//////////////////////////////////////////////////////////////////////////
//RenderTarget* RenderTarget::New( Device* device, LONG width, LONG height )
//{
//	if (device==NULL || width<=0 || height<=0)
//		return NULL;
//
//	iRenderTarget* target = NEW iRenderTarget((iDevice*)device, width, height);
//	if (target && !target->isValid)
//	{
//		target = (delete target, NULL);
//	}
//	return target;
//}
//
//void RenderTarget::Dispose()
//{
//	delete (iRenderTarget*)this;
//}
//void RenderTarget::setViewport( ULONG x, ULONG y, ULONG w, ULONG h )
//{
//	iRenderTarget* pThis = (iRenderTarget*)this;
//	pThis->m_viewport = &pThis->_customViewport;
//	pThis->m_viewport->X = x;
//	pThis->m_viewport->Y = y;
//	pThis->m_viewport->Width = w;
//	pThis->m_viewport->Height = h;
//	pThis->m_viewport->MinZ = 0.0;
//	pThis->m_viewport->MaxZ = 1.0;
//}
//
//void RenderTarget::setViewport( const RECT& rc )
//{
//	iRenderTarget* pThis = (iRenderTarget*)this;
//	pThis->m_viewport = &pThis->_customViewport;
//	pThis->m_viewport->X = rc.left;
//	pThis->m_viewport->Y = rc.top;
//	pThis->m_viewport->Width = rc.right - rc.left;
//	pThis->m_viewport->Height = rc.bottom - rc.top;
//	pThis->m_viewport->MinZ = 0.0;
//	pThis->m_viewport->MaxZ = 1.0;
//}
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
GdiLayer* GdiLayer::New( Device* device, LONG x, LONG y, LONG width, LONG height, GdiLayer* parent/*=NULL*/ )
{
	if (device==NULL || width<=0 || height<=0)
		return NULL;

	iQuadObject* tex = NEW iQuadObject((iDevice*)device, x, y, width, height, (iQuadObject*)parent);
	if (tex && !tex->isValid)
	{
		tex = (delete tex, NULL);
	}
	return tex;
}

void GdiLayer::Dispose()
{
	//GetDispatch()->Release();
	delete (iQuadObject*)this;
}

bool GdiLayer::Parent( GdiLayer* p, BOOL bOrder/*=false*/ )
{
	iQuadObject* pp = (iQuadObject*)p;
	iQuadObject* pThis = (iQuadObject*)this;
	if (this==p || (pp && pp->isParent(pThis)))  // prevent endless loop
		return false;
	//if (pThis->m_parent == pp && pThis->isHead(bOrder?:)) return true;
	////pThis->removeFromList(/*pThis->m_parent ? pThis->m_parent->m_children : pThis->m_device->m_textures*/);
	//pThis->m_parent = pp;
	iQuadObject** refqo = NULL;
	if (pp)
	{
		if (bOrder) refqo = &pp->m_ordered_children;
		else refqo = &pp->m_children;
	}
	else
	{
		if (bOrder) refqo = &pThis->m_device->m_ordered_textures;
		else refqo = &pThis->m_device->m_textures;
	}
	if (pThis->TOrderList<iQuadObject>::isHead(refqo)) return true;
	pThis->m_parent = pp;
	pThis->TOrderList<iQuadObject>::addToList(refqo);
	return true;
}

GdiLayer* GdiLayer::Parent()
{
	iQuadObject* pThis = (iQuadObject*)this;
	return pThis->m_parent;
}

void GdiLayer::ZIndex(long z)
{
	iQuadObject* pThis = (iQuadObject*)this;
	if (pThis->m_zindex == z) return;
	pThis->m_zindex = z;
	pThis->TOrderList<iQuadObject>::reorder();
	//pThis->TOrderList<iQuadObject>::removeFromList();
	//pThis->TOrderList<iQuadObject>::addToList(pThis->m_parent ? &pThis->m_parent->m_children : &pThis->m_device->m_textures);
}

void GdiLayer::Resize( LONG width, LONG height )
{
	iQuadObject* pThis = (iQuadObject*)this;
	pThis->resize(width, height);
}

HDC GdiLayer::GetDC()
{
	iQuadObject* pThis = (iQuadObject*)this;
	return pThis->m_gdi.m_alpha ? pThis->m_gdi.getDC() : pThis->m_gdi.getTexDC();
}

void GdiLayer::ReleaseDC( HDC hdc )
{
	iQuadObject* pThis = (iQuadObject*)this;
	pThis->m_gdi.m_alpha ? pThis->m_gdi.releaseDC(hdc) : pThis->m_gdi.releaseTexDC(hdc);
}

BOOL GdiLayer::State( ObjectStateType ost )
{
	iQuadObject* pThis = (iQuadObject*)this;
	return pThis->states & ostate(ost);
}

void GdiLayer::State( ObjectStateType ost, BOOL value )
{
	iQuadObject* pThis = (iQuadObject*)this;
	if (value) pThis->states |= ostate(ost);
	else pThis->states &= ~ostate(ost);
}

void GdiLayer::SetPaintCallback( PaintCallback* cbPaint )
{
	iQuadObject* pThis = (iQuadObject*)this;
	pThis->_cbpaint = cbPaint;
}

ImageLayer* GdiLayer::Background()
{
	iQuadObject* pThis = (iQuadObject*)this;
	if (pThis->m_background == NULL)
		pThis->m_background = NEW iImageLayer(*pThis);
	return pThis->m_background;
}

ImageLayer* GdiLayer::Foreground()
{
	iQuadObject* pThis = (iQuadObject*)this;
	if (pThis->m_foregrounds == NULL)
		pThis->m_foregrounds = NEW iImageLayer(*pThis);
	return pThis->m_foregrounds;
	//iImageLayer* p = NEW iImageLayer(*pThis);
	//if (p)
	//	p->addToList(&pThis->m_foregrounds);
	//return p;
}

void GdiLayer::RemoveForeground( ImageLayer* il )
{
	if (il==NULL) return;
	iQuadObject* pThis = (iQuadObject*)this;
	iImageLayer* pil = (iImageLayer*)il;
	pil->removeFromList(/*&pThis->m_foregrounds*/);
}

void GdiLayer::Move( LONG left, LONG top, LONG right, LONG bottom )
{
	iQuadObject* pThis = (iQuadObject*)this;
	pThis->move(left, top, right-left, bottom-top);
}

void GdiLayer::Move( LPCRECT prc )
{
	if (prc==NULL) return;
	iQuadObject* pThis = (iQuadObject*)this;
	pThis->move(prc->left, prc->top, prc->right-prc->left, prc->bottom-prc->top);
}

void GdiLayer::Clip(GdiPath* path)
{
	iQuadObject* pThis = (iQuadObject*)this;
	if (pThis->m_clip.m_path == path) return;
	pThis->m_clip.m_texture = NULL;
	pThis->m_clip.m_path = path;
	pThis->m_clip.m_dirty = TRUE;
	//pThis->m_clip.update_path();
	if (path && path->IsValid())
		pThis->isTarget = TRUE;
		//pThis->set_as_target(TRUE);
}

void GdiLayer::ClipContent( LPCRECT prcClip )
{
	if (prcClip==NULL) return;
	iQuadObject* pThis = (iQuadObject*)this;
	::CopyRect(&pThis->m_clipChildren, prcClip);
}

void GdiLayer::Invalidate(BOOL bDrawNow/*=FALSE*/)
{
	iQuadObject* pThis = (iQuadObject*)this;
	pThis->m_gdi.m_dirty = TRUE;
	//pThis->m_device->invalidate();
	if (bDrawNow)
	{
		pThis->m_gdi.m_dirty = FALSE;
		pThis->m_device->render();
	}
	else pThis->m_device->invalidate();
}

Device* GdiLayer::GetDevice() const
{
	iQuadObject* pThis = (iQuadObject*)this;
	return pThis->m_device;
}

void GdiLayer::SetAsTarget( BOOL bTarget/*=FALSE*/ )
{
	iQuadObject* pThis = (iQuadObject*)this;
	pThis->set_as_target(bTarget);
}

void GdiLayer::SetScene( Scene* scene )
{
	iQuadObject* pThis = (iQuadObject*)this;
	pThis->set_scene((iScene*)scene);
}

void GdiLayer::SetName( LPCOLESTR name )
{
	iQuadObject* pThis = (iQuadObject*)this;
	pThis->m_name = name;
}

void GdiLayer::BindWindow(HWND hwnd, IPropertyBag* pb/*=NULL*/)
{
	iQuadObject* pThis = (iQuadObject*)this;
	BOOL bTransparent = FALSE;
	LPCOLESTR anotherWinClassName=NULL;
	long lRenderType=0;
	LPCOLESTR msgs = NULL;

	if (pb)
	{
		OutputDebugStringW(L"=======开始读取窗口绑定属性：\n");
		CComVariant v;
		pb->Read(L"transparent", &v, NULL);
		if (v.vt == VT_BSTR)
			bTransparent = ::ParseBoolString(v.bstrVal);

		v.ClearToZero();
		pb->Read(L"bind-class", &v, NULL);
		if (v.vt == VT_BSTR)
			anotherWinClassName = v.bstrVal;

		v.ClearToZero();
		pb->Read(L"bind-type", &v, NULL);
		if (v.vt!=VT_EMPTY && SUCCEEDED(v.ChangeType(VT_I4)))
			lRenderType = v.lVal;

		v.ClearToZero();
		pb->Read(L"bind-messages", &v, NULL);
		if (v.vt == VT_BSTR)
			msgs = v.bstrVal;
		//if (v.vt!=VT_EMPTY && SUCCEEDED(v.ChangeType(VT_UI4)))
		//	msgHook = v.ulVal;
		OutputDebugStringW(L"=======结束读取窗口绑定属性\n");
	}

	pThis->m_gdi.bindWindow(hwnd, bTransparent);
	pThis->m_gdi.m_changeWinClassName = anotherWinClassName;
	pThis->m_gdi.m_renderType = lRenderType;
	pThis->m_gdi.m_msgHooks.ParseMessagesFromString(msgs);
}

IExtensionContainer* GdiLayer::GetExtensionContainer()
{
	iQuadObject* pThis = (iQuadObject*)this;
	return pThis->m_exts;
}

void GdiLayer::SetExtensionContainer( IExtensionContainer* pExtContainer )
{
	iQuadObject* pThis = (iQuadObject*)this;
	pThis->m_exts = pExtContainer;
}

//////////////////////////////////////////////////////////////////////////
ImageResource* ImageResource::New( Device* device, LPCOLESTR url, LPCOLESTR name/*=NULL*/ )
{
	iImageResource* ir = NEW iImageResource((iDevice*)device, url, name);
	return ir;
}

//void ImageResource::Dispose()
//{
//	delete (iImageResource*)this;
//}

bool ImageResource::IsLoaded()
{
	iImageResource* pThis = (iImageResource*)this;
	return pThis->isLoaded;
}

long ImageResource::GetWidth()
{
	iImageResource* pThis = (iImageResource*)this;
	return pThis->m_width;
}

long ImageResource::GetHeight()
{
	iImageResource* pThis = (iImageResource*)this;
	return pThis->m_height;
}

void ImageResource::SetClipRect( LPCRECT pClip/*=NULL*/ )
{
	iImageResource* pThis = (iImageResource*)this;
	if (pClip)
	{
		::CopyRect(&pThis->_clip, pClip);
		pThis->m_clip = &pThis->_clip;
	}
	else
		pThis->m_clip = NULL;
}

//////////////////////////////////////////////////////////////////////////

#define GetAValue(clr) ((BYTE)((clr)>>24))
#define COLORREF_TO_D3DCOLOR(clr) (D3DCOLOR_ARGB(GetAValue(clr), GetRValue(clr), GetGValue(clr), GetBValue(clr)))

void ImageLayer::SetColor( COLORREF clrTopLeft, COLORREF clrTopRight/*=0*/, COLORREF clrBottomRight/*=0*/, COLORREF clrBottomLeft/*=0*/ )
{
	iImageLayer* pThis = (iImageLayer*)this;
	pThis->m_owner.iSurfaceBase::setcolor(COLORREF_TO_D3DCOLOR(clrTopLeft),
							COLORREF_TO_D3DCOLOR(clrTopRight),
							COLORREF_TO_D3DCOLOR(clrBottomRight),
							COLORREF_TO_D3DCOLOR(clrBottomLeft));
	pThis->m_isLoaded = false;
	pThis->m_isValid = true;
	if (pThis->m_image)
		gConnector.Disconnect((iImageAnimController*)pThis, pThis->m_image);
	pThis->m_image = NULL;
	pThis->m_timerid = -1;
}

void ImageLayer::SetImage( ImageResource* img, int imageLayoutType/* = ilt_normal*/ )
{
	iImageLayer* pThis = (iImageLayer*)this;
	if (pThis->m_image == (iImageResource*)img && pThis->m_ilt==(ImageLayoutType)imageLayoutType) return;
	pThis->m_image = (iImageResource*)img;
	if (pThis->m_image==NULL || !pThis->m_image->isHead(&pThis->m_owner.m_device->m_images)) return;

	pThis->m_ilt = (ImageLayoutType)imageLayoutType;
	pThis->m_isLoaded = false;
	pThis->m_isValid = true;
	pThis->m_timerid = -1;
	if (pThis->m_image->isLoaded)
		pThis->play();
	else
		gConnector.Connect((iImageAnimController*)pThis, img, cp_img_loaded);
}

void ImageLayer::Clear()
{
	iImageLayer* pThis = (iImageLayer*)this;
	pThis->m_isValid = false;
	if (pThis->m_image)
		gConnector.Disconnect((iImageAnimController*)pThis, pThis->m_image);
	pThis->m_image = NULL;
	pThis->m_isLoaded = false;
	pThis->m_timerid = -1;
}

//////////////////////////////////////////////////////////////////////////
Effect* Effect::New( Device* device, LPCOLESTR url/*=NULL*/, LPCOLESTR name/*=NULL*/, LPCOLESTR data/*=NULL*/ )
{
	iEffect* e = NEW iEffect((iDevice*)device, url, name, data);
	if (e && !e->isValid)
		e = (delete e, NULL);
	return e;
}

//void Effect::Dispose()
//{
//	delete (iEffect*)this;
//}

bool Effect::IsLoaded()
{
	iEffect* pThis = (iEffect*)this;
	return pThis->isLoaded;
}

BOOL Effect::__DispidOfName( LPCOLESTR szName, DISPID* pDispid )
{
	iEffect* e = (iEffect*)this;
	long idx;
	if (/*e->m_current_technique &&*/ szName && e->get_param_handle(szName, &idx))
	{
		iEffect::EffectParam& ep = e->m_params_cache[idx];
		if (ep.desc.Type==D3DXPT_BOOL || ep.desc.Type==D3DXPT_FLOAT || ep.desc.Type==D3DXPT_INT)
		{
			*pDispid = 1000 + idx;
			return TRUE;
		}
	}

	return FALSE;
}

HRESULT Effect::__Invoke( DISPID dispid, WORD wFlags, DISPPARAMS* pdispparams, VARIANT *pVarResult )
{
	iEffect* e = (iEffect*)this;
	if (e->m_current_technique && dispid>=1000 && dispid<1000+e->m_params_cache.GetSize())
	{
		iEffect::EffectParam& ep = e->m_params_cache[dispid-1000];
		D3DXPARAMETER_DESC& desc = ep.desc;
		if (wFlags == DISPATCH_PROPERTYGET)
		{
			CComVariant vRet;
			EffectParamValue* epv = e->get_param_value(ep.handle, true);
			ATLASSERT(epv);
			if (epv)
			{
				switch (desc.Type)
				{
				case D3DXPT_BOOL:
					if (desc.Elements>0)
						return ArrayRef(BOOL)::CreateInstance(epv->pbVal, desc.Elements, pVarResult);
					else
						vRet = (*epv->pbVal != FALSE);
					break;
				case D3DXPT_INT:
					if (desc.Elements>0)
						return ArrayRef(INT)::CreateInstance(epv->piVal, desc.Elements, pVarResult);
					else
						vRet = (long)*epv->piVal;
					break;
				case D3DXPT_FLOAT:
					if (desc.Rows>1)
					{
						if (desc.Elements>0)
							return ArrayRef(D3DMATRIX)::CreateInstance(epv->pmVal, desc.Elements, pVarResult);
						else
							return CreateRefInstanceV(D3DMATRIX, epv->pmVal, pVarResult);
					}
					else if (desc.Columns>1)
					{
						if (desc.Elements>0)
							return ArrayRef(D3DXVECTOR4)::CreateInstance(epv->pvVal, desc.Elements, pVarResult);
						else
							return CreateRefInstanceV(D3DXVECTOR4, epv->pvVal, pVarResult);
					}
					else // rows == columns == 1
					{
						if (desc.Elements>0)
							return ArrayRef(float)::CreateInstance(epv->pfVal, desc.Elements, pVarResult);
						else
							vRet = *epv->pfVal;
					}
					break;
				}
			}
			return vRet.Detach(pVarResult);
		}
		else if (/*(wFlags==DISPATCH_PROPERTYPUT || wFlags==DISPATCH_PROPERTYPUTREF) &&*/ pdispparams->cArgs>=1) // 支持方法式的PUT调用
		{
			// 注意：数组变量的赋值必须使用字符串类型，例如 float2 test; 或 float test[2]; 应该这样赋值：effect.test = "1.0 2.0";
			if (desc.Elements>1 /*&& pdispparams->rgvarg[pdispparams->cArgs-1].vt!=VT_BSTR*/) return E_INVALIDARG;

			EffectParamValue* pValue = e->get_param_value(ep.handle, false, true);
			if (pValue==NULL) return E_OUTOFMEMORY;
			e->clearParamBlock();

			CComVariant v;

			switch (desc.Type)
			{
			case D3DXPT_BOOL:
				if (SUCCEEDED(v.ChangeType(VT_INT, &pdispparams->rgvarg[pdispparams->cArgs-1])))
				{
					*pValue->pbVal = (V_INT(&v) != 0);
				}
				break;
			case D3DXPT_INT:
				if (SUCCEEDED(v.ChangeType(VT_INT, &pdispparams->rgvarg[pdispparams->cArgs-1])))
				{
					*pValue->piVal = V_INT(&v);
				}
				break;
			case D3DXPT_FLOAT:
				if (desc.Rows>1 && SUCCEEDED(v.ChangeType(VT_DISPATCH, &pdispparams->rgvarg[pdispparams->cArgs-1])) && v.pdispVal)
				{
					D3DXMATRIX* m = DISP_CAST(v.pdispVal, D3DXMATRIX);
					if (m==NULL) return E_INVALIDARG;
					*pValue->pmVal = *m;
				}
				else if (desc.Rows==1 && desc.Columns>1 && SUCCEEDED(v.ChangeType(VT_DISPATCH, &pdispparams->rgvarg[pdispparams->cArgs-1])) && v.pdispVal)
				{
					// 对于 4 维数，除 VECTOR4D 之外，还可以用颜色和四元数来赋值
					D3DXVECTOR4* v4 = DISP_CAST(v.pdispVal, D3DXVECTOR4);
					D3DCOLORVALUE* c = NULL;
					D3DXQUATERNION* q = NULL;
					if (v4)
						*pValue->pvVal = *v4;
					else if (c = DISP_CAST(v.pdispVal, D3DCOLORVALUE))
					{
						memcpy_s((float*)*pValue->pvVal, 4*sizeof(float), &c->r, 4*sizeof(float));
					}
					else if (q = DISP_CAST(v.pdispVal, D3DXQUATERNION))
					{
						memcpy_s((float*)*pValue->pvVal, 4*sizeof(float), *q, 4*sizeof(float));
					}

					// 尝试 3 维数
					else if (desc.Columns<=3)
					{
						D3DVECTOR* v3 = DISP_CAST(v.pdispVal, D3DVECTOR);
						if (v3) (pValue->pvVal->x=v3->x), (pValue->pvVal->y=v3->y), (pValue->pvVal->z=v3->z);
						else if (desc.Columns<=2)
						{
							D3DXVECTOR2* v2 = DISP_CAST(v.pdispVal, D3DXVECTOR2);
							if (v2) (pValue->pvVal->x=v2->x), (pValue->pvVal->y=v2->y);
							else return E_INVALIDARG;
						}
						else return E_INVALIDARG;
					}
					else return E_INVALIDARG;
				}
				else if (desc.Rows==1 && desc.Columns==1 && SUCCEEDED(v.ChangeType(VT_R4, &pdispparams->rgvarg[pdispparams->cArgs-1])))
				{
					*pValue->pfVal = V_R4(&v);
				}
				else
					return E_INVALIDARG;
				break;
			default: // 其它类型不处理
				return E_FAIL;
			}

			// 如果是方法式PUT，则直接返回对象自身
			if (wFlags==DISPATCH_METHOD)
			{
				CComVariant vRet = GetDispatch();
				vRet.Detach(pVarResult);
			}
			return S_OK;
		}
	}

	return DISP_E_MEMBERNOTFOUND;
}

//////////////////////////////////////////////////////////////////////////
Scene* Scene::New( Device* device )
{
	if (device==NULL)
		return NULL;

	iScene* s = (iScene*)CreateInstance(); // NEW iScene((iDevice*)device);
	if (s) { s->m_device=(iDevice*)device; s->addToList(&s->m_device->m_scenes); }
	return s;
}

//void Scene::Dispose()
//{
//	delete (iScene*)this;
//}

HRESULT Scene::GetScene( VARIANT* val )
{
	CComVariant v = GetDispatch();
	return v.Detach(val);
}

Camera& Scene::GetCameraRef()
{
	return (Camera&)((iScene*)this)->m_camera;
}

COLOR& Scene::GetMouseLightRef()
{
	return ((iScene*)this)->m_mouse_light;
}

AmbientLight& Scene::GetAmbientLightRef()
{
	return (AmbientLight&)((iScene*)this)->m_lightGroup.m_ambientLight;
}


//HRESULT Scene::GetAmbientLight( VARIANT* ret )
//{
//	iScene* pThis = (iScene*)this;
//	return CreateRefInstanceV(AmbientLight, &pThis->m_lightGroup.m_ambientLight, ret);
//}

HRESULT Scene::GetLights( VARIANT* ret )
{
	iScene* pThis = (iScene*)this;
	CDispatchArray* pArr = CDispatchArray::New(ret, NULL, iLightGroup::OnUpdated, &pThis->m_lightGroup);
	if (pArr==NULL) return E_OUTOFMEMORY;

	for (int i=0; i<8; i++)
	{
		CComPtr<IDispatch> disp;
		Light* light = &pThis->m_lightGroup.m_lights[i];
		switch (light->m_Light.Type)
		{
		case D3DLIGHT_POINT: CreateRefInstance(PointLight, (PointLight*)light, &disp); break;
		case D3DLIGHT_SPOT: CreateRefInstance(SpotLight, (SpotLight*)light, &disp); break;
		case D3DLIGHT_DIRECTIONAL: CreateRefInstance(DirectionLight, (DirectionLight*)light, &disp); break;
		default: disp = light->GetDispatch();
		}
		pArr->Add(disp.p, light->m_Name);
	}
	return S_OK;
}

HRESULT Scene::GetEntities( VARIANT* ret )
{
	iScene* pThis = (iScene*)this;
	CDispatchArray* pArr = CDispatchArray::New(ret);
	if (pArr==NULL) return E_OUTOFMEMORY;

	iEntity* e = pThis->m_entities;
	while (e)
	{
		pArr->Add(e->GetDispatch(), e->m_Name);
		e = e->m_siblings;
	}
	return S_OK;
}

HRESULT Scene::PutNewEntity( VARIANT* val )
{
	// val 中必须传递 IDispatch(实体或者模型)
	if (val==NULL || val->vt!=VT_DISPATCH || val->pdispVal==NULL) return E_INVALIDARG;

	iScene* pThis = (iScene*)this;
	iEntity* e = (iEntity*)DISP_CAST(val->pdispVal, Entity);
	if (e == NULL)
	{
		iModelBase* m = iModelBase::fromObject(val->pdispVal);
		if (m) e = m->createEntity();
	}

	if (e == NULL) return E_INVALIDARG;
	e->setParent(pThis);
	return S_OK;
}

HRESULT Scene::GetActiveEntity(VARIANT* ret)
{
	iScene* pThis = (iScene*)this;
	CComVariant v;
	if (pThis->m_hover_entity)
		v = pThis->m_hover_entity->GetDispatch();
	return v.Detach(ret);
}

Scene* Scene::NewAutoInstance()
{
	return NEW iScene();
}

void Scene::DeleteAutoInstance( Scene* p )
{
	delete (iScene*)p;
}

void Scene::ParseAttribute( HANDLE hNode )
{
	ATLASSERT(hNode);
	iScene* pThis = (iScene*)this;
	CMarkupNode* n = (CMarkupNode*)hNode;
	Name = n->GetAttributeValue(L"name", L"id");

	LightEnabled = !!ParseBoolString(n->GetAttributeValue(L"LightEnabled"), TRUE);
	pThis->m_mouse_light.InitFromString(n->GetAttributeValue(L"MouseLight"));
	pThis->m_mirror = !!ParseBoolString(n->GetAttributeValue(L"Mirror"), FALSE);

	// Usage
	LPCOLESTR p = n->GetAttributeValue(L"Usage", L"As");
	if (p)
	{
		long l;
		if (TryLoad_long_FromString(p, l) && l>=SU_NONE && l<=SU_ALL) Usage = (SceneUsage)l;
		else if (lstrcmpiW(p, L"NONE")==0) Usage = SU_NONE				;
		else if (lstrcmpiW(p, L"BACKGROUND")==0) Usage = SU_BACKGROUND		;
		else if (lstrcmpiW(p, L"PRECONTENT")==0) Usage = SU_PRECONTENT	;
		else if (lstrcmpiW(p, L"CONTENT")==0) Usage = SU_CONTENT			;
		else if (lstrcmpiW(p, L"POSTCONTENT")==0) Usage = SU_POSTCONTENT	;
		else if (lstrcmpiW(p, L"FOREGROUND")==0) Usage = SU_FOREGROUND		;
		else if (lstrcmpiW(p, L"ALL")==0) Usage = SU_ALL				;
	}

	// 环境光
	LPCOLESTR ambient = n->GetAttributeValue(L"AmbientLight");
	if (ambient)
	{
		COLORREF clr;
		if (TryLoad_COLORREF_FromString(ambient, clr))
		{
			pThis->m_lightGroup.m_ambientLight.m_Enabled = true;
			pThis->m_lightGroup.m_ambientLight.m_Color = clr;
		}
	}
}

BOOL Scene::IsChildNode( HANDLE hChild )
{
	if (hChild==NULL) return FALSE;
	iScene* pThis = (iScene*)this;
	CMarkupNode* node = (CMarkupNode*)hChild;
	LPCOLESTR tag = node->GetName();

	// 处理灯光资源
	LPCOLESTR lightType = node->GetAttributeValue(L"type");
	bool isLight = lstrcmpiW(tag, L"light")==0;
	LPCOLESTR lightIndex = node->GetAttributeValue(L"index"); // [0, 7]

	if (lstrcmpiW(tag, L"AmbientLight")==0 || (isLight && lstrcmpiW(lightType, L"ambient")==0))
	{
		pThis->m_lightGroup.m_ambientLight.ParseAttribute(hChild);
	}
	else if (lstrcmpiW(tag, L"PointLight")==0 || (isLight && lstrcmpiW(lightType, L"point")==0))
	{
		long l;
		if (TryLoad_long_FromString(lightIndex, l) && l>=0 && l<=7)
		{
			pThis->m_lightGroup.m_lights[l].m_Light.Type = D3DLIGHT_POINT;
			pThis->m_lightGroup.m_lights[l].ParseAttribute(hChild);
		}
	}
	else if (lstrcmpiW(tag, L"SpotLight")==0 || (isLight && lstrcmpiW(lightType, L"spot")==0))
	{
		long l;
		if (TryLoad_long_FromString(lightIndex, l) && l>=0 && l<=7)
		{
			pThis->m_lightGroup.m_lights[l].m_Light.Type = D3DLIGHT_SPOT;
			pThis->m_lightGroup.m_lights[l].ParseAttribute(hChild);
		}
	}
	else if (lstrcmpiW(tag, L"DirectionLight")==0 || (isLight && lstrcmpiW(lightType, L"direction")==0))
	{
		long l;
		if (TryLoad_long_FromString(lightIndex, l) && l>=0 && l<=7)
		{
			pThis->m_lightGroup.m_lights[l].m_Light.Type = D3DLIGHT_DIRECTIONAL;
			pThis->m_lightGroup.m_lights[l].ParseAttribute(hChild);
		}
	}

	// 处理摄像机
	else if (lstrcmpiW(tag, L"Camera")==0)
	{
		pThis->m_camera.ParseAttribute(hChild);
	}

	// 实体或模型。如果是实体，需指明使用的模型名称。如果是模型，则自动创建一个实体，实体名称将使用模型的名称
	else if (lstrcmpiW(tag, L"Entity")==0 || lstrcmpiW(tag, L"Model")==0)
	{
		iEntity::createEntityByNode(node, pThis);
	}

	else
		return FALSE;

	return TRUE;
}

void Scene::SetMousePos( POINT* ppt )
{
	iScene* pThis = (iScene*)this;
	if (ppt)
	{
		pThis->m_mouse_inside = true;
		pThis->m_ptMouse = *ppt;
	}
	else
		pThis->m_mouse_inside = false;
	pThis->update_state();
}

void Scene::Update()
{
	iScene* pThis = (iScene*)this;
	pThis->update_state();
}
//////////////////////////////////////////////////////////////////////////
void Camera::ParseAttribute( HANDLE hNode )
{
	ATLASSERT(hNode);
	CMarkupNode* n = (CMarkupNode*)hNode;

	TryLoad_floatptr_FromString(n->GetAttributeValue(L"Position", L"pos"), m_Position, 3);
	TryLoad_float_FromString(n->GetAttributeValue(L"Pitch"), m_Pitch);
	TryLoad_float_FromString(n->GetAttributeValue(L"Yaw"), m_Yaw);
	TryLoad_float_FromString(n->GetAttributeValue(L"Roll"), m_Roll);
	TryLoad_float_FromString(n->GetAttributeValue(L"NearPlane"), m_NearPlane);
	TryLoad_float_FromString(n->GetAttributeValue(L"FarPlane"), m_FarPlane);
	TryLoad_float_FromString(n->GetAttributeValue(L"Zoom"), m_Zoom);
	TryLoad_float_FromString(n->GetAttributeValue(L"Fov"), m_Fov);
	m_Ortho = !!ParseBoolString(n->GetAttributeValue(L"Ortho"), FALSE);
}

//////////////////////////////////////////////////////////////////////////
void AmbientLight::ParseAttribute( HANDLE hNode )
{
	ATLASSERT(hNode);
	CMarkupNode* n = (CMarkupNode*)hNode;

	LPCOLESTR enabled = n->GetAttributeValue(L"enabled");
	if (enabled) m_Enabled = !!ParseBoolString(enabled);

	COLORREF clr;
	if (TryLoad_COLORREF_FromString(n->GetAttributeValue(L"color"), clr))
		m_Color = clr;
}



//////////////////////////////////////////////////////////////////////////

void Light::ParseAttribute( HANDLE hNode )
{
	ATLASSERT(hNode);
	CMarkupNode* n = (CMarkupNode*)hNode;

	m_Enabled = !!ParseBoolString(n->GetAttributeValue(L"enabled"), TRUE);

	m_Name = n->GetAttributeValue(L"name", L"id");

	// Diffuse
	TryLoad_D3DCOLORVALUE_FromString(n->GetAttributeValue(L"Diffuse"), m_Light.Diffuse);
	// Specular
	TryLoad_D3DCOLORVALUE_FromString(n->GetAttributeValue(L"Specular"), m_Light.Specular);
	// Ambient
	TryLoad_D3DCOLORVALUE_FromString(n->GetAttributeValue(L"Ambient"), m_Light.Ambient);

	// Position
	if (m_Light.Type==D3DLIGHT_POINT || m_Light.Type==D3DLIGHT_SPOT)
		TryLoad_floatptr_FromString(n->GetAttributeValue(L"Position"), &m_Light.Position.x, 3);

	// Direction
	if (m_Light.Type==D3DLIGHT_DIRECTIONAL || m_Light.Type==D3DLIGHT_SPOT)
		TryLoad_floatptr_FromString(n->GetAttributeValue(L"Direction"), &m_Light.Direction.x, 3);

	// Range and Attenuation
	if (m_Light.Type==D3DLIGHT_POINT || m_Light.Type==D3DLIGHT_SPOT)
	{
		TryLoad_float_FromString(n->GetAttributeValue(L"Range"), m_Light.Range);
		TryLoad_float_FromString(n->GetAttributeValue(L"Attenuation0"), m_Light.Attenuation0);
		TryLoad_float_FromString(n->GetAttributeValue(L"Attenuation1"), m_Light.Attenuation1);
		TryLoad_float_FromString(n->GetAttributeValue(L"Attenuation2"), m_Light.Attenuation2);
	}

	// Falloff, Theta and Phi
	if (m_Light.Type==D3DLIGHT_SPOT)
	{
		TryLoad_float_FromString(n->GetAttributeValue(L"Falloff"), m_Light.Falloff);
		TryLoad_float_FromString(n->GetAttributeValue(L"Theta"), m_Light.Theta);
		TryLoad_float_FromString(n->GetAttributeValue(L"Phi"), m_Light.Phi);
	}
}

//////////////////////////////////////////////////////////////////////////
PointLight* PointLight::NewAutoInstance()
{
	return NEW iPointLight;
}

void PointLight::DeleteAutoInstance( PointLight* p )
{
	delete (iPointLight*)p;
}

//////////////////////////////////////////////////////////////////////////
SpotLight* SpotLight::NewAutoInstance()
{
	return NEW iSpotLight;
}

void SpotLight::DeleteAutoInstance( SpotLight* p )
{
	delete (iSpotLight*)p;
}


//////////////////////////////////////////////////////////////////////////
DirectionLight* DirectionLight::NewAutoInstance()
{
	return NEW iDirectionLight;
}

void DirectionLight::DeleteAutoInstance( DirectionLight* p )
{
	delete (iDirectionLight*)p;
}


//////////////////////////////////////////////////////////////////////////
//HRESULT Material::GetDiffuse( VARIANT* val )
//{
//	return CreateRefInstanceV(D3DCOLORVALUE, (&material.Diffuse), val);
//}

//HRESULT Material::SetDiffuse( VARIANT* val )
//{
//	if (val==NULL || val->vt!=VT_DISPATCH || val->pdispVal==NULL) return DISP_E_TYPEMISMATCH;
//
//	D3DXCOLOR* c = DISP_CAST(val->pdispVal, D3DXCOLOR);
//	if (c==NULL) return DISP_E_TYPEMISMATCH;
//	material.Diffuse = *(D3DCOLORVALUE*)c;
//	return S_OK;
//}


//////////////////////////////////////////////////////////////////////////
IDispatch* StoryBoard::Start()
{
	((iStoryBoard*)this)->start();
	return GetDispatch();
}

IDispatch* StoryBoard::Stop(VARIANT vStopBehavior/*=CComVariant()*/, float stopTime/*=1.0f*/)
{
	int stopBehavior = iStoryBoard::stop_current;
	CComVariant v;
	if (vStopBehavior.vt != VT_EMPTY && SUCCEEDED(v.ChangeType(VT_I4, &vStopBehavior)) && v.lVal>=iStoryBoard::stop_current &&v.lVal<=iStoryBoard::stop_end) stopBehavior = v.lVal;
	if (vStopBehavior.vt == VT_BSTR)
	{
		if (lstrcmpiW(vStopBehavior.bstrVal, L"beginNow")==0) stopBehavior = iStoryBoard::stop_beginNow;
		else if (lstrcmpiW(vStopBehavior.bstrVal, L"endNow")==0) stopBehavior = iStoryBoard::stop_endNow;
		else if (lstrcmpiW(vStopBehavior.bstrVal, L"begin")==0) stopBehavior = iStoryBoard::stop_begin;
		else if (lstrcmpiW(vStopBehavior.bstrVal, L"end")==0) stopBehavior = iStoryBoard::stop_end;
	}
	((iStoryBoard*)this)->stop(stopBehavior, stopTime);
	return GetDispatch();
}

IDispatch* StoryBoard::Pause()
{
	((iStoryBoard*)this)->pause();
	return GetDispatch();
}

IDispatch* StoryBoard::Resume()
{
	((iStoryBoard*)this)->resume();
	return GetDispatch();
}

StoryBoard* StoryBoard::New( Device* device )
{
	if (device==NULL) return NULL;

	StoryBoard* sb = CreateInstance(); // NEW iStoryBoard(/*(iDevice*)device*/);
	if (sb) sb->AddTo(device->GetDispatch());
	return sb;
}

StoryBoard* StoryBoard::NewAutoInstance()
{
	return NEW iStoryBoard();
}

void StoryBoard::DeleteAutoInstance( StoryBoard* p )
{
	delete (iStoryBoard*)p;
}

HRESULT StoryBoard::GetFrameSets( VARIANT* val )
{
	iStoryBoard* pThis = (iStoryBoard*)this;
	CDispatchArray* pArr = CDispatchArray::New(val);
	if (pArr==NULL) return E_OUTOFMEMORY;

	iFrameSetBase* kfs = pThis->m_keyframe_sets;
	while (kfs)
	{
		pArr->Add(kfs->getDispatch(), kfs->getName());
		kfs = kfs->Next();
	}
	return S_OK;
}

HRESULT StoryBoard::PutNewFrameSet( VARIANT* val )
{
	if (val==NULL || val->vt!=VT_DISPATCH || val->pdispVal==NULL) return E_INVALIDARG;
	FrameSet* fs = DISP_CAST(val->pdispVal, FrameSet);
	if (fs)
		return fs->AddTo(GetDispatch()), S_OK;
	KeyFrameSet* kfs = DISP_CAST(val->pdispVal, KeyFrameSet);
	if (kfs)
		return kfs->AddTo(GetDispatch()), S_OK;
	return E_INVALIDARG;
}

IDispatch* StoryBoard::AddTo( IDispatch* window )
{
	iStoryBoard* pThis = (iStoryBoard*)this;
	iDevice* dev = (iDevice*)DISP_CAST(window, Device);
	if (dev && pThis->m_device==NULL)
	{
		pThis->m_device = dev;
		pThis->addToList(&dev->m_stories);
	}
	return GetDispatch();
}

void StoryBoard::Remove()
{
	iStoryBoard* pThis = (iStoryBoard*)this;
	pThis->removeFromList();
}

bool& StoryBoard::GetStartedRef()
{
	return ((iStoryBoard*)this)->m_started;
}

bool& StoryBoard::GetStoppedRef()
{
	return ((iStoryBoard*)this)->m_stopped;
}

bool& StoryBoard::GetPausedRef()
{
	return ((iStoryBoard*)this)->m_paused;
}

HRESULT StoryBoard::GetOnStart( VARIANT* val )
{
	iStoryBoard* pThis = (iStoryBoard*)this;
	CComVariant v = pThis->m_onstart;
	return v.Detach(val);
}

HRESULT StoryBoard::PutOnStart( VARIANT* val )
{
	if (val->vt != VT_DISPATCH && val->vt != VT_BSTR) return E_INVALIDARG;
	iStoryBoard* pThis = (iStoryBoard*)this;
	return (pThis->m_onstart = *val), S_OK;
}

HRESULT StoryBoard::GetOnStop( VARIANT* val )
{
	iStoryBoard* pThis = (iStoryBoard*)this;
	CComVariant v = pThis->m_onstop;
	return v.Detach(val);
}

HRESULT StoryBoard::PutOnStop( VARIANT* val )
{
	if (val->vt != VT_DISPATCH && val->vt != VT_BSTR) return E_INVALIDARG;
	iStoryBoard* pThis = (iStoryBoard*)this;
	return (pThis->m_onstop = *val), S_OK;
}

HRESULT StoryBoard::GetOnPause( VARIANT* val )
{
	iStoryBoard* pThis = (iStoryBoard*)this;
	CComVariant v = pThis->m_onpause;
	return v.Detach(val);
}

HRESULT StoryBoard::PutOnPause( VARIANT* val )
{
	if (val->vt != VT_DISPATCH && val->vt != VT_BSTR) return E_INVALIDARG;
	iStoryBoard* pThis = (iStoryBoard*)this;
	return (pThis->m_onpause = *val), S_OK;
}

HRESULT StoryBoard::GetOnResume( VARIANT* val )
{
	iStoryBoard* pThis = (iStoryBoard*)this;
	CComVariant v = pThis->m_onresume;
	return v.Detach(val);
}

HRESULT StoryBoard::PutOnResume( VARIANT* val )
{
	if (val->vt != VT_DISPATCH && val->vt != VT_BSTR) return E_INVALIDARG;
	iStoryBoard* pThis = (iStoryBoard*)this;
	return (pThis->m_onresume = *val), S_OK;
}

HRESULT StoryBoard::GetOnStep( VARIANT* val )
{
	iStoryBoard* pThis = (iStoryBoard*)this;
	CComVariant v = pThis->m_onstep;
	return v.Detach(val);
}

HRESULT StoryBoard::PutOnStep( VARIANT* val )
{
	if (val->vt != VT_DISPATCH && val->vt != VT_BSTR) return E_INVALIDARG;
	iStoryBoard* pThis = (iStoryBoard*)this;
	return (pThis->m_onstep = *val), S_OK;
}

HRESULT StoryBoard::PutEventHandler( VARIANT* val )
{
	if (val->vt != VT_DISPATCH || val->pdispVal==NULL) return E_INVALIDARG;
	iStoryBoard* pThis = (iStoryBoard*)this;
	CComDispatchDriver ev = val->pdispVal;
	if (ev.p==NULL) return E_INVALIDARG;
	{
		DISPPARAMS dp = {NULL, NULL, 0, 0};
		DISPID did;
		OLECHAR* evname[] = {L"onstart", L"onstop", L"onpause", L"onresume", L"onstep"}; 

		// onstart
		if (SUCCEEDED(ev->GetIDsOfNames(IID_NULL, &evname[0], 1, LOCALE_SYSTEM_DEFAULT, &did)))
		{
			CComVariant vRet;
			if (SUCCEEDED(ev->Invoke(did, IID_NULL, LOCALE_SYSTEM_DEFAULT, DISPATCH_PROPERTYGET, &dp, &vRet, NULL, NULL)) && vRet.vt==VT_DISPATCH)
				pThis->m_onstart = vRet.pdispVal;
		}

		// onstop
		if (SUCCEEDED(ev->GetIDsOfNames(IID_NULL, &evname[1], 1, LOCALE_SYSTEM_DEFAULT, &did)))
		{
			CComVariant vRet;
			if (SUCCEEDED(ev->Invoke(did, IID_NULL, LOCALE_SYSTEM_DEFAULT, DISPATCH_PROPERTYGET, &dp, &vRet, NULL, NULL)) && vRet.vt==VT_DISPATCH)
				pThis->m_onstop = vRet.pdispVal;
		}

		// onpause
		if (SUCCEEDED(ev->GetIDsOfNames(IID_NULL, &evname[2], 1, LOCALE_SYSTEM_DEFAULT, &did)))
		{
			CComVariant vRet;
			if (SUCCEEDED(ev->Invoke(did, IID_NULL, LOCALE_SYSTEM_DEFAULT, DISPATCH_PROPERTYGET, &dp, &vRet, NULL, NULL)) && vRet.vt==VT_DISPATCH)
				pThis->m_onpause = vRet.pdispVal;
		}

		// onresume
		if (SUCCEEDED(ev->GetIDsOfNames(IID_NULL, &evname[3], 1, LOCALE_SYSTEM_DEFAULT, &did)))
		{
			CComVariant vRet;
			if (SUCCEEDED(ev->Invoke(did, IID_NULL, LOCALE_SYSTEM_DEFAULT, DISPATCH_PROPERTYGET, &dp, &vRet, NULL, NULL)) && vRet.vt==VT_DISPATCH)
				pThis->m_onresume = vRet.pdispVal;
		}

		// onstep
		if (SUCCEEDED(ev->GetIDsOfNames(IID_NULL, &evname[4], 1, LOCALE_SYSTEM_DEFAULT, &did)))
		{
			CComVariant vRet;
			if (SUCCEEDED(ev->Invoke(did, IID_NULL, LOCALE_SYSTEM_DEFAULT, DISPATCH_PROPERTYGET, &dp, &vRet, NULL, NULL)) && vRet.vt==VT_DISPATCH)
				pThis->m_onstep = vRet.pdispVal;
		}
	}
	return S_OK;

	//CComQIPtr<IDispatchEx> dispex = val->pdispVal;
	//if (dispex.p == NULL) return E_INVALIDARG;

	//{
	//	DISPPARAMS dp = {NULL, NULL, 0, 0};
	//	DISPID did;

	//	// onstart
	//	if (SUCCEEDED(dispex->GetDispID(L"onstart", fdexNameCaseInsensitive, &did)))
	//	{
	//		CComVariant vRet;
	//		if (SUCCEEDED(dispex->InvokeEx(did, LOCALE_SYSTEM_DEFAULT, DISPATCH_PROPERTYGET, &dp, &vRet, NULL, NULL)) && vRet.vt==VT_DISPATCH)
	//			pThis->m_onstart = vRet.pdispVal;
	//	}

	//	// onstop
	//	if (SUCCEEDED(dispex->GetDispID(L"onstop", fdexNameCaseInsensitive, &did)))
	//	{
	//		CComVariant vRet;
	//		if (SUCCEEDED(dispex->InvokeEx(did, LOCALE_SYSTEM_DEFAULT, DISPATCH_PROPERTYGET, &dp, &vRet, NULL, NULL)) && vRet.vt==VT_DISPATCH)
	//			pThis->m_onstop = vRet.pdispVal;
	//	}

	//	// onpause
	//	if (SUCCEEDED(dispex->GetDispID(L"onpause", fdexNameCaseInsensitive, &did)))
	//	{
	//		CComVariant vRet;
	//		if (SUCCEEDED(dispex->InvokeEx(did, LOCALE_SYSTEM_DEFAULT, DISPATCH_PROPERTYGET, &dp, &vRet, NULL, NULL)) && vRet.vt==VT_DISPATCH)
	//			pThis->m_onpause = vRet.pdispVal;
	//	}

	//	// onresume
	//	if (SUCCEEDED(dispex->GetDispID(L"onresume", fdexNameCaseInsensitive, &did)))
	//	{
	//		CComVariant vRet;
	//		if (SUCCEEDED(dispex->InvokeEx(did, LOCALE_SYSTEM_DEFAULT, DISPATCH_PROPERTYGET, &dp, &vRet, NULL, NULL)) && vRet.vt==VT_DISPATCH)
	//			pThis->m_onresume = vRet.pdispVal;
	//	}
	//}
	//return S_OK;
}

void StoryBoard::ParseAttribute( HANDLE hNode )
{
	ATLASSERT(hNode);
	iStoryBoard* sb = (iStoryBoard*)this;
	CMarkupNode* n = (CMarkupNode*)hNode;
	Name = n->GetAttributeValue(L"name", L"id");

	// 处理脚本属性
	sb->m_onstart = n->GetAttributeValue(L"onstart", L"start");
	sb->m_onstop = n->GetAttributeValue(L"onstop", L"stop");
	sb->m_onpause = n->GetAttributeValue(L"onpause", L"pause");
	sb->m_onresume = n->GetAttributeValue(L"onresume", L"resume");
	sb->m_onstep = n->GetAttributeValue(L"onstep", L"step");
}

BOOL StoryBoard::IsChildNode( HANDLE hChild )
{
	if (hChild==NULL) return FALSE;
	CMarkupNode* node = (CMarkupNode*)hChild;
	LPCOLESTR name = node->GetName();

	// From/To/By动画
	if (lstrcmpiW(name, L"FrameSet") == 0)
	{
		FrameSet* fs = FrameSet::New(this);
		if (fs==NULL) return TRUE;
		fs->ParseAttribute(hChild);
	}

	// 关键帧动画
	else if (lstrcmpiW(name, L"KeyFrameSet") == 0)
	{
		KeyFrameSet* kfs = KeyFrameSet::New(this);
		if (kfs==NULL) return TRUE;
		kfs->ParseAttribute(hChild);

		for (CMarkupNode child=node->GetChild(); child.IsValid(); child=child.GetSibling())
			kfs->IsChildNode(&child);
	}

	else return FALSE;

	return TRUE;
}

#ifdef _DEBUG
void StoryBoard::__DebugOnAddRef()
{
	//LineLog(L"StoryBoard - AddRef: %d", __disp._refcount);
}

void StoryBoard::__DebugOnRelease()
{
	//LineLog(L"StoryBoard - Release: %d", __disp._refcount);
}
#endif // _DEBUG

//////////////////////////////////////////////////////////////////////////
KeyFrame* KeyFrame::New( KeyFrameSet* keyframeset )
{
	if (keyframeset==NULL) return NULL;
	KeyFrame* kf = CreateInstance(); // NEW iKeyFrame(/*(iKeyFrameSet*)keyframeset*/);
	if (kf) kf->AddTo(keyframeset->GetDispatch());
	return kf;
}

KeyFrame* KeyFrame::NewAutoInstance()
{
	return NEW iKeyFrame();
}

void KeyFrame::DeleteAutoInstance( KeyFrame* p )
{
	delete (iKeyFrame*)p;
}

HRESULT KeyFrame::GetKeyTime( VARIANT* val )
{
	iKeyFrame* pThis = (iKeyFrame*)this;
	CComVariant v = pThis->m_keytime;
	return v.Detach(val);
}

HRESULT KeyFrame::PutKeyTime( VARIANT* val )
{
	iKeyFrame* pThis = (iKeyFrame*)this;
	HRESULT hr = variant2time(val, pThis->m_keytime);
	if (SUCCEEDED(hr)) pThis->reorder();
	return hr;
}

HRESULT KeyFrame::GetKeyValue( VARIANT* val )
{
	iKeyFrame* pThis = (iKeyFrame*)this;
	CComVariant v = pThis->m_keyvalue;
	return v.Detach(val);
}

HRESULT KeyFrame::PutKeyValue( VARIANT* val )
{
	iKeyFrame* pThis = (iKeyFrame*)this;

	if (val->vt == VT_BSTR)
		return pThis->m_keyvalue.InitFromString(val->bstrVal) ? S_OK : E_INVALIDARG;
	else if (val->vt != VT_EMPTY)
		return pThis->m_keyvalue.CopyFrom(val) ? S_OK : E_INVALIDARG;

	return E_INVALIDARG;
}

IDispatch* KeyFrame::AddTo( IDispatch* keyframeset )
{
	iKeyFrame* pThis = (iKeyFrame*)this;

	iKeyFrameSet* kfs = (iKeyFrameSet*)DISP_CAST(keyframeset, KeyFrameSet);
	if (kfs && !pThis->isHead(&kfs->m_keyframes))
	{
		pThis->m_keyframe_set = kfs;
		pThis->addToList(&kfs->m_keyframes);
		//kf->_clearSpline();
	}
	return GetDispatch();
}

void KeyFrame::Remove()
{
	((iKeyFrame*)this)->removeFromList();
}

void KeyFrame::ParseAttribute( HANDLE hNode )
{
	ATLASSERT(hNode);
	CMarkupNode* n = (CMarkupNode*)hNode;
	Name = n->GetAttributeValue(L"name", L"id");

	CComVariant v;
	v = n->GetAttributeValue(L"keytime", L"time"); PutKeyTime(&v);
	v = n->GetAttributeValue(L"keyvalue", L"value"); PutKeyValue(&v);
}

//////////////////////////////////////////////////////////////////////////
KeyFrameSet* KeyFrameSet::New( StoryBoard* story )
{
	if (story==NULL) return NULL;
	KeyFrameSet* kfs = CreateInstance(); // NEW iKeyFrameSet((iStoryBoard*)story);
	if (kfs) kfs->AddTo(story->GetDispatch());
	return kfs;
}

KeyFrameSet* KeyFrameSet::NewAutoInstance()
{
	return NEW iKeyFrameSet();
}

void KeyFrameSet::DeleteAutoInstance( KeyFrameSet* p )
{
	delete (iKeyFrameSet*)p;
}

CComBSTR& KeyFrameSet::GetTargetTypeRef()
{
	iKeyFrameSet* pThis = (iKeyFrameSet*)this;
	return pThis->m_datatype;
}

HRESULT KeyFrameSet::GetTarget( VARIANT* val )
{
	iKeyFrameSet* pThis = (iKeyFrameSet*)this;
	CComVariant v = pThis->m_target;
	return v.Detach(val);
}

HRESULT KeyFrameSet::PutTarget( VARIANT* val )
{
	iKeyFrameSet* pThis = (iKeyFrameSet*)this;
	pThis->m_target = *val;
	return S_OK;
}

CComBSTR& KeyFrameSet::GetTargetAttributeRef()
{
	iKeyFrameSet* pThis = (iKeyFrameSet*)this;
	return pThis->m_target_attribute;
}

bool& KeyFrameSet::GetReverseRef()
{
	iKeyFrameSet* pThis = (iKeyFrameSet*)this;
	return pThis->m_reverse;
}

HRESULT KeyFrameSet::PutReverse( VARIANT* val )
{
	if (val==NULL || val->vt==VT_EMPTY) return E_INVALIDARG;
	iKeyFrameSet* pThis = (iKeyFrameSet*)this;
	if (val->vt==VT_BSTR && lstrcmpiW(val->bstrVal, L"closure")==0) return (pThis->m_reverse=pThis->m_closure=true), S_OK;
	pThis->m_closure = false;
	CComVariant v;
	HRESULT hr = v.ChangeType(VT_BOOL, val);
	if (SUCCEEDED(hr)) pThis->m_reverse = (v.boolVal!=VARIANT_FALSE);
	return hr;
}

ULONG& KeyFrameSet::GetRepeatRef()
{
	iKeyFrameSet* pThis = (iKeyFrameSet*)this;
	return pThis->m_repeat;
}

HRESULT KeyFrameSet::PutRepeat( VARIANT* val )
{
	if (val==NULL || val->vt==VT_EMPTY) return E_INVALIDARG;
	iKeyFrameSet* pThis = (iKeyFrameSet*)this;
	if (val->vt==VT_BSTR && lstrcmpiW(val->bstrVal, L"forever")==0) return (pThis->m_repeat=0), S_OK;
	CComVariant v;
	HRESULT hr = v.ChangeType(VT_UI4, val);
	if (SUCCEEDED(hr)) pThis->m_repeat = v.ulVal;
	return hr;
}

bool& KeyFrameSet::GetSplineRef()
{
	iKeyFrameSet* pThis = (iKeyFrameSet*)this;
	return pThis->m_spline;
}

float& KeyFrameSet::GetDelayRef()
{
	iKeyFrameSet* pThis = (iKeyFrameSet*)this;
	return pThis->m_delay;
}

HRESULT KeyFrameSet::PutDelay(VARIANT* val)
{
	iKeyFrameSet* pThis = (iKeyFrameSet*)this;
	return variant2time(val, pThis->m_delay);
}

float& KeyFrameSet::GetIdleRef()
{
	iKeyFrameSet* pThis = (iKeyFrameSet*)this;
	return pThis->m_idle;
}

HRESULT KeyFrameSet::PutIdle(VARIANT* val)
{
	iKeyFrameSet* pThis = (iKeyFrameSet*)this;
	return variant2time(val, pThis->m_idle);
}

bool& KeyFrameSet::GetIsClosureRef()
{
	iKeyFrameSet* pThis = (iKeyFrameSet*)this;
	return pThis->m_closure;
}

IDispatch* KeyFrameSet::AddTo( IDispatch* story )
{
	iKeyFrameSet* pThis = (iKeyFrameSet*)this;
	iStoryBoard* sb = (iStoryBoard*)DISP_CAST(story, StoryBoard);
	if (sb && !pThis->isHead(&sb->m_keyframe_sets))
	{
		pThis->m_story = sb;
		pThis->addToList(&sb->m_keyframe_sets);
	}
	return GetDispatch();
}

HRESULT KeyFrameSet::GetKeyFrames( VARIANT* val )
{
	iKeyFrameSet* pThis = (iKeyFrameSet*)this;
	CDispatchArray* pArr = CDispatchArray::New(val);
	if (pArr==NULL) return E_OUTOFMEMORY;

	iKeyFrame* kf = pThis->m_keyframes;
	while (kf)
	{
		pArr->Add(kf->GetDispatch(), kf->Name);
		kf = kf->Next();
	}
	return S_OK;
}

HRESULT KeyFrameSet::PutNewKeyFrame( VARIANT* val )
{
	if (val==NULL || val->vt!=VT_DISPATCH || val->pdispVal==NULL) return E_INVALIDARG;
	KeyFrame* kf = DISP_CAST(val->pdispVal, KeyFrame);
	if (kf)
		return kf->AddTo(GetDispatch()), S_OK;
	return E_INVALIDARG;
}

void KeyFrameSet::Remove()
{
	((iKeyFrameSet*)this)->removeFromList();
}

void KeyFrameSet::OnPropertyChanged( DISPID dispid, LPCOLESTR name )
{
	switch(dispid)
	{
	case 3: // NewKeyFrame
	case 10: // TargetType
	case 13: // Reverse
	case 15: // Spline
		((iKeyFrameSet*)this)->_clearSpline();
		break;
	}
}

void KeyFrameSet::ParseAttribute( HANDLE hNode )
{
	ATLASSERT(hNode);
	iKeyFrameSet* kfs = (iKeyFrameSet*)this;
	CMarkupNode* n = (CMarkupNode*)hNode;
	Name = n->GetAttributeValue(L"name", L"id");

	kfs->m_datatype = n->GetAttributeValue(L"targettype");
	kfs->m_target = n->GetAttributeValue(L"target");
	kfs->m_target_attribute = n->GetAttributeValue(L"TargetAttribute");

	CComVariant v;
	v = n->GetAttributeValue(L"reverse"); PutReverse(&v);
	v = n->GetAttributeValue(L"repeat"); PutRepeat(&v);
	v = n->GetAttributeValue(L"delay", L"delaytime"); PutDelay(&v);
	v = n->GetAttributeValue(L"idle", L"idletime"); PutIdle(&v);

	LPCOLESTR p = n->GetAttributeValue(L"spline");
	if (p) kfs->m_spline = !!ParseBoolString(p);

	LPCOLESTR pTimes = n->GetAttributeValue(L"times", L"keytimes");
	LPCOLESTR pValues = n->GetAttributeValue(L"values", L"keyvalues");
	if (pTimes && pValues)
	{
		CStrArray atimes, avalues;
		if (SplitStringToArray(pTimes, atimes, L" ,\t\r\n") &&
			SplitStringToArray(pValues, avalues, L" ,\t\r\n"))
		{
			int num = min(atimes.GetSize(), avalues.GetSize());
			for (int i=0; i<num; i++)
			{
				KeyFrame* kf = KeyFrame::New(this);
				if (kf)
				{
					kf->PutKeyTime(&CComVariant(atimes[i]));
					kf->PutKeyValue(&CComVariant(avalues[i]));
				}
			}
		}
	}
}

BOOL KeyFrameSet::IsChildNode( HANDLE hChild )
{
	if (hChild==NULL) return FALSE;
	CMarkupNode* node = (CMarkupNode*)hChild;
	LPCOLESTR name = node->GetName();

	// 关键帧
	if (lstrcmpiW(name, L"KeyFrame") == 0)
	{
		KeyFrame* kf = KeyFrame::New(this);
		if (kf==NULL) return TRUE;
		kf->ParseAttribute(hChild);
	}

	else return FALSE;

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
FrameSet* FrameSet::New( StoryBoard* story )
{
	if (story==NULL) return NULL;
	FrameSet* fs = CreateInstance();
	if (fs) fs->AddTo(story->GetDispatch());
	return fs;
	//return NEW iFrameSet((iStoryBoard*)story);
}

FrameSet* FrameSet::NewAutoInstance()
{
	return NEW iFrameSet();
}

void FrameSet::DeleteAutoInstance( FrameSet* p )
{
	delete (iFrameSet*)p;
}

CComBSTR& FrameSet::GetTargetTypeRef()
{
	iFrameSet* pThis = (iFrameSet*)this;
	return pThis->m_datatype;
}

CComBSTR& FrameSet::GetTargetAttributeRef()
{
	iFrameSet* pThis = (iFrameSet*)this;
	return pThis->m_target_attribute;
}

bool& FrameSet::GetReverseRef()
{
	iFrameSet* pThis = (iFrameSet*)this;
	return pThis->m_reverse;
}

ULONG& FrameSet::GetRepeatRef()
{
	iFrameSet* pThis = (iFrameSet*)this;
	return pThis->m_repeat;
}

HRESULT FrameSet::GetTarget( VARIANT* val )
{
	iFrameSet* pThis = (iFrameSet*)this;
	CComVariant v = pThis->m_target;
	return v.Detach(val);
}

HRESULT FrameSet::PutTarget( VARIANT* val )
{
	iFrameSet* pThis = (iFrameSet*)this;
	pThis->m_target = *val;
	return S_OK;
}

HRESULT FrameSet::PutRepeat( VARIANT* val )
{
	if (val==NULL || val->vt==VT_EMPTY) return E_INVALIDARG;
	iFrameSet* pThis = (iFrameSet*)this;
	if (val->vt==VT_BSTR && lstrcmpiW(val->bstrVal, L"forever")==0) return (pThis->m_repeat=0), S_OK;
	CComVariant v;
	HRESULT hr = v.ChangeType(VT_UI4, val);
	if (SUCCEEDED(hr)) pThis->m_repeat = v.ulVal;
	return hr;
}

float& FrameSet::GetDelayRef()
{
	iFrameSet* pThis = (iFrameSet*)this;
	return pThis->m_delay;
}

HRESULT FrameSet::PutDelay(VARIANT* val)
{
	iFrameSet* pThis = (iFrameSet*)this;
	return variant2time(val, pThis->m_delay);
}

float& FrameSet::GetIdleRef()
{
	iFrameSet* pThis = (iFrameSet*)this;
	return pThis->m_idle;
}

HRESULT FrameSet::PutIdle(VARIANT* val)
{
	iFrameSet* pThis = (iFrameSet*)this;
	return variant2time(val, pThis->m_idle);
}

float& FrameSet::GetDurationRef()
{
	iFrameSet* pThis = (iFrameSet*)this;
	return pThis->m_duration;
}

HRESULT FrameSet::PutDuration( VARIANT* val )
{
	iFrameSet* pThis = (iFrameSet*)this;
	return variant2time(val, pThis->m_duration);
}

VARIANT& FrameSet::GetFromRef()
{
	iFrameSet* pThis = (iFrameSet*)this;
	return (VARIANT&)pThis->m_from;
}

VARIANT& FrameSet::GetToRef()
{
	iFrameSet* pThis = (iFrameSet*)this;
	return (VARIANT&)pThis->m_to;
}

VARIANT& FrameSet::GetByRef()
{
	iFrameSet* pThis = (iFrameSet*)this;
	return (VARIANT&)pThis->m_by;
}

HRESULT FrameSet::PutFrom( VARIANT* val )
{
	iFrameSet* pThis = (iFrameSet*)this;

	if (val->vt == VT_BSTR)
		return pThis->m_from.InitFromString(val->bstrVal) ? (pThis->m_has_from=TRUE), S_OK : E_INVALIDARG;
	else if (val->vt != VT_EMPTY)
		return pThis->m_from.CopyFrom(val) ? (pThis->m_has_from=TRUE), S_OK : E_INVALIDARG;

	return E_INVALIDARG;
}

HRESULT FrameSet::PutTo( VARIANT* val )
{
	iFrameSet* pThis = (iFrameSet*)this;

	if (val->vt == VT_BSTR)
		return pThis->m_to.InitFromString(val->bstrVal) ? (pThis->m_has_to=TRUE), S_OK : E_INVALIDARG;
	else if (val->vt != VT_EMPTY)
		return pThis->m_to.CopyFrom(val) ? (pThis->m_has_to=TRUE), S_OK : E_INVALIDARG;

	return E_INVALIDARG;
}

HRESULT FrameSet::PutBy( VARIANT* val )
{
	iFrameSet* pThis = (iFrameSet*)this;

	if (val->vt == VT_BSTR)
		return pThis->m_by.InitFromString(val->bstrVal) ? (pThis->m_has_by=TRUE), S_OK : E_INVALIDARG;
	else if (val->vt != VT_EMPTY)
		return pThis->m_by.CopyFrom(val) ? (pThis->m_has_by=TRUE), S_OK : E_INVALIDARG;

	return E_INVALIDARG;
}

IDispatch* FrameSet::AddTo( IDispatch* story )
{
	iFrameSet* pThis = (iFrameSet*)this;
	iStoryBoard* sb = (iStoryBoard*)DISP_CAST(story, StoryBoard);
	if (sb && !pThis->isHead(&sb->m_keyframe_sets))
	{
		pThis->m_story = sb;
		pThis->addToList(&sb->m_keyframe_sets);
	}
	return GetDispatch();
}

void FrameSet::Remove()
{
	((iFrameSet*)this)->removeFromList();
}

void FrameSet::ParseAttribute( HANDLE hNode )
{
	ATLASSERT(hNode);
	iFrameSet* fs = (iFrameSet*)this;
	CMarkupNode* n = (CMarkupNode*)hNode;
	Name = n->GetAttributeValue(L"name", L"id");

	fs->m_datatype = n->GetAttributeValue(L"targettype");
	fs->m_target = n->GetAttributeValue(L"target");
	fs->m_target_attribute = n->GetAttributeValue(L"TargetAttribute");

	LPCOLESTR p = n->GetAttributeValue(L"reverse");
	if (p) fs->m_reverse = !!ParseBoolString(p);

	if (p = n->GetAttributeValue(L"repeat")) PutRepeat(&CComVariant(p));
	if (p = n->GetAttributeValue(L"duration")) PutDuration(&CComVariant(p));
	if (p = n->GetAttributeValue(L"from")) PutFrom(&CComVariant(p));
	if (p = n->GetAttributeValue(L"to")) PutTo(&CComVariant(p));
	if (p = n->GetAttributeValue(L"by")) PutBy(&CComVariant(p));

	CComVariant v;
	v = n->GetAttributeValue(L"delay", L"delaytime"); PutDelay(&v);
	v = n->GetAttributeValue(L"idle", L"idletime"); PutIdle(&v);
}

//////////////////////////////////////////////////////////////////////////
CComBSTR& Material::GetTextureNameRef()
{
	iMaterial* m = (iMaterial*)this;
	return m->texname;
}

HRESULT Material::PutTextureName( VARIANT* val )
{
	iMaterial* m = (iMaterial*)this;
	CComVariant v;
	if (SUCCEEDED(v.ChangeType(VT_I4,val)))
		return m->resetName(v.lVal), S_OK;
	else if (val->vt == VT_BSTR)
		return m->resetName(val->bstrVal), S_OK;
	return E_INVALIDARG;
}

void Material::ParseAttribute( HANDLE hNode )
{
	ATLASSERT(hNode);
	CMarkupNode* n = (CMarkupNode*)hNode;
	iMaterial* m = (iMaterial*)this;

	if (TryLoad_D3DCOLORVALUE_FromString(n->GetAttributeValue(L"Diffuse"), material.Diffuse)) m_Enabled = true;
	if (TryLoad_D3DCOLORVALUE_FromString(n->GetAttributeValue(L"Specular"), material.Specular)) m_Enabled = true;
	if (TryLoad_D3DCOLORVALUE_FromString(n->GetAttributeValue(L"Ambient"), material.Ambient)) m_Enabled = true;
	if (TryLoad_D3DCOLORVALUE_FromString(n->GetAttributeValue(L"Emissive"), material.Emissive)) m_Enabled = true;
	if (TryLoad_float_FromString(n->GetAttributeValue(L"Power"), material.Power)) m_Enabled = true;

	LPCOLESTR name = n->GetAttributeValue(L"texture");
	if (name)
	{
		m_Enabled = true;
		PutTextureName(&CComVariant(name));
	}

	LPCOLESTR tech = n->GetAttributeValue(L"technique", L"tech");
	if (tech) GetTechniqueRef() = tech;

	m->effect_params.set_params_string(n->GetAttributeValue(L"EffectParameters", L"Parameters"));

	m->onRender = n->GetAttributeValue(L"onRender", L"Render");
	m->onPrepareEffect = n->GetAttributeValue(L"onPrepareEffect", L"PrepareEffect");
}

CComBSTR& Material::GetTechniqueRef()
{
	iMaterial* m = (iMaterial*)this;
	return m->technique;
}

void Material::OnPropertyChanged( DISPID dispid, LPCOLESTR name )
{
	iMaterial* m = (iMaterial*)this;
	if (dispid==3) // technique
	{
		m->effect = NULL;
	}
}

#ifdef _DEBUG
void Material::__DebugOnAddRef()
{
	//DebugBreak();
	//ATLASSERT(__disp._refcount<2);
}

void Material::__DebugOnRelease()
{

}
#endif // _DEBUG

//////////////////////////////////////////////////////////////////////////
//CComBSTR& Model::GetNameRef()
//{
//	return ((iModel*)this)->m_name;
//}

#define processModel(model) \
	model* model::NewAutoInstance() { return NEW i##model(); } \
	void model::DeleteAutoInstance(model* p) { delete (i##model*)p; } \
	model* model::New(Device* device) \
	{ \
		if (device==NULL) return NULL; \
		i##model* p = (i##model*)CreateInstance(); \
		if (p) { p->m_device=(iDevice*)device; p->TList<iModelBase>::addToList(&p->m_device->m_models); } \
		return p; \
		/*return NEW i##model((iDevice*)device);*/ \
	} \
	CComBSTR& model::GetNameRef() { return ((i##model*)this)->m_name; } \
	Transform3D& model::GetTransformRef() { return ((i##model*)this)->m_local_transform; } \
	CComVariant& model::GetonTransformRef() { return ((i##model*)this)->onTransform; } \
	CComVariant& model::GetonRenderRef() { return ((i##model*)this)->onRender; } \
	CComVariant& model::GetonPrepareEffectRef() { return ((i##model*)this)->onPrepareEffect; } \
	CComVariant& model::GetonMouseEnterRef() { return ((i##model*)this)->onMouseEnter; } \
	CComVariant& model::GetonMouseLeaveRef() { return ((i##model*)this)->onMouseLeave; } \
	CComVariant& model::GetonClickRef() { return ((i##model*)this)->onClick; } \
	CComVariant& model::GetonDblClickRef() { return ((i##model*)this)->onDblClick; } \
	IDispatch* model::CreateEntity() { iEntity* e = ((i##model*)this)->createEntity(); return e?e->GetDispatch():NULL; } \
	IDispatch* model::SubmitChange() { ((i##model*)this)->resetMesh(); return GetDispatch(); } \
	void model::Remove() { ((i##model*)this)->TList<iModelBase>::removeFromList(); } \
	HRESULT model::GetParent(VARIANT* val) { CComVariant v; if (((i##model*)this)->m_parent) v=((i##model*)this)->m_parent->getDispatch(); return v.Detach(val); } \
	HRESULT model::GetChildren(VARIANT* val) \
	{ \
		CDispatchArray* pArr = CDispatchArray::New(val); \
		if (pArr==NULL) return E_OUTOFMEMORY; \
		iModelBase* child = ((i##model*)this)->m_children; \
		while (child) { pArr->Add(child->getDispatch(), child->m_name); child = child->m_siblings; } \
		return S_OK; \
	} \
	HRESULT model::GetEntities(VARIANT* val) \
	{ \
		CDispatchArray* pArr = CDispatchArray::New(val); \
		if (pArr==NULL) return E_OUTOFMEMORY; \
		iEntity* e = ((i##model*)this)->m_entities; \
		while (e) { pArr->Add(e->GetDispatch(), e->m_Name); e = e->next; } \
		return S_OK; \
	} \
	HRESULT model::GetMaterials(VARIANT* val) \
	{ \
		CDispatchArray* pArr = CDispatchArray::New(val); \
		if (pArr==NULL) return E_OUTOFMEMORY; \
		((i##model*)this)->getMaterials(pArr); \
		return S_OK; \
	}

__foreach_model(processModel)
#undef processModel

//////////////////////////////////////////////////////////////////////////
HRESULT Model::PutNormals( VARIANT* val )
{
	// Normals="0 0 1  0 0 1  0 0 1  0 0 1"
	if (val==NULL || val->vt != VT_BSTR) return E_INVALIDARG;

	CStrArray strs;
	if (!SplitStringToArray(val->bstrVal, strs, L"(), \t\r\n") || strs.GetSize()%3 != 0) return E_INVALIDARG;

	for (int i=0; i<strs.GetSize(); i+=3)
	{
		D3DVECTOR vec = {0};
		if (!TryLoad_float_FromString(strs[i], vec.x) ||
			!TryLoad_float_FromString(strs[i+1], vec.y) ||
			!TryLoad_float_FromString(strs[i+2], vec.z))
			return E_INVALIDARG;

		m_Normals.Add(vec);
	}
	SubmitChange();
	return S_OK;
}

HRESULT Model::PutPoints( VARIANT* val )
{
	// Positions="-1 -1 0  1 -1 0  -1 1 0  1 1 0"
	if (val==NULL || val->vt != VT_BSTR) return E_INVALIDARG;

	CStrArray strs;
	if (!SplitStringToArray(val->bstrVal, strs, L"(), \t\r\n") || strs.GetSize()%3 != 0) return E_INVALIDARG;

	for (int i=0; i<strs.GetSize(); i+=3)
	{
		D3DVECTOR vec = {0};
		if (!TryLoad_float_FromString(strs[i], vec.x) ||
			!TryLoad_float_FromString(strs[i+1], vec.y) ||
			!TryLoad_float_FromString(strs[i+2], vec.z))
			return E_INVALIDARG;

		m_Points.Add(vec);
	}
	SubmitChange();
	return S_OK;
}

HRESULT Model::PutTextureCoordinates( VARIANT* val )
{
	// TextureCoordinates="0 1  1 1  0 0  1 0"
	if (val==NULL || val->vt != VT_BSTR) return E_INVALIDARG;

	CStrArray strs;
	if (!SplitStringToArray(val->bstrVal, strs, L"(), \t\r\n") || strs.GetSize()%2 != 0) return E_INVALIDARG;

	for (int i=0; i<strs.GetSize(); i+=2)
	{
		Point2D pt;
		if (!TryLoad_float_FromString(strs[i], pt.x) ||
			!TryLoad_float_FromString(strs[i+1], pt.y))
			return E_INVALIDARG;

		m_TextureCoordinates.Add(pt);
	}
	SubmitChange();
	return S_OK;
}

HRESULT Model::PutTriangleIndices( VARIANT* val )
{
	// TriangleIndices="0 1 2  1 3 2"
	if (val==NULL || val->vt != VT_BSTR) return E_INVALIDARG;

	CStrArray strs;
	if (!SplitStringToArray(val->bstrVal, strs, L"(), \t\r\n") || strs.GetSize()%3 != 0) return E_INVALIDARG;

	for (int i=0; i<strs.GetSize(); i++)
	{
		long l;
		if (!TryLoad_long_FromString(strs[i], l))
			return E_INVALIDARG;

		m_TriangleIndices.Add(l);
	}
	SubmitChange();
	return S_OK;
}

void Model::ParseAttribute( HANDLE hNode )
{
	ATLASSERT(hNode);
	CMarkupNode* n = (CMarkupNode*)hNode;
	iModel* m = (iModel*)this;

	PutPoints(&CComVariant(n->GetAttributeValue(L"Points")));
	PutTriangleIndices(&CComVariant(n->GetAttributeValue(L"TriangleIndices", L"Indices")));
	PutTextureCoordinates(&CComVariant(n->GetAttributeValue(L"TextureCoordinates", L"UV")));
	PutNormals(&CComVariant(n->GetAttributeValue(L"Normals")));
}

//////////////////////////////////////////////////////////////////////////
void XModel::ParseAttribute( HANDLE hNode )
{
	ATLASSERT(hNode);
	CMarkupNode* n = (CMarkupNode*)hNode;
	iXModel* m = (iXModel*)this;

	m->load(n->GetAttributeValue(L"src", L"url"));
}

//////////////////////////////////////////////////////////////////////////
void QuadModel::ParseAttribute( HANDLE hNode )
{
	ATLASSERT(hNode);
	CMarkupNode* n = (CMarkupNode*)hNode;
	iQuadModel* m = (iQuadModel*)this;

	TryLoad_float_FromString(n->GetAttributeValue(L"Width"), Width);
	TryLoad_float_FromString(n->GetAttributeValue(L"Height"), Height);
}

//////////////////////////////////////////////////////////////////////////
void BoxModel::ParseAttribute( HANDLE hNode )
{
	ATLASSERT(hNode);
	CMarkupNode* n = (CMarkupNode*)hNode;
	iBoxModel* m = (iBoxModel*)this;

	TryLoad_float_FromString(n->GetAttributeValue(L"Width"), Width);
	TryLoad_float_FromString(n->GetAttributeValue(L"Height"), Height);
	TryLoad_float_FromString(n->GetAttributeValue(L"Depth"), Depth);
}

//////////////////////////////////////////////////////////////////////////
void CylinderModel::ParseAttribute( HANDLE hNode )
{
	ATLASSERT(hNode);
	CMarkupNode* n = (CMarkupNode*)hNode;
	iCylinderModel* m = (iCylinderModel*)this;

	TryLoad_float_FromString(n->GetAttributeValue(L"Radius1"), Radius1);
	TryLoad_float_FromString(n->GetAttributeValue(L"Radius2"), Radius2);
	TryLoad_float_FromString(n->GetAttributeValue(L"Length"), Length);
	long l;
	if (TryLoad_long_FromString(n->GetAttributeValue(L"Slices"), l)) Slices = (ULONG)l;
	if (TryLoad_long_FromString(n->GetAttributeValue(L"Stacks"), l)) Stacks = (ULONG)l;
}

//////////////////////////////////////////////////////////////////////////
void PolygonModel::ParseAttribute( HANDLE hNode )
{
	ATLASSERT(hNode);
	CMarkupNode* n = (CMarkupNode*)hNode;
	iPolygonModel* m = (iPolygonModel*)this;

	TryLoad_float_FromString(n->GetAttributeValue(L"Length"), Length);
	long l;
	if (TryLoad_long_FromString(n->GetAttributeValue(L"Sides"), l)) Sides = max((ULONG)l,3);
}

//////////////////////////////////////////////////////////////////////////
void SphereModel::ParseAttribute( HANDLE hNode )
{
	ATLASSERT(hNode);
	CMarkupNode* n = (CMarkupNode*)hNode;
	iSphereModel* m = (iSphereModel*)this;

	TryLoad_float_FromString(n->GetAttributeValue(L"Radius"), Radius);
	long l;
	if (TryLoad_long_FromString(n->GetAttributeValue(L"Slices"), l)) Slices = (ULONG)l;
	if (TryLoad_long_FromString(n->GetAttributeValue(L"Stacks"), l)) Stacks = (ULONG)l;
}

//////////////////////////////////////////////////////////////////////////
void TorusModel::ParseAttribute( HANDLE hNode )
{
	ATLASSERT(hNode);
	CMarkupNode* n = (CMarkupNode*)hNode;
	iTorusModel* m = (iTorusModel*)this;

	TryLoad_float_FromString(n->GetAttributeValue(L"InnerRadius"), InnerRadius);
	TryLoad_float_FromString(n->GetAttributeValue(L"OuterRadius"), OuterRadius);
	long l;
	if (TryLoad_long_FromString(n->GetAttributeValue(L"Sides"), l)) Sides = (ULONG)l;
	if (TryLoad_long_FromString(n->GetAttributeValue(L"Rings"), l)) Rings = (ULONG)l;
}

//////////////////////////////////////////////////////////////////////////
void TextModel::ParseAttribute( HANDLE hNode )
{
	ATLASSERT(hNode);
	CMarkupNode* n = (CMarkupNode*)hNode;
	iTextModel* m = (iTextModel*)this;

	Text = n->GetAttributeValue(L"text");
	TryLoad_float_FromString(n->GetAttributeValue(L"Deviation"), Deviation);
	TryLoad_float_FromString(n->GetAttributeValue(L"Extrusion"), Extrusion);

	Center = !!ParseBoolString(n->GetAttributeValue(L"Center"));

	// 解析字体
	CStrArray strs;
	if (SplitStringToArray(n->GetAttributeValue(L"Font"), strs))
	{
		LONG mask;
		for (int i=0; i<strs.GetSize(); i++)
		{
			if (!TryLoadFontFaceFromString(strs[i], Font, mask) &&
				!TryLoadFontStyleFromString(strs[i], Font, mask))
				TryLoadFontSizeFromString(strs[i], Font, mask);
		}
	}
	Font.Validate();
}
//
//D3DXVECTOR3& TextModel::GetCenterRef()
//{
//	iTextModel* m = (iTextModel*)this;
//	return m->m_meshInfo.boundCenter;
//}

//////////////////////////////////////////////////////////////////////////
void TeapotModel::ParseAttribute( HANDLE hNode )
{
	ATLASSERT(hNode);
	CMarkupNode* n = (CMarkupNode*)hNode;
	iTeapotModel* m = (iTeapotModel*)this;
}

//////////////////////////////////////////////////////////////////////////
AnimationSet* AnimationSet::NewAutoInstance()
{
	return NEW iAnimationSet;
}

void AnimationSet::DeleteAutoInstance( AnimationSet* p )
{
	delete (iAnimationSet*)p;
}

//////////////////////////////////////////////////////////////////////////
void Entity::Remove()
{
	iEntity* e = (iEntity*)this;
	e->TList<iEntity>::removeFromList();
	e->TList<iEntity,1>::removeFromList();
}

Material& Entity::GetMaterialRef()
{
	iEntity* e = (iEntity*)this;
	return (Material&)e->m_Material;
}

HRESULT Entity::GetAnimationSets(VARIANT* val)
{
	iEntity* e = (iEntity*)this;
	CComVariant v;
	if (e->m_animations)
		v = (IDispatch*)e->m_animations;
	return v.Detach(val);
}

void Entity::ResetTime()
{
	iEntity* e = (iEntity*)this;
	e->resetTime();
}

void Entity::SetAnimationSet( VARIANT as )
{
	iEntity* e = (iEntity*)this;
	CComVariant v;
	// 传递的动画集对象
	if (as.vt == VT_DISPATCH)
	{
		iAnimationSet* ias = (iAnimationSet*)DISP_CAST(as.pdispVal, AnimationSet);
		if (ias) e->setAnimationSet(ias->m_as.p);
		return;
	}

	// 传递的动画集名称
	if (as.vt == VT_BSTR)
	{
		CComPtr<ID3DXAnimationSet> das;
		if (e->findAnimationSet(as.bstrVal, &das))
		{
			e->setAnimationSet(das);
			return;
		}
	}

	// 传递的动画集索引
	if (SUCCEEDED(v.ChangeType(VT_UI4, &as)) && v.ulVal < e->getAnimationSetCount())
	{
		e->setAnimationSet(v.ulVal);
	}
}

void Entity::DeleteAutoInstance( Entity* p )
{
	delete (iEntity*)p;
}

void Entity::SetEffectParameters( LPCOLESTR params )
{
	iEntity* e = (iEntity*)this;
	e->effect_params.set_params_string(params);
}

CComBSTR& Entity::GetParametersRef()
{
	iEntity* e = (iEntity*)this;
	return e->effect_params.params_string;
}

HRESULT Entity::PutParameters( VARIANT* val )
{
	if (val->vt!=VT_BSTR) return E_INVALIDARG;
	SetEffectParameters(val->bstrVal);
	return S_OK;
}

HRESULT Entity::GetScene(VARIANT* val)
{
	iEntity* e = (iEntity*)this;
	CComVariant v;
	if (e->m_scene)
		v = e->m_scene->GetDispatch();
	return v.Detach(val);
}

HRESULT Entity::GetModel( VARIANT* val )
{
	iEntity* e = (iEntity*)this;
	CComVariant v;
	if (e->m_model)
		v = e->m_model->getDispatch();
	return v.Detach(val);
}

HRESULT Entity::GetParent( VARIANT* val )
{
	iEntity* e = (iEntity*)this;
	CComVariant v;
	if (e->m_parent)
		v = e->m_parent->GetDispatch();
	return v.Detach(val);
}

HRESULT Entity::GetParentIndex( VARIANT* val )
{
	iEntity* e = (iEntity*)this;
	CComVariant v = e->TList<iEntity, cookie_sibling>::getIndex();
	return v.Detach(val);
}

HRESULT Entity::GetChildren( VARIANT* val )
{
	CDispatchArray* pArr = CDispatchArray::New(val);
	if (pArr==NULL) return E_OUTOFMEMORY;
	iEntity* e = (iEntity*)this;
	iEntity* child = e->m_children;
	while (child)
	{
		pArr->Add(child->GetDispatch(), child->m_Name);
		child = child->m_siblings;
	}
	return S_OK;
}
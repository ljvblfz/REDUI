#include <atlbase.h>
#include <atltypes.h>
//#include <atlapp.h>
//#include <atlgdi.h>
#include <vssym32.h>
#include <new>

#include "theme.h"

/* static */
const CThemeMgr* CThemeMgr::instance()
{
	// The global CThemeMgr instance.
	static const CThemeMgr s_native_theme;
	return &s_native_theme;
}

CThemeMgr::CThemeMgr() : theme_dll_(::LoadLibraryW(L"uxtheme.dll")),
	draw_theme_(NULL),
	draw_theme_ex_(NULL),
	get_theme_color_(NULL),
	get_theme_content_rect_(NULL),
	get_theme_part_size_(NULL),
	open_theme_(NULL),
	close_theme_(NULL),
	set_theme_properties_(NULL),
	is_theme_active_(NULL),
	get_theme_int_(NULL)
{
	if (theme_dll_)
	{
		draw_theme_ = reinterpret_cast<DrawThemeBackgroundPtr>(GetProcAddress(theme_dll_, "DrawThemeBackground"));
		draw_theme_ex_ = reinterpret_cast<DrawThemeBackgroundExPtr>(GetProcAddress(theme_dll_, "DrawThemeBackgroundEx"));
		get_theme_color_ = reinterpret_cast<GetThemeColorPtr>(GetProcAddress(theme_dll_, "GetThemeColor"));
		get_theme_content_rect_ = reinterpret_cast<GetThemeContentRectPtr>(GetProcAddress(theme_dll_, "GetThemeBackgroundContentRect"));
		get_theme_part_size_ = reinterpret_cast<GetThemePartSizePtr>(GetProcAddress(theme_dll_, "GetThemePartSize"));
		open_theme_ = reinterpret_cast<OpenThemeDataPtr>(GetProcAddress(theme_dll_, "OpenThemeData"));
		close_theme_ = reinterpret_cast<CloseThemeDataPtr>(GetProcAddress(theme_dll_, "CloseThemeData"));
		set_theme_properties_ = reinterpret_cast<SetThemeAppPropertiesPtr>(GetProcAddress(theme_dll_, "SetThemeAppProperties"));
		is_theme_active_ = reinterpret_cast<IsThemeActivePtr>(GetProcAddress(theme_dll_, "IsThemeActive"));
		get_theme_int_ = reinterpret_cast<GetThemeIntPtr>(GetProcAddress(theme_dll_, "GetThemeInt"));
	}
	memset(theme_handles_, 0, sizeof(theme_handles_));
}

CThemeMgr::~CThemeMgr()
{
	if (theme_dll_)
	{
		// todo (cpu): fix this soon.
		// CloseHandles();
		::FreeLibrary(theme_dll_);
		theme_dll_ = NULL;
	}
}

HRESULT CThemeMgr::PaintTheme(ThemeName theme_name, HDC hdc, int part_id, int state_id, const RECT* rect) const
{
	HANDLE handle = GetThemeHandle(theme_name);
	if (handle && draw_theme_)
		return draw_theme_(handle, hdc, part_id, state_id, rect, NULL);
	return E_NOTIMPL;
}

HRESULT CThemeMgr::PaintButton(HDC hdc, int part_id, int state_id, int classic_state, RECT* rect) const
{
	HANDLE handle = GetThemeHandle(BUTTON);
	if (handle && draw_theme_)
		return draw_theme_(handle, hdc, part_id, state_id, rect, NULL);

	// Draw it manually.
	// All pressed states have both low bits set, and no other states do.
	const bool focused = ((state_id & PBS_PRESSED) == PBS_PRESSED);
	if ((part_id == BP_PUSHBUTTON) && focused)
	{
		// BP_PUSHBUTTON has a focus rect drawn around the outer edge, and the
		// button itself is shrunk by 1 pixel.
		HBRUSH brush = ::GetSysColorBrush(COLOR_3DDKSHADOW);
		if (brush)
		{
			::FrameRect(hdc, rect, brush);
			::InflateRect(rect, -1, -1);
		}
	}

	::DrawFrameControl(hdc, rect, DFC_BUTTON, classic_state);

	// BP_RADIOBUTTON, BP_CHECKBOX, BP_GROUPBOX and BP_USERBUTTON have their
	// focus drawn over the control.
	if ((part_id != BP_PUSHBUTTON) && focused)
		::DrawFocusRect(hdc, rect);

	return S_OK;
}

HRESULT CThemeMgr::PaintTextField(HDC hdc, int part_id, int state_id, int classic_state, RECT* rect, COLORREF color, bool fill_content_area, bool draw_edges) const
{
	HANDLE handle = GetThemeHandle(TEXTFIELD);
	// TODO(mpcomplete): can we detect if the color is specified by the user,
	// and if not, just use the system color?
	// CreateSolidBrush() accepts a RGB value but alpha must be 0.
	HBRUSH bg_brush = ::CreateSolidBrush(color);
	HRESULT hr;
	// DrawThemeBackgroundEx was introduced in XP SP2, so that it's possible
	// draw_theme_ex_ is NULL and draw_theme_ is non-null.
	if (handle && (draw_theme_ex_ || (draw_theme_ && draw_edges)))
	{
		if (draw_theme_ex_)
		{
			static DTBGOPTS omit_border_options = {sizeof(DTBGOPTS), DTBG_OMITBORDER, {0,0,0,0}};
			DTBGOPTS* draw_opts = draw_edges ? NULL : &omit_border_options;
			hr = draw_theme_ex_(handle, hdc, part_id, state_id, rect, draw_opts);
		}
		else
			hr = draw_theme_(handle, hdc, part_id, state_id, rect, NULL);

		// TODO(maruel): Need to be fixed if get_theme_content_rect_ is NULL.
		if (fill_content_area && get_theme_content_rect_)
		{
			RECT content_rect;
			hr = get_theme_content_rect_(handle, hdc, part_id, state_id, rect, &content_rect);
			::FillRect(hdc, &content_rect, bg_brush);
		}
	}
	else
	{
		// Draw it manually.
		if (draw_edges)
			::DrawEdge(hdc, rect, EDGE_SUNKEN, BF_RECT | BF_ADJUST);

		if (fill_content_area)
			::FillRect(hdc, rect, (classic_state & DFCS_INACTIVE) ? reinterpret_cast<HBRUSH>(COLOR_BTNFACE + 1) : bg_brush);
		hr = S_OK;
	}
	DeleteObject(bg_brush);
	return hr;
}

HRESULT CThemeMgr::PaintMenuList(HDC hdc, int part_id, int state_id, int classic_state, RECT* rect) const
{
	HANDLE handle = GetThemeHandle(MENULIST);
	if (handle && draw_theme_)
		return draw_theme_(handle, hdc, part_id, state_id, rect, NULL);

	// Draw it manually.
	::DrawFrameControl(hdc, rect, DFC_SCROLL, DFCS_SCROLLCOMBOBOX | classic_state);
	return S_OK;
}

HRESULT CThemeMgr::PaintScrollbarArrow(HDC hdc, int state_id, int classic_state, RECT* rect) const
{
	HANDLE handle = GetThemeHandle(SCROLLBAR);
	if (handle && draw_theme_)
		return draw_theme_(handle, hdc, SBP_ARROWBTN, state_id, rect, NULL);

	// Draw it manually.
	::DrawFrameControl(hdc, rect, DFC_SCROLL, classic_state);
	return S_OK;
}

HRESULT CThemeMgr::PaintScrollbarTrack(HDC hdc, int part_id, int state_id, int classic_state, RECT* target_rect) const
{
	HANDLE handle = GetThemeHandle(SCROLLBAR);
	if (handle && draw_theme_)
		return draw_theme_(handle, hdc, part_id, state_id, target_rect, NULL);

	// Draw it manually.
	const DWORD colorScrollbar = ::GetSysColor(COLOR_SCROLLBAR);
	const DWORD color3DFace = ::GetSysColor(COLOR_3DFACE);
	//if ((colorScrollbar != color3DFace) && (colorScrollbar != ::GetSysColor(COLOR_WINDOW)))
	::FillRect(hdc, target_rect, reinterpret_cast<HBRUSH>(COLOR_SCROLLBAR + 1));

	if (classic_state & DFCS_PUSHED)
		::InvertRect(hdc, target_rect);
	return S_OK;
}

HRESULT CThemeMgr::PaintScrollbarThumb(HDC hdc, int part_id, int state_id, int classic_state, RECT* rect) const
{
	HANDLE handle = GetThemeHandle(SCROLLBAR);
	if (handle && draw_theme_)
		return draw_theme_(handle, hdc, part_id, state_id, rect, NULL);

	// Draw it manually.
	if ((part_id == SBP_THUMBBTNHORZ) || (part_id == SBP_THUMBBTNVERT))
		::DrawEdge(hdc, rect, EDGE_RAISED, BF_RECT | BF_MIDDLE);
	// Classic mode doesn't have a gripper.
	return S_OK;
}

HRESULT CThemeMgr::PaintStatusGripper(HDC hdc, int part_id, int state_id, int classic_state, RECT* rect) const
{
	HANDLE handle = GetThemeHandle(STATUS);
	if (handle && draw_theme_)
	{
		// Paint the status bar gripper.  There doesn't seem to be a
		// standard gripper in Windows for the space between
		// scrollbars.  This is pretty close, but it's supposed to be
		// painted over a status bar.
		return draw_theme_(handle, hdc, SP_GRIPPER, 0, rect, 0);
	}

	// Draw a windows classic scrollbar gripper.
	::DrawFrameControl(hdc, rect, DFC_SCROLL, DFCS_SCROLLSIZEGRIP);
	return S_OK;
}

HRESULT CThemeMgr::PaintDialogBackground(HDC hdc, bool active, RECT* rect) const
{
	HANDLE handle = GetThemeHandle(WINDOW);
	if (handle && draw_theme_)
		return draw_theme_(handle, hdc, WP_DIALOG, active ? FS_ACTIVE : FS_INACTIVE, rect, NULL);

	// Classic just renders a flat color background.
	::FillRect(hdc, rect, reinterpret_cast<HBRUSH>(COLOR_3DFACE + 1));
	return S_OK;
}

HRESULT CThemeMgr::PaintTabPanelBackground(HDC hdc, RECT* rect) const
{
	HANDLE handle = GetThemeHandle(TAB);
	if (handle && draw_theme_)
		return draw_theme_(handle, hdc, TABP_BODY, 0, rect, NULL);

	// Classic just renders a flat color background.
	::FillRect(hdc, rect, reinterpret_cast<HBRUSH>(COLOR_3DFACE + 1));
	return S_OK;
}

HRESULT CThemeMgr::PaintListBackground(HDC hdc,
										 bool enabled,
										 RECT* rect) const
{
	HANDLE handle = GetThemeHandle(LIST);
	if (handle && draw_theme_)
		return draw_theme_(handle, hdc, 1, TS_NORMAL, rect, NULL);

	// Draw it manually.
	HBRUSH bg_brush = ::GetSysColorBrush(COLOR_WINDOW);
	::FillRect(hdc, rect, bg_brush);
	::DrawEdge(hdc, rect, EDGE_SUNKEN, BF_RECT | BF_ADJUST);
	return S_OK;
}

bool CThemeMgr::IsThemingActive() const
{
	if (is_theme_active_)
		return !!is_theme_active_();
	return false;
}

HRESULT CThemeMgr::PaintMenuArrow(ThemeName theme, HDC hdc, int part_id, int state_id, RECT* rect, MenuArrowDirection arrow_direction, bool is_highlighted) const
{
	HANDLE handle = GetThemeHandle(MENU);
	if (handle && draw_theme_)
	{
		if (arrow_direction == RIGHT_POINTING_ARROW)
			return draw_theme_(handle, hdc, part_id, state_id, rect, NULL);
		else
		{
			// There is no way to tell the uxtheme API to draw a left pointing arrow;
			// it doesn't have a flag equivalent to DFCS_MENUARROWRIGHT.  But they
			// are needed for RTL locales on Vista.  So use a memory DC and mirror
			// the region with GDI's StretchBlt.
			CRect r(*rect);
			HDC hmemdc = ::CreateCompatibleDC(hdc);
			HBITMAP hbmp = ::CreateCompatibleBitmap(hdc, r.right - r.left, r.bottom - r.top);
			HBITMAP hbmpOld = (HBITMAP)::SelectObject(hmemdc, hbmp);
			::SetViewportOrgEx(hmemdc, -r.left, -r.top, NULL);


			// Copy and horizontally mirror the background from hdc into mem_dc. Use
			// a negative-width source rect, starting at the rightmost pixel.
			::StretchBlt(hmemdc, 0, 0, r.Width(), r.Height(), hdc, r.right-1, r.top, -r.Width(), r.Height(), SRCCOPY);
			// Draw the arrow.
			RECT theme_rect = {0, 0, r.Width(), r.Height()};
			HRESULT result = draw_theme_(handle, hmemdc, part_id, state_id, &theme_rect, NULL);
			// Copy and mirror the result back into mem_dc.
			::StretchBlt(hdc, r.left, r.top, r.Width(), r.Height(),
				hmemdc, r.Width()-1, 0, -r.Width(), r.Height(), SRCCOPY);

			// 跳过 CMemoryDC 的析构函数，但是要清理数据成员
			::SelectObject(hmemdc, hbmpOld);
			::DeleteObject(hbmp);
			::DeleteDC(hmemdc);

			return result;
		}
	}

	// For some reason, Windows uses the name DFCS_MENUARROWRIGHT to indicate a
	// left pointing arrow. This makes the following 'if' statement slightly
	// counterintuitive.
	UINT state;
	if (arrow_direction == RIGHT_POINTING_ARROW)
		state = DFCS_MENUARROW;
	else
		state = DFCS_MENUARROWRIGHT;
	return PaintFrameControl(hdc, rect, DFC_MENU, state, is_highlighted);
}

HRESULT CThemeMgr::PaintMenuBackground(ThemeName theme, HDC hdc, int part_id, int state_id, RECT* rect) const
{
	HANDLE handle = GetThemeHandle(MENU);
	if (handle && draw_theme_)
	{
		HRESULT result = draw_theme_(handle, hdc, part_id, state_id, rect, NULL);
		::FrameRect(hdc, rect, GetSysColorBrush(COLOR_3DSHADOW));
		return result;
	}

	::FillRect(hdc, rect, GetSysColorBrush(COLOR_MENU));
	::DrawEdge(hdc, rect, EDGE_RAISED, BF_RECT);
	return S_OK;
}

HRESULT CThemeMgr::PaintMenuCheckBackground(ThemeName theme, HDC hdc, int part_id, int state_id, RECT* rect) const
{
	HANDLE handle = GetThemeHandle(MENU);
	if (handle && draw_theme_)
		return draw_theme_(handle, hdc, part_id, state_id, rect, NULL);
	// Nothing to do for background.
	return S_OK;
}

HRESULT CThemeMgr::PaintMenuCheck(ThemeName theme, HDC hdc, int part_id, int state_id, RECT* rect, bool is_highlighted) const
{
	HANDLE handle = GetThemeHandle(MENU);
	if (handle && draw_theme_)
		return draw_theme_(handle, hdc, part_id, state_id, rect, NULL);
	return PaintFrameControl(hdc, rect, DFC_MENU, DFCS_MENUCHECK, is_highlighted);
}

HRESULT CThemeMgr::PaintMenuGutter(HDC hdc, int part_id, int state_id, RECT* rect) const
{
	HANDLE handle = GetThemeHandle(MENU);
	if (handle && draw_theme_)
		return draw_theme_(handle, hdc, part_id, state_id, rect, NULL);
	return E_NOTIMPL;
}

HRESULT CThemeMgr::PaintMenuSeparator(HDC hdc, int part_id, int state_id, RECT* rect) const
{
	HANDLE handle = GetThemeHandle(MENU);
	if (handle && draw_theme_)
		return draw_theme_(handle, hdc, part_id, state_id, rect, NULL);
	::DrawEdge(hdc, rect, EDGE_ETCHED, BF_TOP);
	return S_OK;
}

HRESULT CThemeMgr::PaintMenuItemBackground(ThemeName theme, HDC hdc, int part_id, int state_id, bool selected, RECT* rect) const
{
	HANDLE handle = GetThemeHandle(MENU);
	if (handle && draw_theme_)
		return draw_theme_(handle, hdc, part_id, state_id, rect, NULL);
	if (selected)
		::FillRect(hdc, rect, GetSysColorBrush(COLOR_HIGHLIGHT));
	return S_OK;
}

HRESULT CThemeMgr::GetThemePartSize(ThemeName theme_name, HDC hdc, int part_id, int state_id, RECT* rect, int ts, SIZE* size) const
{
	HANDLE handle = GetThemeHandle(theme_name);
	if (handle && get_theme_part_size_)
		return get_theme_part_size_(handle, hdc, part_id, state_id, rect, ts, size);

	return E_NOTIMPL;
}

HRESULT CThemeMgr::GetThemeColor(ThemeName theme, int part_id, int state_id, int prop_id, COLORREF* color) const
{
	HANDLE handle = GetThemeHandle(theme);
	if (handle && get_theme_color_) {
		COLORREF color_ref;
		if (get_theme_color_(handle, part_id, state_id, prop_id, &color_ref) == S_OK)
			return *color=color_ref, S_OK;
	}
	return E_NOTIMPL;
}

COLORREF CThemeMgr::GetThemeColorWithDefault(ThemeName theme, int part_id, int state_id, int prop_id, int default_sys_color) const
{
	COLORREF color;
	if (GetThemeColor(theme, part_id, state_id, prop_id, &color) != S_OK)
		color = ::GetSysColor(default_sys_color);
	return color;
}

HRESULT CThemeMgr::GetThemeInt(ThemeName theme, int part_id, int state_id, int prop_id, int *value) const
{
	HANDLE handle = GetThemeHandle(theme);
	if (handle && get_theme_int_)
		return get_theme_int_(handle, part_id, state_id, prop_id, value);
	return E_NOTIMPL;
}

SIZE CThemeMgr::GetThemeBorderSize(ThemeName theme) const
{
	// For simplicity use the wildcard state==0, part==0, since it works
	// for the cases we currently depend on.
	int border;
	if (GetThemeInt(theme, 0, 0, TMT_BORDERSIZE, &border) == S_OK)
		return CSize(border, border);
	else
		return CSize(::GetSystemMetrics(SM_CXEDGE), ::GetSystemMetrics(SM_CYEDGE));
}


void CThemeMgr::DisableTheming() const
{
	if (!set_theme_properties_)
		return;
	set_theme_properties_(0);
}

HRESULT CThemeMgr::PaintFrameControl(HDC hdc, RECT* rect, UINT type, UINT state, bool is_highlighted) const
{
	const int width = rect->right - rect->left;
	const int height = rect->bottom - rect->top;

	// We're going to use BitBlt with a b&w mask. This results in using the dest
	// dc's text color for the black bits in the mask, and the dest dc's
	// background color for the white bits in the mask. DrawFrameControl draws the
	// check in black, and the background in white.
	COLORREF old_bg_color = SetBkColor(hdc, GetSysColor(is_highlighted ? COLOR_HIGHLIGHT : COLOR_MENU));
	COLORREF old_text_color = SetTextColor(hdc, GetSysColor(is_highlighted ? COLOR_HIGHLIGHTTEXT : COLOR_MENUTEXT));
	::DrawFrameControl(hdc, rect, type, state);
	::SetBkColor(hdc, old_bg_color);
	::SetTextColor(hdc, old_text_color);

	return S_OK;
}

void CThemeMgr::CloseHandles() const
{
	if (!close_theme_)
		return;

	for (int i = 0; i < LAST; ++i) {
		if (theme_handles_[i])
			close_theme_(theme_handles_[i]);
		theme_handles_[i] = NULL;
	}
}

HANDLE CThemeMgr::GetThemeHandle(ThemeName theme_name) const
{
	if (!open_theme_ || theme_name < 0 || theme_name >= LAST)
		return 0;

	if (theme_handles_[theme_name])
		return theme_handles_[theme_name];

	// Not found, try to load it.
	HANDLE handle = 0;
#define theme_item(x) case CThemeMgr::x: handle = open_theme_(NULL, L#x); break;
	switch (theme_name)
	{
		__foreach_theme(theme_item);
	default:
		ATLASSERT(FALSE);
	}
#undef theme_item

	theme_handles_[theme_name] = handle;
	return handle;
}

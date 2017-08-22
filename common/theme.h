#ifndef __THEME_H__
#define __THEME_H__

#pragma once

#include <windows.h>
#include <uxtheme.h>
#include <vsstyle.h>

#define __foreach_theme(V) \
	V(BUTTON)  V(CLOCK)  V(COMBOBOX)  V(COMMUNICATIONS)  V(CONTROLPANEL)  V(DATEPICKER)  V(DRAGDROP) \
	V(EDIT)  V(EXPLORERBAR)  V(FLYOUT)  V(GLOBALS)  V(HEADER)  V(LISTBOX)  V(LISTVIEW)  V(MENU) \
	V(MENUBAND)  V(NAVIGATION)  V(PAGE)  V(PROGRESS)  V(REBAR)  V(SCROLLBAR)  V(SPIN)  V(STARTPANEL) \
	V(STATUS)  V(TAB)  V(TASKBAND)  V(TASKBAR)  V(TASKDIALOG)  V(TEXTSTYLE)  V(TOOLBAR)  V(TOOLTIP) \
	V(TRACKBAR)  V(TRAYNOTIFY)  V(TREEVIEW)  V(WINDOW)

class CThemeMgr
{
public:
#define theme_item(x) x,
	enum ThemeName
	{
		__foreach_theme(theme_item)

		TEXTFIELD = EDIT,
		MENULIST = COMBOBOX,
		LIST = LISTVIEW,
		LAST = WINDOW + 1
	};
#undef theme_item

	// This enumeration is used within PaintMenuArrow in order to indicate the
	// direction the menu arrow should point to.
	enum MenuArrowDirection
	{
		LEFT_POINTING_ARROW,
		RIGHT_POINTING_ARROW
	};

	typedef HRESULT (WINAPI* DrawThemeBackgroundPtr)(HANDLE theme, HDC hdc, int part_id, int state_id, const RECT* rect, const RECT* clip_rect);
	typedef HRESULT (WINAPI* DrawThemeBackgroundExPtr)(HANDLE theme, HDC hdc, int part_id, int state_id, const RECT* rect, const DTBGOPTS* opts);
	typedef HRESULT (WINAPI* GetThemeColorPtr)(HANDLE hTheme, int part_id, int state_id, int prop_id, COLORREF* color);
	typedef HRESULT (WINAPI* GetThemeContentRectPtr)(HANDLE hTheme, HDC hdc, int part_id, int state_id, const RECT* rect, RECT* content_rect);
	typedef HRESULT (WINAPI* GetThemePartSizePtr)(HANDLE hTheme, HDC hdc, int part_id, int state_id, RECT* rect, int ts, SIZE* size);
	typedef HANDLE (WINAPI* OpenThemeDataPtr)(HWND window, LPCWSTR class_list);
	typedef HRESULT (WINAPI* CloseThemeDataPtr)(HANDLE theme);

	typedef void (WINAPI* SetThemeAppPropertiesPtr) (DWORD flags);
	typedef BOOL (WINAPI* IsThemeActivePtr)();
	typedef HRESULT (WINAPI* GetThemeIntPtr)(HANDLE hTheme, int part_id, int state_id, int prop_id, int *value);

	HRESULT PaintTheme(ThemeName theme_name, HDC hdc, int part_id, int state_id, const RECT* rect) const;

	HRESULT PaintButton(HDC hdc, int part_id, int state_id, int classic_state, RECT* rect) const;

	HRESULT PaintTextField(HDC hdc, int part_id, int state_id, int classic_state, RECT* rect, COLORREF color, bool fill_content_area, bool draw_edges) const;

	HRESULT PaintMenuList(HDC hdc, int part_id, int state_id, int classic_state, RECT* rect) const;

	// Paints a scrollbar arrow.  |classic_state| should have the appropriate
	// classic part number ORed in already.
	HRESULT PaintScrollbarArrow(HDC hdc, int state_id, int classic_state, RECT* rect) const;

	// Paints a scrollbar track section.  |align_rect| is only used in classic
	// mode, and makes sure the checkerboard pattern in |target_rect| is aligned
	// with one presumed to be in |align_rect|.
	HRESULT PaintScrollbarTrack(HDC hdc, int part_id, int state_id, int classic_state, RECT* target_rect) const;

	// |arrow_direction| determines whether the arrow is pointing to the left or
	// to the right. In RTL locales, sub-menus open from right to left and
	// therefore the menu arrow should point to the left and not to the right.
	HRESULT PaintMenuArrow(ThemeName theme, HDC hdc, int part_id, int state_id, RECT* rect, MenuArrowDirection arrow_direction, bool is_highlighted) const;

	HRESULT PaintMenuBackground(ThemeName theme, HDC hdc, int part_id, int state_id, RECT* rect) const;

	HRESULT PaintMenuCheck(ThemeName theme, HDC hdc, int part_id, int state_id, RECT* rect, bool is_highlighted) const;

	HRESULT PaintMenuCheckBackground(ThemeName theme, HDC hdc, int part_id, int state_id, RECT* rect) const;

	HRESULT PaintMenuGutter(HDC hdc, int part_id, int state_id, RECT* rect) const;

	HRESULT PaintMenuSeparator(HDC hdc, int part_id, int state_id, RECT* rect) const;

	HRESULT PaintMenuItemBackground(ThemeName theme, HDC hdc, int part_id, int state_id, bool selected, RECT* rect) const;

	// Paints a scrollbar thumb or gripper.
	HRESULT PaintScrollbarThumb(HDC hdc, int part_id, int state_id, int classic_state, RECT* rect) const;

	HRESULT PaintStatusGripper(HDC hdc, int part_id, int state_id, int classic_state, RECT* rect) const;

	HRESULT PaintDialogBackground(HDC dc, bool active, RECT* rect) const;

	HRESULT PaintTabPanelBackground(HDC dc, RECT* rect) const;

	HRESULT PaintListBackground(HDC dc, bool enabled, RECT* rect) const;

	bool IsThemingActive() const;

	HRESULT GetThemePartSize(ThemeName themeName, HDC hdc, int part_id, int state_id, RECT* rect, int ts, SIZE* size) const;

	HRESULT GetThemeColor(ThemeName theme, int part_id, int state_id, int prop_id, COLORREF* color) const;

	// Get the theme color if theming is enabled.  If theming is unsupported
	// for this part, use Win32's GetSysColor to find the color specified
	// by default_sys_color.
	COLORREF GetThemeColorWithDefault(ThemeName theme, int part_id, int state_id, int prop_id, int default_sys_color) const;

	HRESULT GetThemeInt(ThemeName theme, int part_id, int state_id, int prop_id, int *result) const;

	// Get the thickness of the border associated with the specified theme,
	// defaulting to GetSystemMetrics edge size if themes are disabled.
	// In Classic Windows, borders are typically 2px; on XP+, they are 1px.
	SIZE GetThemeBorderSize(ThemeName theme) const;

	// Disables all theming for top-level windows in the entire process, from
	// when this method is called until the process exits.  All the other
	// methods in this class will continue to work, but their output will ignore
	// the user's theme. This is meant for use when running tests that require
	// consistent visual results.
	void DisableTheming() const;

	// Closes cached theme handles so we can unload the DLL or update our UI
	// for a theme change.
	void CloseHandles() const;

	// Gets our singleton instance.
	static const CThemeMgr* instance();

private:
	CThemeMgr();
	~CThemeMgr();

	HRESULT PaintFrameControl(HDC hdc, RECT* rect, UINT type, UINT state, bool is_highlighted) const;

	// Returns a handle to the theme data.
	HANDLE GetThemeHandle(ThemeName theme_name) const;

	// Function pointers into uxtheme.dll.
	DrawThemeBackgroundPtr draw_theme_;
	DrawThemeBackgroundExPtr draw_theme_ex_;
	GetThemeColorPtr get_theme_color_;
	GetThemeContentRectPtr get_theme_content_rect_;
	GetThemePartSizePtr get_theme_part_size_;
	OpenThemeDataPtr open_theme_;
	CloseThemeDataPtr close_theme_;
	SetThemeAppPropertiesPtr set_theme_properties_;
	IsThemeActivePtr is_theme_active_;
	GetThemeIntPtr get_theme_int_;

	// Handle to uxtheme.dll.
	HMODULE theme_dll_;

	// A cache of open theme handles.
	mutable HANDLE theme_handles_[LAST];
};

#define theme_ptr CThemeMgr::instance()
#define draw_theme theme_ptr->PaintTheme
#define draw_theme_scroll(hdc, partid, stateid, prect) draw_theme(CThemeMgr::SCROLLBAR, hdc, partid, stateid, prect)

#define minmax(v, minv, maxv) min(max(v, minv), maxv)

#endif // __THEME_H__
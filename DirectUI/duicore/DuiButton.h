#if !defined(__DUIBUTTON_H__)
#define __DUIBUTTON_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DuiManager.h"

/////////////////////////////////////////////////////////////////////////////////////
//

class  CDuiButton : public ControlExtension
{
public:
	DECLARE_CONTROL_EXTENSION(CDuiButton)
	DECLARE_CONTROL_FLAGS(DUIFLAG_TABSTOP)
	DECLARE_CLASS_CAST(CDuiButton, ControlExtension)

	BEGIN_CLASS_STYLE_MAP("text-style:singleline center middle; padding:2; overflow:hidden; visual:button")
		//STATE_STYLE(DUISTATE_HOT, "background:btnhighlight")
	END_CLASS_STYLE_MAP()

	virtual BOOL Activate(HDE hde);

	//void SetText(LPCOLESTR pstrText);
};

THIS_IS_CONTROL_EXTENSION(CDuiButton, Button)


/////////////////////////////////////////////////////////////////////////////////////
//

class  CDuiOption : public CDuiButton // 从CDuiButton派生的好处是能重用它的默认样式
{
public:
	DECLARE_CONTROL_EXTENSION(CDuiOption)
	DECLARE_CONTROL_FLAGS(DUIFLAG_TABSTOP)
	DECLARE_CLASS_CAST(CDuiOption, ControlExtension)

	DECLARE_BASE_CLASS_STYLE("text-style:singleline left middle; visual:option;")

	BOOL Activate(HDE hde);

	BOOL IsChecked();
	void SetCheck(BOOL bSelected);
	void SetAttribute(HDE hde, LPCWSTR szName, VARIANT* value);
};

THIS_IS_CONTROL_EXTENSION(CDuiOption, Option)


/////////////////////////////////////////////////////////////////////////////////////
//

class  CDuiCheckBox : public CDuiOption
{
public:
	DECLARE_CONTROL_EXTENSION(CDuiCheckBox)
	DECLARE_CONTROL_FLAGS(DUIFLAG_TABSTOP)
	DECLARE_CLASS_CAST(CDuiCheckBox, ControlExtension)

	DECLARE_BASE_CLASS_STYLE("visual:checkbox;")

	BOOL Activate(HDE hde);

	void SetCheck(BOOL bSelected);
};

THIS_IS_CONTROL_EXTENSION(CDuiCheckBox, CheckBox)


/////////////////////////////////////////////////////////////////////////////////////
//

class  CDuiGroupBox : public ControlExtension
{
public:
	DECLARE_CONTROL_EXTENSION(CDuiGroupBox)
	DECLARE_CLASS_CAST(CDuiGroupBox, ControlExtension)

	DECLARE_BASE_CLASS_STYLE("visual:groupbox")
};

THIS_IS_CONTROL_EXTENSION(CDuiGroupBox, GroupBox)


#endif // !defined(__DUIBUTTON_H__)


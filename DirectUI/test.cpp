//#include "../Raptor/dispatchimpl.h"
//#include "stdafx.h"
#include "../common/dispatchimpl.h"
#include "../common/macro.h"
//#include "D:\work\source\RapidIE\common\macro - 副本 (2).h"

//#define LL L
//#define LLL(...) L#__VA_ARGS__##L""
////#define LLLL(m, x) m##x
////#define LLLLL(...)
////#define LLLLLL(x) x##
//#define DCN(class, ...) LPCOLESTR class = L"" LLL(__VA_ARGS__);
////DCN(var1)
////DCN(var2, abc)
//#define XX(...) L#__VA_ARGS__
//#define X() L""
////#define LX X
//#define LX() L""
//#define CC(m,n) m##n
////#define XXX(...) XX(__VA_ARGS__)##X()
//#define YYY(...) "" #__VA_ARGS__
//#define XXX(...) OLESTR(YYY(__VA_ARGS__))
//LPCOLESTR test = XXX();
//LPCOLESTR test2 = XXX(sss);
//
//LPCOLESTR test3 = L"";
//LPCOLESTR test4 = L"eee"L"";

//__method_helper_t(0)

#define __foreach(v, ...) \
	v(0, __VA_ARGS__) \
	v(1, __VA_ARGS__) \
	v(2, __VA_ARGS__) \
	v(3, __VA_ARGS__) 
	//v##(4, __VA_ARGS__) \
	//v##(5, __VA_ARGS__) \
	//v##(6, __VA_ARGS__) \
	//v##(7, __VA_ARGS__) \
	//v##(8, __VA_ARGS__) \
	//v##(9, __VA_ARGS__) \
	//v##(10, __VA_ARGS__) \
	//v##(11, __VA_ARGS__) \
	//v##(12, __VA_ARGS__) \
	//v##(13, __VA_ARGS__) \
	//v##(14, __VA_ARGS__) \
	//v##(15, __VA_ARGS__) \

#define ___repeat_test(n, m,  ...) ___connect(___repeat2d_, ___cntdec(n))(m, n, __VA_ARGS__) ___connect(__last_repeat2d_, n)(m, n, __VA_ARGS__)

#define ___foreach_sub(n, v, ...) ___repeat2d(n, v, n, __VA_ARGS__)

#define __foreach2(v, ...) __foreach(___foreach_sub, v, __VA_ARGS__)

#define xxx(n, n1, ...) __pragma(message( "mmm___" #n "_" #n1 "_" __to_string(___cntmixed(n,n1))  ))
#define mmm(n, nOpt, ...) \
	template<class TT, typename rettype, ___repeat(n, __tparam, __tparam)  rettype (TT::* func)(___repeat(n, __funcparam_type, __funcparam_type_end)) > \
	class _Test_##n##_##nOpt \
	{ \
	public: \
	static HRESULT CallMethod (LPVOID pT, DISPPARAMS* dp, VARIANT* pvarResult, DISPID dispid, LPCOLESTR name) \
	{ \
		if (pT==NULL) return E_FAIL; \
		if (dp->cArgs < n-nOpt) return DISP_E_BADPARAMCOUNT; \
		CComVariant vRet; \
		switch (n - dp->cArgs) \
		{ \
		___repeat(nOpt, __funcparamopt, __funcparamopt, n) \
		} \
		if (pvarResult && vRet.vt!=VT_EMPTY) vRet.Detach(pvarResult); \
		return S_OK; \
	} \
	}; 

//___repeat_test(3, xxx)
//__foreach(xxx, 15)
//___for_each_number(xxx, 15)
//__foreach2(mmm)
//____for_each_number2(mmm)
//xxx(5, 4)
//___for_each_number2d(mmm)

//__method_opt(3,1)

//___for_each_number2(__method_helper_opt_t)
//___for_each_number2(xxx)
//___for_each_number2(mmm)
//___for_each_number(mmm,15)
//___ 
//void ff(int i=0){}
//class __func
//{
//public:
//	void operator()() { ff(); }
//	void operator()(int i) { ff(i); }
//};
//typedef void (pfff)(int i=0);
//void bar()
//{
//	//__func pf;
//	pfff pf = ff;
//	pf();
//}

//class myclass
//{
//public:
//	long a;
//	//CComBSTR name;
//	void test(long l, float) {return ;}
//
//	Begin_Disp_Map(myclass)
//		//Disp_Property(1, b, long)
//		////Disp_Property(10, c, long)
//		//Disp_Property(2, name, CComBSTR)
//		Disp_Method(3, test, void, 2, long,float)
//	End_Disp_Map()
//
//	//myclass() : a(3) {}
//
//	//HRESULT Getb(VARIANT*) { return S_OK; }
//	//HRESULT Putb(VARIANT*) { return S_OK; }
//};


//myclass my;
//
//struct xxx
//{
//	xxx()
//	{
//		CComPtr<IDispatch> disp = my.GetDispatch();
//		disp.PutPropertyByName(L"a", &CComVariant(123));
//		disp.PutPropertyByName(L"name", &CComVariant(L"haha"));
//		disp;
//	}
//} _xxx;
//class CCC
//{
//public:
//	int __i;
//};
//class CAnimal
//{
//public:
//	HRESULT GetSex(VARIANT* pvar) { ATLTRACE(L"CAnimal::GetSex() %08x\n", (LPVOID)this); V_VT(pvar)=VT_I4; V_I4(pvar)=33; return S_OK; }
//
//	Begin_Disp_Map(CAnimal)
//		Disp_PropertyGet(10, Sex)
//	End_Disp_Map()
//};
//
//class CDog : public CCC, public CAnimal
//{
//public:
//	CComBSTR Name;
//	long Age;
//
//	void Drink() { ATLTRACE(L"CDog::Drink() %08x\n", (LPVOID)this); }
//	bool Eat(long lType, long lNum=0, float=0.f, long=4) {return true;}
//
//	Begin_Method_Params_Map(CDog)
//		Method_Params(Eat, 4, 3, bool, long, long, float, long)
//	End_Method_Params_Map()
//
//	Begin_Disp_Map(CDog, CAnimal, CCC)
//		Disp_Property(1, Name, CComBSTR)
//		Disp_PropertyGet(2, Age, long)
//		Disp_Method(3, Drink, void, 0)
//		Disp_Method(4, Eat, 0,0)
//	End_Disp_Map()
//};
//
//CDog dog;
////CComPtr<IDispatch> disp = dog.GetDispatch();
////disp.Invoke0(OLESTR("Drink")); // 这句自动化调用将导致 CDog::Drink() 函数被执行
//
//struct xxx
//{
//	xxx()
//	{
//		CComDispatchDriver disp = dog.GetDispatch();
//		CComVariant v;
//		disp.Invoke0(L"Drink");
//		disp.GetPropertyByName(L"Sex", &v);
//		disp.PutPropertyByName(L"a", &CComVariant(123));
//		disp.PutPropertyByName(L"name", &CComVariant(L"haha"));
//	}
//} _xxx;

//inline HRESULT ATL::CComPtr<IDispatch>::ThisInvoke(IDispatch* pThis, DISPID dispid, VARIANT* pvarParams, int nParams, VARIANT* pvarRet = NULL)
//{
//	DISPPARAMS dispparams = { pvarParams, NULL, nParams, 0};
//	return p->Invoke(dispid, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &dispparams, pvarRet, NULL, NULL);
//}
//int test()
//{
//	const int aa[] = {
//		0,
//typedef	class 
//	{
//	public:
//		static int CallMethod(int) {return 2;}
//	}ttt;
//
//		1,2};
//return ttt::CallMethod(3);
//}

#include "duicore/DuiManager.h"


class CDuiMyControl : public ControlExtension
{
public:
	DECLARE_CONTROL_EXTENSION(CDuiMyControl)
	DECLARE_CLASS_CAST(CDuiMyControl, ControlExtension)

	BEGIN_CLASS_STYLE_MAP("color: blue; display:inlineblock")
		STATE_STYLE(DUISTATE_HOT, "color: red")
	END_CLASS_STYLE_MAP()
};

THIS_IS_CONTROL_EXTENSION(CDuiMyControl, MyCtrl)


//class base
//{
//private:
//	static void Abc() {}
//};
//
//class base2 : public base
//{
//private:
//	void test()
//	{
//		__if_exists(base2::Abc) { __pragma(message("has Abc")) }
//	}
//};
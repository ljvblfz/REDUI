#ifndef __MACRO_H__
#define __MACRO_H__

#pragma once

#ifndef __cplusplus
	#error macro.h requires C++ compilation (use a .cpp suffix)
#endif

#if (_MSC_VER < 1400)
	#error macro.h requires Visual C++ 2005 and above.
#endif 

#include <DispEx.h>
#include "dispatchimpl.h"

#pragma warning(push)
#pragma warning(disable:4800)


#ifndef NEW
#define NEW new
#endif // NEW

#ifndef DISPID_EXPANDO_BASE
#define DISPID_EXPANDO_BASE             3000000
#define DISPID_EXPANDO_MAX              3999999
#define IsExpandoDispid(dispid)         (DISPID_EXPANDO_BASE <= dispid && dispid <= DISPID_EXPANDO_MAX)
#endif // DISPID_EXPANDO_BASE

#ifndef offsetof
#define offsetof(s,m)   (size_t)&(((s *)0)->m)
#endif

//////////////////////////////////////////////////////////////////////////
// 基础工具宏定义

#define ___for_each_number(v, ...) \
	v(0, __VA_ARGS__) \
	v(1, __VA_ARGS__) \
	v(2, __VA_ARGS__) \
	v(3, __VA_ARGS__) \
	v(4, __VA_ARGS__) \
	v(5, __VA_ARGS__) \
	v(6, __VA_ARGS__) \
	v(7, __VA_ARGS__) \
	v(8, __VA_ARGS__) \
	v(9, __VA_ARGS__) \
	v(10, __VA_ARGS__) \
	v(11, __VA_ARGS__) \
	v(12, __VA_ARGS__) \
	v(13, __VA_ARGS__) \
	v(14, __VA_ARGS__) \
	v(15, __VA_ARGS__) \

#define ___for_each_number_base1(v, ...) \
	v(1, __VA_ARGS__) \
	v(2, __VA_ARGS__) \
	v(3, __VA_ARGS__) \
	v(4, __VA_ARGS__) \
	v(5, __VA_ARGS__) \
	v(6, __VA_ARGS__) \
	v(7, __VA_ARGS__) \
	v(8, __VA_ARGS__) \
	v(9, __VA_ARGS__) \
	v(10, __VA_ARGS__) \
	v(11, __VA_ARGS__) \
	v(12, __VA_ARGS__) \
	v(13, __VA_ARGS__) \
	v(14, __VA_ARGS__) \
	v(15, __VA_ARGS__)

#define ___for_each_number2d(v, ...) \
	v(0,0, __VA_ARGS__) \
	v(1,0, __VA_ARGS__)  v(1,1,__VA_ARGS__) \
	v(2,0, __VA_ARGS__)  v(2,1,__VA_ARGS__)  v(2,2,__VA_ARGS__) \
	v(3,0, __VA_ARGS__)  v(3,1,__VA_ARGS__)  v(3,2,__VA_ARGS__) v(3,3,__VA_ARGS__) \
	v(4,0, __VA_ARGS__)  v(4,1,__VA_ARGS__)  v(4,2,__VA_ARGS__) v(4,3,__VA_ARGS__) v(4,4,__VA_ARGS__) \
	v(5,0, __VA_ARGS__)  v(5,1,__VA_ARGS__)  v(5,2,__VA_ARGS__) v(5,3,__VA_ARGS__) v(5,4,__VA_ARGS__) v(5,5,__VA_ARGS__) \
	v(6,0, __VA_ARGS__)  v(6,1,__VA_ARGS__)  v(6,2,__VA_ARGS__) v(6,3,__VA_ARGS__) v(6,4,__VA_ARGS__) v(6,5,__VA_ARGS__) v(6,6,__VA_ARGS__) \
	v(7,0, __VA_ARGS__)  v(7,1,__VA_ARGS__)  v(7,2,__VA_ARGS__) v(7,3,__VA_ARGS__) v(7,4,__VA_ARGS__) v(7,5,__VA_ARGS__) v(7,6,__VA_ARGS__) \
						v(7,7,__VA_ARGS__) \
	v(8,0, __VA_ARGS__)  v(8,1,__VA_ARGS__)  v(8,2,__VA_ARGS__) v(8,3,__VA_ARGS__) v(8,4,__VA_ARGS__) v(8,5,__VA_ARGS__) v(8,6,__VA_ARGS__) \
						v(8,7,__VA_ARGS__) v(8,8,__VA_ARGS__) \
	v(9,0, __VA_ARGS__)  v(9,1,__VA_ARGS__)  v(9,2,__VA_ARGS__) v(9,3,__VA_ARGS__) v(9,4,__VA_ARGS__) v(9,5,__VA_ARGS__) v(9,6,__VA_ARGS__) \
						v(9,7,__VA_ARGS__) v(9,8,__VA_ARGS__) \v(9,9,__VA_ARGS__) \
	v(10,0, __VA_ARGS__) v(10,1,__VA_ARGS__) v(10,2,__VA_ARGS__) v(10,3,__VA_ARGS__) v(10,4,__VA_ARGS__) v(10,5,__VA_ARGS__) v(10,6,__VA_ARGS__) \
						v(10,7,__VA_ARGS__) v(10,8,__VA_ARGS__) v(10,9,__VA_ARGS__) v(10,10,__VA_ARGS__) \
	v(11,0, __VA_ARGS__) v(11,1,__VA_ARGS__) v(11,2,__VA_ARGS__) v(11,3,__VA_ARGS__) v(11,4,__VA_ARGS__) v(11,5,__VA_ARGS__) v(11,6,__VA_ARGS__) \
						v(11,7,__VA_ARGS__) v(11,8,__VA_ARGS__) v(11,9,__VA_ARGS__) v(11,10,__VA_ARGS__) v(11,11,__VA_ARGS__) \
	v(12,0, __VA_ARGS__) v(12,1,__VA_ARGS__) v(12,2,__VA_ARGS__) v(12,3,__VA_ARGS__) v(12,4,__VA_ARGS__) v(12,5,__VA_ARGS__) v(12,6,__VA_ARGS__) \
						v(12,7,__VA_ARGS__) v(12,8,__VA_ARGS__) v(12,9,__VA_ARGS__) v(12,10,__VA_ARGS__) v(12,11,__VA_ARGS__) v(12,12,__VA_ARGS__) \
	v(13,0, __VA_ARGS__) v(13,1,__VA_ARGS__) v(13,2,__VA_ARGS__) v(13,3,__VA_ARGS__) v(13,4,__VA_ARGS__) v(13,5,__VA_ARGS__) v(13,6,__VA_ARGS__) \
						v(13,7,__VA_ARGS__) v(13,8,__VA_ARGS__) v(13,9,__VA_ARGS__) v(13,10,__VA_ARGS__) v(13,11,__VA_ARGS__) v(13,12,__VA_ARGS__) \
						v(13,13,__VA_ARGS__) \
	v(14,0, __VA_ARGS__) v(14,1,__VA_ARGS__) v(14,2,__VA_ARGS__) v(14,3,__VA_ARGS__) v(14,4,__VA_ARGS__) v(14,5,__VA_ARGS__) v(14,6,__VA_ARGS__) \
						v(14,7,__VA_ARGS__) v(14,8,__VA_ARGS__) v(14,9,__VA_ARGS__) v(14,10,__VA_ARGS__) v(14,11,__VA_ARGS__) v(14,12,__VA_ARGS__) \
						v(14,13,__VA_ARGS__) v(14,14,__VA_ARGS__) \
	v(15,0, __VA_ARGS__) v(15,1,__VA_ARGS__) v(15,2,__VA_ARGS__) v(15,3,__VA_ARGS__) v(15,4,__VA_ARGS__) v(15,5,__VA_ARGS__) v(15,6,__VA_ARGS__) \
						v(15,7,__VA_ARGS__) v(15,8,__VA_ARGS__) v(15,9,__VA_ARGS__) v(15,10,__VA_ARGS__) v(15,11,__VA_ARGS__) v(15,12,__VA_ARGS__) \
						v(15,13,__VA_ARGS__) v(15,14,__VA_ARGS__) v(15,15,__VA_ARGS__)

// 数值减1的常数
#define __cntdec_0 0
#define __cntdec_1 0
#define __cntdec_2 1
#define __cntdec_3 2
#define __cntdec_4 3
#define __cntdec_5 4
#define __cntdec_6 5
#define __cntdec_7 6
#define __cntdec_8 7
#define __cntdec_9 8
#define __cntdec_10 9
#define __cntdec_11 10
#define __cntdec_12 11
#define __cntdec_13 12
#define __cntdec_14 13
#define __cntdec_15 14

#define __cntdec(n) __cntdec_##n

#define ___cntmixed_0_0	0
#define ___cntmixed_1_0	1
#define ___cntmixed_1_1	0
#define ___cntmixed_2_0	2
#define ___cntmixed_2_1	1
#define ___cntmixed_2_2	0
#define ___cntmixed_3_0	3
#define ___cntmixed_3_1	2
#define ___cntmixed_3_2	1
#define ___cntmixed_3_3	0
#define ___cntmixed_4_0	4
#define ___cntmixed_4_1	3
#define ___cntmixed_4_2	2
#define ___cntmixed_4_3	1
#define ___cntmixed_4_4	0
#define ___cntmixed_5_0	5
#define ___cntmixed_5_1	4
#define ___cntmixed_5_2	3
#define ___cntmixed_5_3	2
#define ___cntmixed_5_4	1
#define ___cntmixed_5_5	0
#define ___cntmixed_6_0	6
#define ___cntmixed_6_1	5
#define ___cntmixed_6_2	4
#define ___cntmixed_6_3	3
#define ___cntmixed_6_4	2
#define ___cntmixed_6_5	1
#define ___cntmixed_6_6	0
#define ___cntmixed_7_0	7
#define ___cntmixed_7_1	6
#define ___cntmixed_7_2	5
#define ___cntmixed_7_3	4
#define ___cntmixed_7_4	3
#define ___cntmixed_7_5	2
#define ___cntmixed_7_6	1
#define ___cntmixed_7_7	0
#define ___cntmixed_8_0	8
#define ___cntmixed_8_1	7
#define ___cntmixed_8_2	6
#define ___cntmixed_8_3	5
#define ___cntmixed_8_4	4
#define ___cntmixed_8_5	3
#define ___cntmixed_8_6	2
#define ___cntmixed_8_7	1
#define ___cntmixed_8_8	0
#define ___cntmixed_9_0	9
#define ___cntmixed_9_1	8
#define ___cntmixed_9_2	7
#define ___cntmixed_9_3	6
#define ___cntmixed_9_4	5
#define ___cntmixed_9_5	4
#define ___cntmixed_9_6	3
#define ___cntmixed_9_7	2
#define ___cntmixed_9_8	1
#define ___cntmixed_9_9	0
#define ___cntmixed_10_0	10
#define ___cntmixed_10_1	9
#define ___cntmixed_10_2	8
#define ___cntmixed_10_3	7
#define ___cntmixed_10_4	6
#define ___cntmixed_10_5	5
#define ___cntmixed_10_6	4
#define ___cntmixed_10_7	3
#define ___cntmixed_10_8	2
#define ___cntmixed_10_9	1
#define ___cntmixed_10_10	0
#define ___cntmixed_11_0	11
#define ___cntmixed_11_1	10
#define ___cntmixed_11_2	9
#define ___cntmixed_11_3	8
#define ___cntmixed_11_4	7
#define ___cntmixed_11_5	6
#define ___cntmixed_11_6	5
#define ___cntmixed_11_7	4
#define ___cntmixed_11_8	3
#define ___cntmixed_11_9	2
#define ___cntmixed_11_10	1
#define ___cntmixed_11_11	0
#define ___cntmixed_12_0	12
#define ___cntmixed_12_1	11
#define ___cntmixed_12_2	10
#define ___cntmixed_12_3	9
#define ___cntmixed_12_4	8
#define ___cntmixed_12_5	7
#define ___cntmixed_12_6	6
#define ___cntmixed_12_7	5
#define ___cntmixed_12_8	4
#define ___cntmixed_12_9	3
#define ___cntmixed_12_10	2
#define ___cntmixed_12_11	1
#define ___cntmixed_12_12	0
#define ___cntmixed_13_0	13
#define ___cntmixed_13_1	12
#define ___cntmixed_13_2	11
#define ___cntmixed_13_3	10
#define ___cntmixed_13_4	9
#define ___cntmixed_13_5	8
#define ___cntmixed_13_6	7
#define ___cntmixed_13_7	6
#define ___cntmixed_13_8	5
#define ___cntmixed_13_9	4
#define ___cntmixed_13_10	3
#define ___cntmixed_13_11	2
#define ___cntmixed_13_12	1
#define ___cntmixed_13_13	0
#define ___cntmixed_14_0	14
#define ___cntmixed_14_1	13
#define ___cntmixed_14_2	12
#define ___cntmixed_14_3	11
#define ___cntmixed_14_4	10
#define ___cntmixed_14_5	9
#define ___cntmixed_14_6	8
#define ___cntmixed_14_7	7
#define ___cntmixed_14_8	6
#define ___cntmixed_14_9	5
#define ___cntmixed_14_10	4
#define ___cntmixed_14_11	3
#define ___cntmixed_14_12	2
#define ___cntmixed_14_13	1
#define ___cntmixed_14_14	0
#define ___cntmixed_15_0	15
#define ___cntmixed_15_1	14
#define ___cntmixed_15_2	13
#define ___cntmixed_15_3	12
#define ___cntmixed_15_4	11
#define ___cntmixed_15_5	10
#define ___cntmixed_15_6	9
#define ___cntmixed_15_7	8
#define ___cntmixed_15_8	7
#define ___cntmixed_15_9	6
#define ___cntmixed_15_10	5
#define ___cntmixed_15_11	4
#define ___cntmixed_15_12	3
#define ___cntmixed_15_13	2
#define ___cntmixed_15_14	1
#define ___cntmixed_15_15	0

#define ___cntmixed(n,n2) ___cntmixed_##n##_##n2

// 连接两个符号
#define ___connect_i(x, y) x##y
#define ___connect(x, y) ___connect_i(x, y)

// 把符号变成字符串
#define __to_string2(x) #x
#define __to_string(x) __to_string2(x)

// 生成不同个数的顺序符号
#define ___repeat_0(m, ...)
#define ___repeat_1(m, ...)	___repeat_0(m, __VA_ARGS__)  m(1, __VA_ARGS__)
#define ___repeat_2(m, ...)	___repeat_1(m, __VA_ARGS__)  m(2, __VA_ARGS__)
#define ___repeat_3(m, ...)	___repeat_2(m, __VA_ARGS__)  m(3, __VA_ARGS__)
#define ___repeat_4(m, ...)	___repeat_3(m, __VA_ARGS__)  m(4, __VA_ARGS__)
#define ___repeat_5(m, ...)	___repeat_4(m, __VA_ARGS__)  m(5, __VA_ARGS__)
#define ___repeat_6(m, ...)	___repeat_5(m, __VA_ARGS__)  m(6, __VA_ARGS__)
#define ___repeat_7(m, ...)	___repeat_6(m, __VA_ARGS__)  m(7, __VA_ARGS__)
#define ___repeat_8(m, ...)	___repeat_7(m, __VA_ARGS__)  m(8, __VA_ARGS__)
#define ___repeat_9(m, ...)	___repeat_8(m, __VA_ARGS__)  m(9, __VA_ARGS__)
#define ___repeat_10(m, ...) ___repeat_9(m, __VA_ARGS__)  m(10, __VA_ARGS__)
#define ___repeat_11(m, ...) ___repeat_10(m, __VA_ARGS__)  m(11, __VA_ARGS__)
#define ___repeat_12(m, ...) ___repeat_11(m, __VA_ARGS__)  m(12, __VA_ARGS__)
#define ___repeat_13(m, ...) ___repeat_12(m, __VA_ARGS__)  m(13, __VA_ARGS__)
#define ___repeat_14(m, ...) ___repeat_13(m, __VA_ARGS__)  m(14, __VA_ARGS__)
#define ___repeat_15(m, ...) ___repeat_14(m, __VA_ARGS__)  m(15, __VA_ARGS__)

#define __last_repeat_0(m, ...)
#define __last_repeat_1(m, ...)	m(1, __VA_ARGS__)
#define __last_repeat_2(m, ...)	m(2, __VA_ARGS__)
#define __last_repeat_3(m, ...)	m(3, __VA_ARGS__)
#define __last_repeat_4(m, ...)	m(4, __VA_ARGS__)
#define __last_repeat_5(m, ...)	m(5, __VA_ARGS__)
#define __last_repeat_6(m, ...)	m(6, __VA_ARGS__)
#define __last_repeat_7(m, ...)	m(7, __VA_ARGS__)
#define __last_repeat_8(m, ...)	m(8, __VA_ARGS__)
#define __last_repeat_9(m, ...)	m(9, __VA_ARGS__)
#define __last_repeat_10(m, ...) m(10, __VA_ARGS__)
#define __last_repeat_11(m, ...)  m(11, __VA_ARGS__)
#define __last_repeat_12(m, ...)  m(12, __VA_ARGS__)
#define __last_repeat_13(m, ...)  m(13, __VA_ARGS__)
#define __last_repeat_14(m, ...)  m(14, __VA_ARGS__)
#define __last_repeat_15(m, ...)  m(15, __VA_ARGS__)

#define ___repeat2_0(m, p1,p2,p3)	m(0, p1,p2,p3)
#define ___repeat2_1(m, p1,p2,p3)	___repeat2_0(m, p1,p2,p3)  m(1, p1,p2,p3)
#define ___repeat2_2(m, p1,p2,p3)	___repeat2_1(m, p1,p2,p3)  m(2, p1,p2,p3)
#define ___repeat2_3(m, p1,p2,p3)	___repeat2_2(m, p1,p2,p3)  m(3, p1,p2,p3)
#define ___repeat2_4(m, p1,p2,p3)	___repeat2_3(m, p1,p2,p3)  m(4, p1,p2,p3)
#define ___repeat2_5(m, p1,p2,p3)	___repeat2_4(m, p1,p2,p3)  m(5, p1,p2,p3)
#define ___repeat2_6(m, p1,p2,p3)	___repeat2_5(m, p1,p2,p3)  m(6, p1,p2,p3)
#define ___repeat2_7(m, p1,p2,p3)	___repeat2_6(m, p1,p2,p3)  m(7, p1,p2,p3)
#define ___repeat2_8(m, p1,p2,p3)	___repeat2_7(m, p1,p2,p3)  m(8, p1,p2,p3)
#define ___repeat2_9(m, p1,p2,p3)	___repeat2_8(m, p1,p2,p3)  m(9, p1,p2,p3)
#define ___repeat2_10(m, p1,p2,p3) ___repeat2_9(m, p1,p2,p3)  m(10, p1,p2,p3)
#define ___repeat2_11(m, p1,p2,p3) ___repeat2_10(m, p1,p2,p3)  m(11, p1,p2,p3)
#define ___repeat2_12(m, p1,p2,p3) ___repeat2_11(m, p1,p2,p3)  m(12, p1,p2,p3)
#define ___repeat2_13(m, p1,p2,p3) ___repeat2_12(m, p1,p2,p3)  m(13, p1,p2,p3)
#define ___repeat2_14(m, p1,p2,p3) ___repeat2_13(m, p1,p2,p3)  m(14, p1,p2,p3)
#define ___repeat2_15(m, p1,p2,p3) ___repeat2_14(m, p1,p2,p3)  m(15, p1,p2,p3)

#define __last_repeat2_0(m, p1,p2,p3)
#define __last_repeat2_1(m, p1,p2,p3)	m(1, p1,p2,p3)
#define __last_repeat2_2(m, p1,p2,p3)	m(2, p1,p2,p3)
#define __last_repeat2_3(m, p1,p2,p3)	m(3, p1,p2,p3)
#define __last_repeat2_4(m, p1,p2,p3)	m(4, p1,p2,p3)
#define __last_repeat2_5(m, p1,p2,p3)	m(5, p1,p2,p3)
#define __last_repeat2_6(m, p1,p2,p3)	m(6, p1,p2,p3)
#define __last_repeat2_7(m, p1,p2,p3)	m(7, p1,p2,p3)
#define __last_repeat2_8(m, p1,p2,p3)	m(8, p1,p2,p3)
#define __last_repeat2_9(m, p1,p2,p3)	m(9, p1,p2,p3)
#define __last_repeat2_10(m, p1,p2,p3) m(10, p1,p2,p3)
#define __last_repeat2_11(m, p1,p2,p3)  m(11, p1,p2,p3)
#define __last_repeat2_12(m, p1,p2,p3)  m(12, p1,p2,p3)
#define __last_repeat2_13(m, p1,p2,p3)  m(13, p1,p2,p3)
#define __last_repeat2_14(m, p1,p2,p3)  m(14, p1,p2,p3)
#define __last_repeat2_15(m, p1,p2,p3)  m(15, p1,p2,p3)

#define ___repeat(n, m_begin, m_end, ...) ___connect(___repeat_, __cntdec(n))(m_begin, __VA_ARGS__) ___connect(__last_repeat_, n)(m_end, __VA_ARGS__)
#define ___repeat2(n, m_begin, m_end, p1,p2,p3) ___connect(___repeat2_, __cntdec(n))(m_begin, p1,p2,p3) ___connect(__last_repeat2_, n)(m_end, p1,p2,p3)


// 基础工具宏结束
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Add IDispatch to class

//////////////////////////////////////////////////////////////////////////
// 扩充 CVarTypeInfo 模板类的定义
//template<>
//class CVarTypeInfo< void >
//{
//public:
//	static const VARTYPE VT = VT_EMPTY;
//	//static char VARIANT::* const pmField;
//};
template<typename T>
class CVarTypeInfoEx : public CVarTypeInfo<T>
{
public:
	static HRESULT Assign(T& tDst, VARIANT* pSrc)
	{
		CComVariant v;
		if (FAILED(v.ChangeType(VT, pSrc))) return DISP_E_BADVARTYPE;
#pragma warning(push)
#pragma warning(disable:4800)
		tDst = v.*pmField;
#pragma warning(pop)
		return S_OK;
	}
	template<typename refcreator> static HRESULT AssignPtr(T* tDst, VARIANT* pSrc)
	{
		__if_exists(refcreator::ref_map_class) {
			if (pSrc->vt != VT_DISPATCH) return DISP_E_BADVARTYPE;
			T* v = refcreator::ref_map_class::__Disp2Class(pSrc->pdispVal); 
			if (v == NULL) return E_INVALIDARG;
			*tDst = *v;
			return S_OK;
		}
		__if_not_exists(refcreator::ref_map_class) {
			__if_exists(T::__ClassName) {
				if (pSrc->vt != VT_DISPATCH) return E_INVALIDARG;
				T* v = (T*)Disp2Class(pSrc->pdispVal, T::__ClassName());
				if (v == NULL) return E_INVALIDARG;
				*tDst = *v;
				return S_OK;
			}
			__if_not_exists(T::__ClassName) {
				HRESULT hr = Assign(*tDst, pSrc);
				return hr;
			}
		}
	}
	static T Value(CComVariant& v)
	{
		return v.*pmField;
	}
	static bool ChangeType(CComVariant& vDst, VARIANT* pSrc)
	{
		return SUCCEEDED(vDst.ChangeType(VT, pSrc));
	}
};

template<>
class CVarTypeInfoEx<VARIANT> : public CVarTypeInfo<VARIANT>
{
public:
	static HRESULT Assign(VARIANT& tDst, VARIANT* pSrc)
	{
		return ::VariantCopy(&tDst, pSrc);
	}
	static VARIANT Value(CComVariant& v)
	{
		return v;
	}
	static bool ChangeType(CComVariant& vDst, VARIANT* pSrc) { return vDst=*pSrc, true; }
};

template<>
class CVarTypeInfoEx<CComVariant> : public CVarTypeInfoEx<VARIANT>
{
public:
	static HRESULT Assign(CComVariant& tDst, VARIANT* pSrc)
	{
		tDst = *pSrc;
		return S_OK;
	}
	static CComVariant Value(CComVariant& v)
	{
		return v;
	}
};

//template<>
//class CVarTypeInfoEx<CComBSTR> : public CVarTypeInfoEx<BSTR>
//{
//public:
//	static HRESULT Assign(CComBSTR& tDst, VARIANT* pSrc)
//	{
//		CComVariant v;
//		if (FAILED(v.ChangeType(VT, pSrc))) return DISP_E_BADVARTYPE;
//		tDst
//#pragma warning(push)
//#pragma warning(disable:4800)
//		tDst = v.*pmField;
//#pragma warning(pop)
//		return S_OK;
//	}
//	static CComBSTR Value(CComVariant& v)
//	{
//		return v.*pmField;
//	}
//};

//////////////////////////////////////////////////////////////////////////
template<>
class CVarTypeInfo< bool >
{
public:
	static const VARTYPE VT = VT_BOOL;
	static VARIANT_BOOL VARIANT::* const pmField;
};

__declspec( selectany ) VARIANT_BOOL VARIANT::* const CVarTypeInfo< bool >::pmField = &VARIANT::boolVal;

// 扩充 CComBSTR 类型，用这种类型代替BSTR，能防止内存泄露或者内存释放错误
template<>
class CVarTypeInfo< CComBSTR >
{
public:
	static const VARTYPE VT = VT_BSTR;
	static BSTR VARIANT::* const pmField;
};

__declspec( selectany ) BSTR VARIANT::* const CVarTypeInfo< CComBSTR >::pmField = &VARIANT::bstrVal;

// END of CVarTypeInfo. 使用者可以自行扩充新的类型，例如用CString来保存字符串
//////////////////////////////////////////////////////////////////////////

// 定义多参数的模板类

// 方法工具模板类和工具宏
#define __tparam(n, ...) typename T##n,
#define __tparam_end(n, ...) typename T##n
#define __param_type(n, ...) if (n<=dp->cArgs && !CVarTypeInfoEx<T##n>::ChangeType(v[n-1], &dp->rgvarg[dp->cArgs-n])) return E_INVALIDARG;
#define __funcparam(n, ...) CVarTypeInfoEx<T##n>::Value(v[n-1]),	//.*CVarTypeInfo<T##n>::pmField,
#define __funcparam_end(n, ...) CVarTypeInfoEx<T##n>::Value(v[n-1])	//v[n-1].*CVarTypeInfo<T##n>::pmField
#define __funcparam_type(n, ...) T##n,
#define __funcparam_type_end(n, ...) T##n
#define __method_helper_t(n, ...) \
	template<class TT, typename rettype, ___repeat(n, __tparam, __tparam)  rettype (TT::* func)(___repeat(n, __funcparam_type, __funcparam_type_end)) > \
	class _MethodHelper_##n \
	{ \
	public: \
	static HRESULT CallMethod (LPVOID pT, DISPPARAMS* dp, VARIANT* pvarResult, DISPID dispid, LPCOLESTR name) \
	{ \
		if (pT==NULL) return E_FAIL; \
		/*if (dp->cArgs < n) return DISP_E_BADPARAMCOUNT;*/ \
		CComVariant v[n+1]; /*加1是为了避免 n==0 时的编译错误*/ \
		___repeat(n, __param_type, __param_type) \
		CComVariant vRet = (reinterpret_cast<TT*>(pT)->*func)( ___repeat(n, __funcparam, __funcparam_end) ); \
		if (pvarResult/* && vRet.vt!=VT_EMPTY*/) vRet.Detach(pvarResult); \
		return S_OK; \
	} \
	}; \
	/* 返回VOID的特化模板类 */ \
	template<class TT, ___repeat(n, __tparam, __tparam)  void (TT::* func)(___repeat(n, __funcparam_type, __funcparam_type_end)) > \
	class _MethodHelper_##n<TT, void, ___repeat(n, __funcparam_type, __funcparam_type)  func> \
	{ \
	public: \
	static HRESULT CallMethod (LPVOID pT, DISPPARAMS* dp, VARIANT* pvarResult, DISPID dispid, LPCOLESTR name) \
	{ \
		if (pT==NULL) return E_FAIL; \
		/*if (dp->cArgs < n) return DISP_E_BADPARAMCOUNT;*/ \
		CComVariant v[n+1]; /*加1是为了避免 n==0 时的编译错误*/ \
		___repeat(n, __param_type, __param_type) \
		(reinterpret_cast<TT*>(pT)->*func)( ___repeat(n, __funcparam, __funcparam_end) ); \
		return S_OK; \
	} \
	};

// 预定义16个方法调用工具模板类
___for_each_number(__method_helper_t)


//////////////////////////////////////////////////////////////////////////
// 可选参数模板类，
// 带可选参数的方法调用，必须使用Begin_Method_Params_Map/End_Method_Params_Map定义每个方法的参数列表以及可选参数个数
// 如果一个方法填写了这个映射表项，那么在 Begin_Disp_Map 里应该使用 Disp_Method_With_Optional，如果继续使用 Disp_Method，将不能应用可选参数功能
#define __opt_tparam(n, ...) typename T##n=int,
#define __opt_tparam_end(n, ...) typename T##n=int
#define __opt_tparam_type(n, ...) typedef T##n Type##n;
#define __opt_param_trans(n, ...) if (!CVarTypeInfoEx<Params::Type##n>::ChangeType(v[n-1], &dp->rgvarg[dp->cArgs-n])) return E_INVALIDARG;
#define __opt_param_call(n,  ...) CVarTypeInfoEx<Params::Type##n>::Value(v[n-1]),
#define __opt_param_call_end(n,  ...) CVarTypeInfoEx<Params::Type##n>::Value(v[n-1])

template<___repeat(15, __opt_tparam, __opt_tparam_end) >
class _Params
{
public:
	___repeat(15, __opt_tparam_type, __opt_tparam_type)
};

#define __opt_funcparamopt(n2, nt, name, ...) \
	case n2: { \
		CComVariant v[nt-n2+1]; \
		___repeat(___cntmixed(nt,n2), __opt_param_trans, __opt_param_trans) \
		vRet = p->name( ___repeat(___cntmixed(nt,n2), __opt_param_call, __opt_param_call_end) ); \
	} break;
#define __opt_funcparamopt_void(n2, nt, name, ...) \
	case n2: { \
		CComVariant v[nt-n2+1]; \
		___repeat(___cntmixed(nt,n2), __opt_param_trans, __opt_param_trans) \
		p->name( ___repeat(___cntmixed(nt,n2), __opt_param_call, __opt_param_call_end, nt, n2) ); \
	} break;

#define Begin_Method_Params_Map(classname) \
	public: class MethodParamsInfo \
	{ \
	public: \
		typedef classname ownerclass;

#define Method_Params(name, Nparam, Nopt, type, ...) \
	static HRESULT Call##name(LPVOID pT, DISPPARAMS* dp, VARIANT* pvarResult, DISPID dispid, LPCOLESTR szname) \
	{ \
		typedef _Params<__VA_ARGS__ > Params; \
		if (pT==NULL) return E_FAIL; \
		ownerclass* p = (ownerclass*)pT; \
		if (dp->cArgs < Nparam-Nopt) return DISP_E_BADPARAMCOUNT; \
		CComVariant vRet; \
		switch (Nparam - min(Nparam,dp->cArgs)) \
		{ \
		___repeat2(Nopt, __opt_funcparamopt, __opt_funcparamopt, Nparam, name, 0) \
		case 16:break; \
		} \
		if (pvarResult /*&& vRet.vt!=VT_EMPTY*/) vRet.Detach(pvarResult); \
		return S_OK; \
	};

#define Method_Params_void(name, Nparam, Nopt, ...) \
	static HRESULT Call##name(LPVOID pT, DISPPARAMS* dp, VARIANT* pvarResult, DISPID dispid, LPCOLESTR szname) \
	{ \
		typedef _Params<__VA_ARGS__ > Params; \
		if (pT==NULL) return E_FAIL; \
		ownerclass* p = (ownerclass*)pT; \
		if (dp->cArgs < Nparam-Nopt) return DISP_E_BADPARAMCOUNT; \
		switch (Nparam - min(Nparam,dp->cArgs)) \
		{ \
		___repeat2(Nopt, __opt_funcparamopt_void, __opt_funcparamopt_void, Nparam, name, 0) \
		case 16:break; \
		} \
		return S_OK; \
	};

#define End_Method_Params_Map() };


#define _method_helper(T, name, type, paramcnt, ...) _MethodHelper_##paramcnt<T,type,__VA_ARGS__,&T::name>::CallMethod
//#define _method_helper_with_option(T, name, type, paramcnt, params, optparams, optdefvals) _MethodHelper_##paramcnt<T,type,params optparams,optdefvals>,&T::name>::CallMethod
#define _method_helper_with_option(name) MethodParamsInfo::Call##name


//////////////////////////////////////////////////////////////////////////
// 属性GET/PUT工具模板类和工具宏

// 直接访问基类成员变量时采用这个模板
//template<class T, class baseT/*=T*/, typename rettype, rettype baseT::* member>
//class _GetHelper
//{
//public:
//	static HRESULT CallGet(LPVOID pT, DISPPARAMS* dp, VARIANT* pvarResult)
//	{
//		if (pT==NULL) return E_FAIL;
//		CComVariant vRet = reinterpret_cast<T*>(pT)->*(member);
//		if (pvarResult) vRet.Detach(pvarResult);
//		return S_OK;
//	}
//};

template<class T, typename rettype, size_t member_offset/*=offsetof(T,member)*/, typename refcreator_rettype=int>
class _GetPutHelper
{
public:
	static HRESULT CallGet(LPVOID pT, DISPPARAMS* dp, VARIANT* pvarResult, DISPID dispid, LPCOLESTR name)
	{
		if (pT==NULL) return E_FAIL;
		CComVariant vRet; // = // *(rettype*)(((char*)pT)+member_offset);
		__if_exists(rettype::GetDispatch) { vRet = ((rettype*)(((char*)pT)+member_offset))->GetDispatch(); }
		__if_not_exists(rettype::GetDispatch) {
			__if_exists(refcreator_rettype::CreateInstance) { vRet.vt=VT_DISPATCH; refcreator_rettype::CreateInstance((rettype*)(((char*)pT)+member_offset), &vRet.pdispVal); }
			__if_not_exists(refcreator_rettype::CreateInstance) { vRet = *(rettype*)(((char*)pT)+member_offset); }
		}
		if (pvarResult) vRet.Detach(pvarResult);
		return S_OK;
	}
	static HRESULT CallPut(LPVOID pT, DISPPARAMS* dp, VARIANT* pvarResult, DISPID dispid, LPCOLESTR name)
	{
		if (pT==NULL) return E_FAIL;
		if (dp->cArgs != 1) return DISP_E_BADPARAMCOUNT;
		__if_exists(refcreator_rettype::ref_map_class) {
			if (dp->rgvarg->vt == VT_BSTR && dp->rgvarg->bstrVal &&
				SUCCEEDED(refcreator_rettype::ref_map_class::__InitFromString((rettype*)(((char*)pT)+member_offset), dp->rgvarg->bstrVal)))
			{
				__if_exists(T::OnPropertyChanged) { ((T*)pT)->OnPropertyChanged(dispid, name); }
				return S_OK;
			}
			if (dp->rgvarg->vt != VT_DISPATCH) return E_INVALIDARG;
			rettype* v = refcreator_rettype::ref_map_class::__Disp2Class(dp->rgvarg->pdispVal); 
			if (v == NULL) return E_INVALIDARG;
			*(rettype*)(((char*)pT)+member_offset) = *v;
			__if_exists(T::OnPropertyChanged) { ((T*)pT)->OnPropertyChanged(dispid, name); }
			//refcreator_rettype::ref_map_class::__FirePropertyChanged(pT, dispid, name);
			return S_OK;
		}
		__if_not_exists(refcreator_rettype::ref_map_class) {
			__if_exists(rettype::__ClassName) {
				__if_exists(rettype::InitFromString) {
					if (dp->rgvarg->vt == VT_BSTR && dp->rgvarg->bstrVal &&
						SUCCEEDED(((rettype*)(((char*)pT)+member_offset))->InitFromString(dp->rgvarg->bstrVal)))
					{
						__if_exists(T::OnPropertyChanged) { ((T*)pT)->OnPropertyChanged(dispid, name); }
						return S_OK;
					}
				}
				if (dp->rgvarg->vt != VT_DISPATCH) return E_INVALIDARG;
				rettype* v = (rettype*)Disp2Class(dp->rgvarg->pdispVal, rettype::__ClassName());
				if (v == NULL) return E_INVALIDARG;
				*(rettype*)(((char*)pT)+member_offset) = *v;
				__if_exists(T::OnPropertyChanged) { ((T*)pT)->OnPropertyChanged(dispid, name); }
				return S_OK;
			}
			__if_not_exists(rettype::__ClassName) {
				HRESULT hr = CVarTypeInfoEx<rettype>::Assign(*(rettype*)(((char*)pT)+member_offset), dp->rgvarg);
				__if_exists(T::OnPropertyChanged) { if (SUCCEEDED(hr)) ((T*)pT)->OnPropertyChanged(dispid, name); }
				return hr;
			}
		}

//		CComVariant v;
//		if (FAILED(v.ChangeType(CVarTypeInfo<rettype>::VT, dp->rgvarg))) return DISP_E_BADVARTYPE;
//#pragma warning(push)
//#pragma warning(disable:4800)
//		reinterpret_cast<T*>(pT)->*member = v.*CVarTypeInfo<rettype>::pmField;
//#pragma warning(pop)
//		return S_OK;
	}
};

// 用户当前类未直接提供成员，但是在派生类里提供了成员，通过定义函数获取成员指针
template<class T, typename rettype, rettype& (T::* getreffunc)(), typename refcreator_rettype=int>
class _GetPutRefHelper
{
public:
	static HRESULT CallGet(LPVOID pT, DISPPARAMS* dp, VARIANT* pvarResult, DISPID dispid, LPCOLESTR name)
	{
		if (pT==NULL) return E_FAIL;
		CComVariant vRet; // = // *(rettype*)(((char*)pT)+member_offset);
		__if_exists(rettype::GetDispatch) { vRet = ((((T*)pT)->*getreffunc)()).GetDispatch(); }
		__if_not_exists(rettype::GetDispatch) {
			__if_exists(refcreator_rettype::CreateInstance) { vRet.vt=VT_DISPATCH; refcreator_rettype::CreateInstance(&(((T*)pT)->*getreffunc)(), &vRet.pdispVal); }
			__if_not_exists(refcreator_rettype::CreateInstance) { vRet = (((T*)pT)->*getreffunc)(); }
		}
		if (pvarResult) vRet.Detach(pvarResult);
		return S_OK;
	}
	static HRESULT CallPut(LPVOID pT, DISPPARAMS* dp, VARIANT* pvarResult, DISPID dispid, LPCOLESTR name)
	{
		if (pT==NULL) return E_FAIL;
		if (dp->cArgs != 1) return DISP_E_BADPARAMCOUNT;
		__if_exists(refcreator_rettype::ref_map_class) {
			if (dp->rgvarg->vt == VT_BSTR && dp->rgvarg->bstrVal &&
				SUCCEEDED(refcreator_rettype::ref_map_class::__InitFromString(&(((T*)pT)->*getreffunc)(), dp->rgvarg->bstrVal)))
			{
				__if_exists(T::OnPropertyChanged) { ((T*)pT)->OnPropertyChanged(dispid, name); }
				return S_OK;
			}
			if (dp->rgvarg->vt != VT_DISPATCH) return E_INVALIDARG;
			rettype* v = refcreator_rettype::ref_map_class::__Disp2Class(dp->rgvarg->pdispVal); 
			if (v == NULL) return E_INVALIDARG;
			(((T*)pT)->*getreffunc)() = *v;
			__if_exists(T::OnPropertyChanged) { ((T*)pT)->OnPropertyChanged(dispid, name); }
			return S_OK;
		}
		__if_not_exists(refcreator_rettype::ref_map_class) {
			__if_exists(rettype::__ClassName) {
				__if_exists(rettype::InitFromString) {
					if (dp->rgvarg->vt == VT_BSTR && dp->rgvarg->bstrVal &&
						SUCCEEDED((((T*)pT)->*getreffunc)().InitFromString(dp->rgvarg->bstrVal)))
					{
						__if_exists(T::OnPropertyChanged) { ((T*)pT)->OnPropertyChanged(dispid, name); }
						return S_OK;
					}
				}
				if (dp->rgvarg->vt != VT_DISPATCH) return E_INVALIDARG;
				rettype* v = (rettype*)Disp2Class(dp->rgvarg->pdispVal, rettype::__ClassName());
				if (v == NULL) return E_INVALIDARG;
				(((T*)pT)->*getreffunc)() = *v;
				__if_exists(T::OnPropertyChanged) { ((T*)pT)->OnPropertyChanged(dispid, name); }
				return S_OK;
			}
			__if_not_exists(rettype::__ClassName) {
				HRESULT hr = CVarTypeInfoEx<rettype>::Assign((((T*)pT)->*getreffunc)(), dp->rgvarg);
				__if_exists(T::OnPropertyChanged) { if (SUCCEEDED(hr)) ((T*)pT)->OnPropertyChanged(dispid, name); }
				return hr;
			}
		}
	}
};

// 用户提供了Get函数时采用这个模板，在这种情况下，属性名称不需要跟成员变量名称一致。函数原型是 HRESULT GetXXX(VARIANT*)
template<class T, HRESULT (T::* getfunc)(VARIANT*)>
class _GetFuncHelper
{
public:
	static HRESULT CallGet(LPVOID pT, DISPPARAMS* dp, VARIANT* pvarResult, DISPID dispid, LPCOLESTR name)
	{
		if (pT==NULL) return E_FAIL;
		CComVariant vRet;
		HRESULT hr = (reinterpret_cast<T*>(pT)->*getfunc)(&vRet);
		if (pvarResult) return vRet.Detach(pvarResult);
		return S_OK;
	}
};

// 对于有默认值的集合类（如 colls(1)），必须使用函数方式，因为GET操作也会带参数。函数原型是 HRESULT GetXXX(VARIANT index, VARIANT* pResult)
template<class T, HRESULT (T::* getvaluefunc)(VARIANT,VARIANT*)>
class _GetValueFuncHelper
{
public:
	static HRESULT CallGet(LPVOID pT, DISPPARAMS* dp, VARIANT* pvarResult, DISPID dispid, LPCOLESTR name)
	{
		if (pT==NULL) return E_FAIL;
		CComVariant vIndex;
		if (dp->cArgs>0) vIndex = dp->rgvarg[dp->cArgs-1];
		CComVariant vRet;
		HRESULT hr = (reinterpret_cast<T*>(pT)->*getvaluefunc)(vIndex, &vRet);
		if (pvarResult) return vRet.Detach(pvarResult);
		return S_OK;
	}
};

#define _get_helper(T, name, type) _GetPutHelper<T,type,offsetof(T,name) __if_exists(RefCreator_##type){,RefCreator_##type}>::CallGet
#define _getref_helper(T, name, type) _GetPutRefHelper<T,type,&T::Get##name##Ref __if_exists(RefCreator_##type){,RefCreator_##type}>::CallGet
#define _getfunc_helper(T, name) _GetFuncHelper<T, &T::Get##name>::CallGet
#define _getvalue_helper(T, name) _GetValueFuncHelper<T, &T::Get##name>::CallGet



//////////////////////////////////////////////////////////////////////////
// 属性PUT工具模板类和工具宏

// 用户提供了Set或Put函数时采用这个模板，在这种情况下，属性名称不需要跟成员变量名称一致。函数原型是 HRESULT SetXXX(VARIANT*) 或 HRESULT PutXXX(VARIANT*)
template<class T, HRESULT (T::* putfunc)(VARIANT*)>
class _PutFuncHelper
{
public:
	static HRESULT CallPut(LPVOID pT, DISPPARAMS* dp, VARIANT* pvarResult, DISPID dispid, LPCOLESTR name)
	{
		if (pT==NULL) return E_FAIL;
		if (dp->cArgs != 1) return DISP_E_BADPARAMCOUNT;
		HRESULT hr = (reinterpret_cast<T*>(pT)->*putfunc)(dp->rgvarg);
		__if_exists(T::OnPropertyChanged) { if (SUCCEEDED(hr)) ((T*)pT)->OnPropertyChanged(dispid, name); }
		return hr;
	}
};

#define _put_helper(T, name, type) _GetPutHelper<T,type,offsetof(T,name) __if_exists(RefCreator_##type){,RefCreator_##type}>::CallPut
#define _putref_helper(T, name, type) _GetPutRefHelper<T,type,&T::Get##name##Ref __if_exists(RefCreator_##type){,RefCreator_##type}>::CallPut
#define _putfunc_helper(T, name) _PutFuncHelper<T,&T::Put##name>::CallPut
#define _setfunc_helper(T, name) _PutFuncHelper<T,&T::Set##name>::CallPut


//////////////////////////////////////////////////////////////////////////
// 映射表工具模板类和映射宏
typedef HRESULT (* fnDispMethod)(LPVOID pT, DISPPARAMS* dp, VARIANT* pVarResult, DISPID dispid, LPCOLESTR name);
struct DispMethodData 
{
	LPCOLESTR name;		// property or method name
	DISPID dispid;		// dispid
	fnDispMethod pfnGet;
	fnDispMethod pfnPut;
	fnDispMethod pfnMethod;
	DWORD_PTR reserved; // == 0
};

//////////////////////////////////////////////////////////////////////////
typedef HRESULT (CALLBACK* fnCreateNamedItem)(DISPPARAMS*, IDispatch**, BOOL);
typedef struct __tagCreatableNamedItemFactory
{
	LPCOLESTR name;
	fnCreateNamedItem pfnCreateNamedItem;
}CreatableNamedItemFactory;

#pragma section("AUTOMATION$__a", read, shared)
#pragma section("AUTOMATION$__b", read, shared)
#pragma section("AUTOMATION$__c", read, shared)
extern "C"
{
	__declspec(selectany) __declspec(allocate("AUTOMATION$__a")) CreatableNamedItemFactory* __pcniMapEntryFirst = NULL;
	__declspec(selectany) __declspec(allocate("AUTOMATION$__c")) CreatableNamedItemFactory* __pcniMapEntryLast = NULL;
}
#pragma comment(linker, "/merge:AUTOMATION=.rdata")

// 可创建的自动化命名项可以在脚本里直接创建，比如 Rect(1,2,3,4)
#define Implement_Creatable_NamedItem(classname) \
	class classname##__Creator \
	{ \
		public: static HRESULT CALLBACK __CreateNamedItemInstance(DISPPARAMS* params, IDispatch** ppDisp, BOOL bInit=TRUE) \
		{ \
			HRESULT hr = E_FAIL; \
			__if_exists(classname::CreateInstance) \
			{ \
				classname* pClass=NULL; \
				hr = classname::CreateInstance(ppDisp, &pClass); \
				if (SUCCEEDED(hr)) \
				{ \
					__if_exists(classname::InitNamedItem) \
					{ \
						if (bInit && FAILED(hr=pClass->InitNamedItem(params))) \
						{ \
							(*ppDisp)->Release(); \
							*ppDisp = NULL; \
						} \
					} \
					__if_not_exists(classname::InitNamedItem) \
					{ \
						__pragma(message("WARNING: class '" #classname "' hasn't member function 'HRESULT InitNamedItem(DISPPARAMS* params)'. FILE(" __FILE__ ") LINE(" __to_string(__LINE__) ")")) \
					} \
				} \
			} \
			__if_not_exists(classname::CreateInstance) \
			{ \
				__pragma(message("WARNING: class '" #classname "' has to use 'Begin_Auto_Disp_Map' to implement creatable named item. FILE(" __FILE__ ") LINE(" __to_string(__LINE__) ")")) \
			} \
			return hr; \
		} \
	}; \
	__declspec(selectany) CreatableNamedItemFactory __cniMap_##classname = {__if_exists(classname::__ClassName){classname::__ClassName(), classname##__Creator::__CreateNamedItemInstance} \
			__if_not_exists(classname::__ClassName){NULL,NULL}}; \
	extern "C" __declspec(allocate("AUTOMATION$__b")) __declspec(selectany) CreatableNamedItemFactory* const __pcniMap_##classname = &__cniMap_##classname; \
	__pragma(comment(linker, "/include:___pcniMap_" #classname));


static IDispatch* GetCreatableNamedItemPeer()
{
	class CNioDisp : public IDispatchEx
	{
		ULONG _refCnt;
		fnCreateNamedItem _pfn;
		CSimpleArray<CreatableNamedItemFactory> factories;
	public:
		CNioDisp(fnCreateNamedItem pfn=NULL) : _pfn(pfn), _refCnt(1)
		{
			if (_pfn==NULL)
			{
				for (CreatableNamedItemFactory** ppEntry = &__pcniMapEntryFirst; ppEntry < &__pcniMapEntryLast; ppEntry++)
				{
					if (*ppEntry && (*ppEntry)->name && (*ppEntry)->pfnCreateNamedItem)
						factories.Add(**ppEntry);
				}
			}
		}

		STDMETHOD_(ULONG, AddRef)() { return ++_refCnt; }
		STDMETHOD_(ULONG, Release)()
		{
			if (--_refCnt==0 && _pfn) return (delete this), 0;
			return _refCnt;
		}
		STDMETHOD(QueryInterface)(REFIID iid, LPVOID* ppvObj)
		{
			if (!ppvObj)
				return E_POINTER;

			*ppvObj = NULL;
			if ((iid==__uuidof(IUnknown)) ||
				(iid==__uuidof(IDispatch)) ||
				(_pfn && iid==__uuidof(IDispatchEx)))
			{
				*ppvObj = (IDispatchEx*)this;
				AddRef();
				return S_OK;
			}
			return E_NOINTERFACE;
		}
		STDMETHOD(GetTypeInfoCount)(UINT *pctinfo)
		{
			*pctinfo = 0;
			ATLTRACENOTIMPL(_T("CNioDisp::GetTypeInfoCount"));
		}
		STDMETHOD(GetTypeInfo)(UINT /*iTInfo*/, LCID /*lcid*/, ITypeInfo **ppTInfo)
		{
			*ppTInfo = NULL;
			ATLTRACENOTIMPL(_T("CNioDisp::GetTypeInfo"));
		}
		STDMETHOD(GetIDsOfNames)(REFIID riid, OLECHAR ** rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId)
		{
			*rgDispId = DISPID_UNKNOWN;
			for (int i=0,num=factories.GetSize(); i<num; i++)
			{
				if (lstrcmpiW(*rgszNames, factories[i].name) == 0)
				{
					return *rgDispId = i, S_OK;
				}
			}
			return DISP_E_MEMBERNOTFOUND;
		}
		STDMETHOD(Invoke)(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pdispparams, VARIANT *pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr)
		{
			if (dispIdMember>=0 && dispIdMember<factories.GetSize())
			{
				if ((wFlags&DISPATCH_METHOD)==0 && (wFlags!=DISPATCH_PROPERTYGET)) return E_UNEXPECTED;
				pVarResult->vt = VT_DISPATCH;
				if (wFlags&DISPATCH_METHOD)
					return factories[dispIdMember].pfnCreateNamedItem(pdispparams, &pVarResult->pdispVal, (wFlags&DISPATCH_METHOD));
				else // get
					return (pVarResult->pdispVal=(IDispatch*)NEW CNioDisp(factories[dispIdMember].pfnCreateNamedItem)), S_OK;
			}
			return DISP_E_MEMBERNOTFOUND;
		}

		// IDispatchEx
		STDMETHOD(DeleteMemberByDispID)(DISPID id) { return E_NOTIMPL; }
		STDMETHOD(DeleteMemberByName)(BSTR bstrName, DWORD grfdex) { return E_NOTIMPL; }
		STDMETHOD(GetDispID)(BSTR bstrName, DWORD grfdex, DISPID *pid) { return E_NOTIMPL; }
		STDMETHOD(GetMemberName)(DISPID id, BSTR *pbstrName) { return E_NOTIMPL; }
		STDMETHOD(GetMemberProperties)(DISPID id, DWORD grfdexFetch, DWORD *pgrfdex) { return E_NOTIMPL; }
		STDMETHOD(GetNameSpaceParent)(IUnknown **ppunk) { return E_NOTIMPL; }
		STDMETHOD(GetNextDispID)(DWORD grfdex, DISPID id, DISPID *pid) { return E_NOTIMPL; }
		STDMETHOD(InvokeEx)(DISPID id, LCID lcid, WORD wFlags, DISPPARAMS *pdp, VARIANT *pvarRes, EXCEPINFO *pei, IServiceProvider *pspCaller)
		{
			if (_pfn==NULL) return E_INVALIDARG;
			switch(wFlags)
			{
			case DISPATCH_METHOD:
			case DISPATCH_CONSTRUCT:
				pvarRes->vt = VT_DISPATCH;
				return _pfn(pdp, &pvarRes->pdispVal, TRUE);
				break;
			default:
				return E_INVALIDARG;
			}
			return S_OK;
		}
	};

	static CNioDisp disp;
	return &disp;
}

// {276887CB-4F1A-468d-AF41-D03070C53E68}
EXTERN_C const GUID DECLSPEC_SELECTANY IID_IDispHost = { 0x276887cb, 0x4f1a, 0x468d, { 0xaf, 0x41, 0xd0, 0x30, 0x70, 0xc5, 0x3e, 0x68 } };

MIDL_INTERFACE("276887CB-4F1A-468d-AF41-D03070C53E68")
IDispHost : public IUnknown
{
public:
	virtual LPCOLESTR STDMETHODCALLTYPE GetClassName() = 0;
	virtual LPVOID STDMETHODCALLTYPE GetOwner() = 0;
};


static LPVOID Disp2Class(IUnknown* unk, LPCOLESTR name)
{
	if (unk==NULL || name==NULL) return NULL;

#ifdef __DISPATCHIMPL_H__
	CComPtr<IDispatch> disp;
	CComQIPtr<IPeerHolder2> ph(unk);
	if (ph.p)
	{
		if (SUCCEEDED(ph->FindObject(name, &disp)))
			unk = disp.p;
	}
#endif // __DISPATCHIMPL_H__

	CComQIPtr<IDispHost> dh(unk);
	if (dh.p==NULL || name!=dh->GetClassName()) return NULL;
	return dh->GetOwner();
}

// DISP_CAST 可以自动查找参考的派生类，并转换到 classname
#define DISP_CAST(disp, classname) \
	__if_exists(RefCreator_##classname) { RefCreator_##classname::ref_map_class::__Disp2Class(disp) } \
	__if_not_exists(RefCreator_##classname) { \
		__if_exists(classname::__ClassName){(classname*)Disp2Class((disp), classname::__ClassName())} \
		__if_not_exists(classname::__ClassName) {NULL} \
	}

// DispProxy 是外部IDispatch的代理对象，其本身是有引用计数的，可自动释放。外部 IDispatch 指针可以是非自动释放的指针
class DispProxy : public IDispatch
{
	ULONG _refcount;
	IDispatch* _extdisp;

	DispProxy(IDispatch* disp) : _extdisp(disp), _refcount(1)
	{
		_extdisp->AddRef();
	}

	~DispProxy()
	{
		Clear();
	}

public:

	static DispProxy* New(IDispatch* disp)
	{
		if (disp==NULL) return NULL;
		return NEW DispProxy(disp);
	}

	void Clear()
	{
		if (_extdisp) _extdisp->Release();
		_extdisp = NULL;
	}

	STDMETHOD_(ULONG, AddRef)()
	{
		++_refcount;
		return _refcount;
	}
	STDMETHOD_(ULONG, Release)()
	{
		--_refcount;
		if(_refcount==0)
		{
			delete this;
			return 0;
		};
		return _refcount;
	}
	STDMETHOD(QueryInterface)(REFIID iid, LPVOID* ppvObj)
	{
		if (!_extdisp) return E_NOINTERFACE;
		return _extdisp->QueryInterface(iid, ppvObj);
	}
	STDMETHOD(GetTypeInfoCount)(UINT *pctinfo) { *pctinfo=0; return E_NOTIMPL; }
	STDMETHOD(GetTypeInfo)(UINT /*iTInfo*/, LCID /*lcid*/, ITypeInfo **ppTInfo) { *ppTInfo = NULL; return E_NOTIMPL; }
	STDMETHOD(GetIDsOfNames)(REFIID riid, OLECHAR ** rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId)
	{
		if (!_extdisp) return DISP_E_UNKNOWNNAME;
		return _extdisp->GetIDsOfNames(riid, rgszNames, cNames, lcid, rgDispId);
	}
	STDMETHOD(Invoke)(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pdispparams, VARIANT *pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr)
	{
		if (!_extdisp) return DISP_E_MEMBERNOTFOUND;
		return _extdisp->Invoke(dispIdMember, riid, lcid, wFlags, pdispparams, pVarResult, pExcepInfo, puArgErr);
	}
};

template<class T, bool tManaged=false>
class DispProvider : public IDispatch, public IDispHost
{
	friend T;
private:
	T* _owner;
	ULONG _refcount;
	//DispProxy* _proxy;

public:
	DispProvider() : _owner(NULL), _refcount(1)/*, _proxy(NULL)*/ {} // 注意引用计数初始值为1。如果不是用CreateInstance/__CreateNamedItemInstance创建的，该引用计数将保证不会被误删除
	//~DispProvider()
	//{
	//	__if_not_exists(T::CreateInstance)
	//	{
	//		if (_proxy) { _proxy->Clear(); _proxy->Release(); _proxy=NULL; }
	//	}
	//}
	void SetOwner(T* owner) { _owner = owner; }
	void operator =(const DispProvider<T,tManaged>& other) {} // 两个属主类通过赋值复制数据时避免复制此代理数据
	// IDispHost
	STDMETHOD_(LPVOID, GetOwner)() { return _owner; }
	STDMETHOD_(LPCOLESTR, GetClassName)()
	{
		__if_exists(T::__ClassName)
		{
			return T::__ClassName();
		}
		return NULL;
	}
	//STDMETHOD_(BOOL, GetOwnerAttribute)(LPCOLESTR szName, OUT LPVOID pBuffer, LPCOLESTR szTypeName)
	//{
	//	__if_exists(T::DispGetAttribute)
	//	{
	//		if (_owner) return _owner->DispGetAttribute(szName, pBuffer, szTypeName);
	//	}
	//	return FALSE;
	//}
	//STDMETHOD_(void, SetOwnerAttribute)(LPCOLESTR szName, LPVOID pValue, LPCOLESTR szTypeName)
	//{
	//	__if_exists(T::DispSetAttribute)
	//	{
	//		if (_owner) _owner->DispSetAttribute(szName, pValue, szTypeName);
	//	}
	//}
	//} 

	IDispatch* GetDispatch()
	{
		//__if_not_exists(T::CreateInstance)
		//{
		//	if (_proxy==NULL)
		//	{
		//		_proxy = DispProxy::New(this);
		//	}
		//	return _proxy;
		//}
		//__if_exists(T::CreateInstance)
		{ return this; }
	}

	/* IDispatch Methods*/
	STDMETHOD_(ULONG, AddRef)()
	{
		++_refcount;
#ifdef _DEBUG
		__if_exists(T::__DebugOnAddRef) { _owner->__DebugOnAddRef(); }
#endif // _DEBUG
		return /*tManaged ?*/ _refcount /*: 2*/;
	}
	STDMETHOD_(ULONG, Release)()
	{
		--_refcount;
#ifdef _DEBUG
		__if_exists(T::__DebugOnRelease) { _owner->__DebugOnRelease(); }
#endif // _DEBUG
		if(tManaged && _refcount==0)
		{
			__if_exists(T::DeleteInstance){T::DeleteInstance(_owner);}
			return 0;
			//delete this;
		};
		return /*tManaged ?*/ _refcount /*: 1*/;
	}
	STDMETHOD(QueryInterface)(REFIID iid, LPVOID* ppvObj)
	{
		if (!_owner) return E_UNEXPECTED;
		if (!ppvObj) return E_POINTER;
		*ppvObj = NULL;
		if (IsEqualIID(iid, __uuidof(IUnknown)) ||
			IsEqualIID(iid, __uuidof(IDispatch)))
			*ppvObj = (IDispatch*)this;
		else if (IsEqualIID(iid, IID_IDispHost))
			*ppvObj = (IDispHost*)this;
		if (*ppvObj)
		{
			((LPUNKNOWN)(*ppvObj))->AddRef();
			return S_OK;
		}
		return E_NOINTERFACE;
	}
	STDMETHOD(GetTypeInfoCount)(UINT *pctinfo) { *pctinfo=0; return E_NOTIMPL; }
	STDMETHOD(GetTypeInfo)(UINT /*iTInfo*/, LCID /*lcid*/, ITypeInfo **ppTInfo) { *ppTInfo = NULL; return E_NOTIMPL; }
	STDMETHOD(GetIDsOfNames)(REFIID riid, OLECHAR ** rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId)
	{
		ATLASSERT(cNames == 1);
		if (cNames != 1) return E_NOTIMPL;
		if (!_owner) return E_UNEXPECTED;

		// 给用户机会处理DISPID，用户实现的原型： BOOL __DispidOfName(LPCOLESTR szName, DISPID* pDispid);
		__if_exists(T::__DispidOfName)
		{
			if (_owner->__DispidOfName(*rgszNames, rgDispId))
				return S_OK;
		}

		*rgDispId = DISPID_UNKNOWN;
		const DispMethodData* pMap = T::__GetDispMapEntry(*rgszNames);
		if (pMap)
			return *rgDispId = pMap->dispid, S_OK;
		return DISP_E_UNKNOWNNAME;
	}
	STDMETHOD(Invoke)(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pdispparams, VARIANT *pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr)
	{
		if (!_owner) return E_UNEXPECTED;

		// 给用户机会处理Invoke调用，用户实现的原型： HRESULT __Invoke(DISPID dispid, WORD wFlags, DISPPARAMS* pdispparams, VARIANT *pVarResult);
		// 仅当返回 DISP_E_MEMBERNOTFOUND 时才执行后续过程，否则整个函数返回
		__if_exists(T::__Invoke)
		{
			HRESULT hr = _owner->__Invoke(dispIdMember, wFlags, pdispparams, pVarResult);
			if (hr != DISP_E_MEMBERNOTFOUND)
				return hr;
		}

		LPVOID pVoid = _owner;
		const DispMethodData* pMap = T::__GetDispMapEntry(NULL, &dispIdMember, &pVoid);
		// 如果映射表中没有DISPVALUE项，则作为属性读取时直接返回类名
		if (dispIdMember==DISPID_VALUE && (wFlags==DISPATCH_PROPERTYGET) && pMap==NULL)
		{
			CComVariant vRet = GetClassName();
			return vRet.Detach(pVarResult);
		}
		if (pMap)
		{
			//if ((wFlags&DISPATCH_PROPERTYGET) && dispIdMember==DISPID_VALUE && pMap->pfnGet)
			//	return pMap->pfnGet(_owner, pdispparams, pVarResult);

			fnDispMethod pfn = (wFlags&DISPATCH_METHOD) ? pMap->pfnMethod : (wFlags==DISPATCH_PROPERTYGET) ? pMap->pfnGet : pMap->pfnPut;
			/*
			 *	允许类使用一个通用的 OnPropertyCanAccess 方法作为属性读取/设置的过滤器，如果返回 FALSE，则表示不能读取/设置该属性
			 *	方法原型： BOOL T::OnPropertyCanAccess(BOOL bPut, DISPID dispid, LPCOLESTR name); bPut:(TRUE - Put; FALSE - Get)
			 */
			__if_exists(T::OnPropertyCanAccess)
			{
				if (pfn && (wFlags==DISPATCH_PROPERTYGET) && !_owner->OnPropertyCanAccess(FALSE, pMap->dispid, pMap->name))
					return DISP_E_MEMBERNOTFOUND;
				else if (((pfn && ((wFlags&(DISPATCH_PROPERTYGET|DISPATCH_METHOD))==0)) // put
					|| (pfn==NULL && (wFlags&DISPATCH_METHOD) && pMap->pfnPut && pdispparams->cArgs>=1)) // put as method
					&& !_owner->OnPropertyCanAccess(TRUE, pMap->dispid, pMap->name))
					return DISP_E_MEMBERNOTFOUND;
					//return ::VariantClear(pVarResult), S_FALSE;
			}
			/*
			 *	有一种特殊的级联用法，Put类型的属性名可以被当作方法调用，它返回对象自身。
			 *	此种方式的好处是能使对象做级联式的属性赋值。
			 *	例如：假设对象有3个可设置的属性pa,pb,pc，如果PUT方法设置为可方法化调用，则下面两种调用方式相同
			 *			obj.pa = 1; obj.pb = 2; obj.pc = 3;		单独设置3个属性
			 *			obj.pa(1).pb(2).pb(3);					级联设置3个属性
			 *	级联类型必须在表格上的方法栏里填入固定值 1.
			 */
			if (pfn==NULL && (wFlags&DISPATCH_METHOD) && pMap->pfnPut && pdispparams->cArgs>=1) // 级联调用
			{
				// 首先调用 PUT 方法
				CComVariant vRet;
				HRESULT hr = pMap->pfnPut(pVoid, pdispparams, &vRet, pMap->dispid, pMap->name);
				if (FAILED(hr)) return hr;
				// 返回自身
				if (pVarResult)
				{
					vRet = (IDispatch*)this;
					return vRet.Detach(pVarResult);
				}
				return S_OK;
			}
			if (pfn)
				return pfn(pVoid/*_owner*/, pdispparams, pVarResult, pMap->dispid, pMap->name);
		}
		return DISP_E_MEMBERNOTFOUND;
	}
};

//////////////////////////////////////////////////////////////////////////
// 父映射的类继承工具
#define __parent_map(n, ...) __if_exists(T##n::__GetDispMapEntry) { if (!p) p = T##n::__GetDispMapEntry(pName, pDispid); if (p) pVoid=(LPVOID)static_cast<T##n*>((T*)pVoid); }
#define __tparam16(n,...) typename T##n=int,
#define __tparam16_end(n,...) typename T##n=int

template<class T, ___repeat(15, __tparam16, __tparam16_end)>
class _ParentMapHelper
{
public:
	static const DispMethodData* __GetParentsMap(LPCOLESTR pName, DISPID* pDispid, LPVOID* ppVoid)
	{
		LPVOID pVoid = NULL;
		if (ppVoid) pVoid = *ppVoid;
		const DispMethodData* p = NULL;
		___repeat(15, __parent_map, __parent_map)
		if (ppVoid) *ppVoid = pVoid;
		return p;
	}
};

//////////////////////////////////////////////////////////////////////////
#define UseParamsMap 0,0

// 如果希望合并基类的映射表，...应该列举出基类
#define Begin_Disp_Map(classname, ...) \
protected: DispProvider<classname> __disp; \
public: \
	virtual IDispatch* GetDispatch() { return __disp.SetOwner(this), __disp.GetDispatch(); } \
	/*virtual IDispatch* GetThisDispatch() { return __disp.SetOwner(this), __disp.GetDispatch(); }*/ \
	static LPCOLESTR __ClassName() { return L#classname; } \
	static const DispMethodData* __GetDispMapEntry(LPCOLESTR pszByName=NULL/*find by name*/, DISPID* pByDispid=NULL/*find by dispid*/, LPVOID* ppVoid=NULL/*offset of parent*/) \
	{ \
		typedef classname owner_class; \
		typedef _ParentMapHelper<classname, __VA_ARGS__> parent_map_class; \
		static const DispMethodData __map_entry[] = {

#define Begin_Auto_Disp_Map(classname, ...) \
protected: DispProvider<classname, true> __disp; /*LPVOID* __ext_ref;*/ \
private: /*classname();*/ \
public: \
	virtual IDispatch* GetDispatch() { return __disp.GetDispatch(); } \
	/*virtual void SetExternalRef(LPVOID* ppref) { __ext_ref=ppref; }*/ \
	static LPCOLESTR __ClassName() { return L#classname; } \
	static void DeleteInstance(classname* p) { \
		/*if (p && p->__ext_ref) *p->__ext_ref=NULL;*/ \
		/* 类有自己删除实例的机会，只需实现 static void DeleteAutoInstance(classname* p) */ \
		__if_exists(classname::DeleteAutoInstance){classname::DeleteAutoInstance(p);} \
		__if_not_exists(classname::DeleteAutoInstance) {delete p;} \
	} \
	static classname* CreateInstance() \
	{ \
		IDispatch* pdisp=NULL; \
		classname* p = NULL; \
		CreateInstance(&pdisp, &p, TRUE); \
		return p; \
	} \
	static HRESULT CreateInstance(IDispatch** ppDisp, classname** ppOwner=NULL, BOOL bDetach=FALSE) \
	{ \
		if (ppOwner) *ppOwner = NULL; \
		if (ppDisp==NULL) return E_POINTER; \
		*ppDisp = NULL; \
		/* 类有自己创建实例的机会，只需实现 static classname* NewAutoInstance() */ \
		classname* pOwner = __if_exists(classname::NewAutoInstance){classname::NewAutoInstance();} __if_not_exists(classname::NewAutoInstance){NEW classname;} \
		if (pOwner==NULL) return E_OUTOFMEMORY; \
		pOwner->__disp._refcount = 0; /* 注意引用计数的初始值为1 */ \
		/*pOwner->__ext_ref = NULL;*/ \
		pOwner->__disp.SetOwner(pOwner); \
		if (!bDetach) pOwner->__disp.AddRef(); \
		if (ppOwner) *ppOwner = pOwner; \
		*ppDisp = (IDispatch*)&pOwner->__disp; \
		return S_OK; \
	} \
	static const DispMethodData* __GetDispMapEntry(LPCOLESTR pszByName=NULL/*find by name*/, DISPID* pByDispid=NULL/*find by dispid*/, LPVOID* ppVoid=NULL/*offset of parent*/) \
	{ \
		typedef classname owner_class; \
		typedef _ParentMapHelper<classname, __VA_ARGS__> parent_map_class; \
		static const DispMethodData __map_entry[] = {

#define Disp_PropertyGet(dispid, name, ...) \
			{OLESTR(#name), dispid, \
			__if_exists(owner_class::Get##name){_getfunc_helper(owner_class,name)} \
			__if_not_exists(owner_class::Get##name){ \
				__if_exists(owner_class::Get##name##Ref){_getref_helper(owner_class,name,__VA_ARGS__)} \
				__if_not_exists(owner_class::Get##name##Ref){ \
					__if_exists(owner_class::name){_get_helper(owner_class,name,__VA_ARGS__)} \
					__if_not_exists(owner_class::name){NULL \
						__pragma(message("WARNING: property '" #name "' can't be got, and will be ignored. FILE(" __FILE__ ") LINE(" __to_string(__LINE__) ")")) \
					} \
				} \
			}, \
			NULL, NULL, NULL},

#define Disp_PropertyGet_Member(dispid, name, member, ...) \
			{OLESTR(#name), dispid, \
			__if_exists(owner_class::Get##name){_getfunc_helper(owner_class,name)} \
			__if_not_exists(owner_class::Get##name){_get_helper(owner_class,member,__VA_ARGS__)}, \
			NULL, NULL, NULL},

#define Disp_ValueGet(name, ...) \
			{OLESTR(#name), DISPID_VALUE, \
			__if_exists(owner_class::Get##name){_getvalue_helper(owner_class,name)} \
			__if_not_exists(owner_class::Get##name){ \
				__pragma(message("WARNING: property '" #name "' can't be got, and will be ignored. FILE(" __FILE__ ") LINE(" __to_string(__LINE__) ")")) \
			}, \
			NULL, NULL, NULL},

#define Disp_PropertyPut(dispid, name, ...) /* ...==type */ \
			{OLESTR(#name), dispid, NULL, \
			__if_exists(owner_class::Set##name){_setfunc_helper(owner_class,name)} \
			__if_not_exists(owner_class::Set##name){ \
				__if_exists(owner_class::Put##name){_putfunc_helper(owner_class,name)} \
				__if_not_exists(owner_class::Put##name){ \
					__if_exists(owner_class::Get##name##Ref){_putref_helper(owner_class,name,__VA_ARGS__)} \
					__if_not_exists(owner_class::Get##name##Ref){ \
						__if_exists(owner_class::name){_put_helper(owner_class,name,__VA_ARGS__)} \
						__if_not_exists(owner_class::name){NULL \
							__pragma(message("WARNING: property '" #name "' can't be put, and will be ignored. FILE(" __FILE__ ") LINE(" __to_string(__LINE__) ")")) \
						} \
					} \
				} \
			}, \
			NULL, NULL},

#define Disp_PropertyPut_Member(dispid, name, member, ...) /* ...==type */ \
			{OLESTR(#name), dispid, NULL, \
			__if_exists(owner_class::Set##name){_setfunc_helper(owner_class,name)} \
			__if_not_exists(owner_class::Set##name){ \
				__if_exists(owner_class::Put##name){_putfunc_helper(owner_class,name)} \
				__if_not_exists(owner_class::Put##name){_put_helper(owner_class,member,__VA_ARGS__)} \
			}, \
			NULL, NULL},

#define Disp_Property(dispid, name, ...) /* ...==type */ \
			{OLESTR(#name), dispid, \
			__if_exists(owner_class::Get##name){_getfunc_helper(owner_class,name)} \
			__if_not_exists(owner_class::Get##name){ \
				__if_exists(owner_class::Get##name##Ref){_getref_helper(owner_class,name,__VA_ARGS__)} \
				__if_not_exists(owner_class::Get##name##Ref){ \
					__if_exists(owner_class::name){_get_helper(owner_class,name,__VA_ARGS__)} \
					__if_not_exists(owner_class::name){NULL \
						__pragma(message("WARNING: property '" #name "' can't be got, and will be ignored. FILE(" __FILE__ ") LINE(" __to_string(__LINE__) ")")) \
					} \
				} \
			}, \
			__if_exists(owner_class::Set##name){_setfunc_helper(owner_class,name)} \
			__if_not_exists(owner_class::Set##name){ \
				__if_exists(owner_class::Put##name){_putfunc_helper(owner_class,name)} \
				__if_not_exists(owner_class::Put##name){ \
					__if_exists(owner_class::Get##name##Ref){_putref_helper(owner_class,name,__VA_ARGS__)} \
					__if_not_exists(owner_class::Get##name##Ref){ \
						__if_exists(owner_class::name){_put_helper(owner_class,name,__VA_ARGS__)} \
						__if_not_exists(owner_class::name){NULL \
							__pragma(message("WARNING: property '" #name "' can't be put, and will be ignored. FILE(" __FILE__ ") LINE(" __to_string(__LINE__) ")")) \
						} \
					} \
				} \
			}, \
			NULL, NULL},

#define Disp_Property_Member(dispid, name, member, ...) /* ...==type */ \
			{OLESTR(#name), dispid, \
			__if_exists(owner_class::Get##name){_getfunc_helper(owner_class,name)} \
			__if_not_exists(owner_class::Get##name){_get_helper(owner_class,member,__VA_ARGS__) \
			}, \
			__if_exists(owner_class::Set##name){_setfunc_helper(owner_class,name)} \
			__if_not_exists(owner_class::Set##name){ \
				__if_exists(owner_class::Put##name){_putfunc_helper(owner_class,name)} \
				__if_not_exists(owner_class::Put##name){_put_helper(owner_class,member,__VA_ARGS__)} \
			}, \
			NULL, NULL},

#define Disp_Method(dispid, name, type, paramcnt, ...) /* ... == paramlist */ \
			__if_exists(owner_class::MethodParamsInfo::Call##name){ {OLESTR(#name), dispid, NULL, NULL, owner_class::MethodParamsInfo::Call##name, NULL}, } \
			__if_not_exists(owner_class::MethodParamsInfo::Call##name){ {OLESTR(#name), dispid, NULL, NULL, _method_helper(owner_class,name,type,paramcnt,__VA_ARGS__), NULL}, }

#define Disp_Method_Member(dispid, name, member, type, paramcnt, ...) /* ... == paramlist */ \
			__if_exists(owner_class::MethodParamsInfo::Call##member){ {OLESTR(#name), dispid, NULL, NULL, owner_class::MethodParamsInfo::Call##member, NULL}, } \
			__if_not_exists(owner_class::MethodParamsInfo::Call##member){ {OLESTR(#name), dispid, NULL, NULL, _method_helper(owner_class,member,type,paramcnt,__VA_ARGS__), NULL}, }

#define Disp_Method_Ext(dispid, name) \
			{OLESTR(#name), dispid, NULL, NULL, owner_class::MethodParamsInfo::Call##name, NULL},

#define Disp_Method_Member_Ext(dispid, name, member) \
			{OLESTR(#name), dispid, NULL, NULL, owner_class::MethodParamsInfo::Call##member, NULL},

#define Disp_Alias(dispid, alias) {OLESTR(#alias), DISPID_UNKNOWN, NULL, NULL, NULL, dispid},

#define End_Disp_Map() \
			{NULL, DISPID_UNKNOWN, NULL, NULL, NULL, NULL} \
		}; \
		if (pszByName==NULL && pByDispid==NULL) return __map_entry; \
		for (int i=0; i<sizeof(__map_entry)/sizeof(__map_entry[0]) - 1; i++) \
		{ \
			if (pByDispid) \
			{ \
				if (__map_entry[i].dispid == *pByDispid) return &__map_entry[i]; \
			} \
			else /*if (pszByName)*/ \
			{ \
				if (lstrcmpiW(__map_entry[i].name, pszByName) == 0) \
					return (__map_entry[i].dispid!=DISPID_UNKNOWN) ? &__map_entry[i] : __GetDispMapEntry(NULL, (DISPID*)&__map_entry[i].reserved, ppVoid); \
			} \
		} \
		return parent_map_class::__GetParentsMap(pszByName, pByDispid, ppVoid); \
	}

//////////////////////////////////////////////////////////////////////////
// 实现在目标结构/类外部定义一个引用类型的自动化实现类，它的作用是针对某个已经存在的结构体实例创建一个引用对象，脚本可利用这个对象直接修改实例成员
// 特点：
//		* 无需修改目标类
//		* 调用者可直接修改已存在实例的成员变量，或者调用其成员函数
//		* 如果目标类已经实现了内部的属性方法映射，则不必在外部重新填写表格，此时只需在类外部用 Implement_Ref_Creator(classname) 即可
//		* 如果目标类未实现内部属性方法映射，则必须用 Begin_Ref_Disp_Map/End_Ref_Disp_Map 在外部实现映射
//		* 如果目标类未实现内部映射，但存在一个实现了映射的派生类，可以使用 Implement_Ref_Creator 添加 0-N 个参考的派生类，这种情况下同意无需再次填表
// 限制：
//		* 不能实现继承基类映射表，但如果实现了内部映射，则具备继承特性
//		* 此实现仅作为 《针对已有类实例》 产生一个自动化引用对象，不能用于命名项，且创建对象时必须指定一个代表原对象的指针参数

//////////////////////////////////////////////////////////////////////////
// 参考映射的类继承工具，参考类必须是T的派生类（不是基类），否则无法调整this指针
#define __ref_map(n, ...) __if_exists(T##n::__GetDispMapEntry) { if (!p) p = T##n::__GetDispMapEntry(pName, pDispid); if (p) pVoid=(LPVOID)static_cast<T##n*>((T*)pVoid); }
#define __ref_cast(n, ...) __if_exists(T##n::__ClassName) { if (!p) p = static_cast<T*>((T##n*)Disp2Class(unk, T##n::__ClassName())); }
#define __ref_fire(n, ...) __if_exists(T##n::__GetDispMapEntry) { p = T##n::__GetDispMapEntry(NULL, &dispid); if (p) { __if_exists(T##n::OnPropertyChanged){ static_cast<T##n*>((T*)pVoid)->OnPropertyChanged(dispid,name); } return; } }
#define __ref_initfromstring(n, ...) __if_exists(T##n::InitFromString) { hr = static_cast<T##n*>(p)->InitFromString(szInit); if (SUCCEEDED(hr)) return hr; }
#define __reftparam16(n,...) typename T##n=int,
#define __reftparam16_end(n,...) typename T##n=int

template<class dispT, class T, ___repeat(15, __reftparam16, __reftparam16_end)>
class _RefMapHelper
{
public:
	static const DispMethodData* __GetRefMap(LPCOLESTR pName, DISPID* pDispid, LPVOID* ppVoid)
	{
		LPVOID pVoid = NULL;
		if (ppVoid) pVoid = *ppVoid;
		const DispMethodData* p = NULL;
		___repeat(15, __ref_map, __ref_map)
		if (ppVoid) *ppVoid = pVoid;
		return p;
	}
	static T* __Disp2Class(IUnknown* unk)
	{
		T* p = (T*)Disp2Class(unk, dispT::__ClassName());
		//__if_exists(T::__ClassName) { p = Disp2Class(unk, T::__ClassName()); }
		___repeat(15, __ref_cast, __ref_cast);
		return p;
	}
	static HRESULT __InitFromString(T* p, LPCOLESTR szInit)
	{
		HRESULT hr = E_FAIL;
		__if_exists(T::InitFromString) { hr = p->InitFromString(szInit); if (SUCCEEDED(hr)) return hr; }
		___repeat(15, __ref_initfromstring, __ref_initfromstring);
		return hr;
	}
	static void __FirePropertyChanged(LPVOID pVoid, DISPID dispid, LPCOLESTR name)
	{
		const DispMethodData* p = NULL;
		___repeat(15, __ref_fire, __ref_fire);
	}
};


#define Begin_Ref_Disp_Map(classname, ...) \
class RefCreator_##classname : public IDispatch, public IDispHost \
{ \
private: \
	typedef RefCreator_##classname thisClass; \
	classname* _owner; \
	ULONG _refcount; \
	RefCreator_##classname(classname* owner) : _owner(owner), _refcount(0) {} \
public: \
	typedef _RefMapHelper<thisClass, classname, __VA_ARGS__> ref_map_class; \
	static LPCOLESTR __ClassName() { return L#classname; } \
	/* IDispHost Methods */ \
	STDMETHOD_(LPVOID, GetOwner)() { return _owner; } \
	STDMETHOD_(LPCOLESTR, GetClassName)() { return __ClassName(); } \
	/* IDispatch Methods*/ \
	STDMETHOD_(ULONG, AddRef)() { return ++_refcount; } \
	STDMETHOD_(ULONG, Release)() \
	{ \
		if(--_refcount==0) return (delete this, 0); \
		return _refcount; \
	} \
	STDMETHOD(QueryInterface)(REFIID iid, LPVOID* ppvObj) \
	{ \
		if (!ppvObj) return E_POINTER; \
		*ppvObj = NULL; \
		if (IsEqualIID(iid, __uuidof(IUnknown)) || IsEqualIID(iid, __uuidof(IDispatch))) \
			*ppvObj = (IDispatch*)this; \
		else if (IsEqualIID(iid, IID_IDispHost)) \
			*ppvObj = (IDispHost*)this; \
		if (*ppvObj) \
			return AddRef(), S_OK; \
		return E_NOINTERFACE; \
	} \
	STDMETHOD(GetTypeInfoCount)(UINT *pctinfo) { *pctinfo=0; return E_NOTIMPL; } \
	STDMETHOD(GetTypeInfo)(UINT /*iTInfo*/, LCID /*lcid*/, ITypeInfo **ppTInfo) { *ppTInfo = NULL; return E_NOTIMPL; } \
	STDMETHOD(GetIDsOfNames)(REFIID riid, OLECHAR ** rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId) \
	{ \
		ATLASSERT(cNames == 1); \
		if (cNames != 1) return E_NOTIMPL; \
		*rgDispId = DISPID_UNKNOWN; \
		const DispMethodData* pMap = thisClass::__GetDispMapEntry(*rgszNames); \
		if (pMap) \
			return *rgDispId = pMap->dispid, S_OK; \
		return DISP_E_UNKNOWNNAME; \
	} \
	STDMETHOD(Invoke)(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pdispparams, VARIANT *pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr) \
	{ \
		LPVOID pVoid = _owner; \
		const DispMethodData* pMap = thisClass::__GetDispMapEntry(NULL, &dispIdMember, &pVoid); \
		if (dispIdMember==DISPID_VALUE && (wFlags==DISPATCH_PROPERTYGET) && pMap==NULL) \
		{ \
			CComVariant vRet = GetClassName(); \
			return vRet.Detach(pVarResult); \
		} \
		if (pMap) \
		{ \
			fnDispMethod pfn = (wFlags&DISPATCH_METHOD) ? pMap->pfnMethod : (wFlags==DISPATCH_PROPERTYGET) ? pMap->pfnGet : pMap->pfnPut; \
			if (pfn==NULL && (wFlags&DISPATCH_METHOD) && pMap->pfnPut && pdispparams->cArgs>=1) \
			{ \
				CComVariant vRet; \
				HRESULT hr = pMap->pfnPut(pVoid, pdispparams, &vRet, pMap->dispid, pMap->name); \
				if (FAILED(hr)) return hr; \
				if (pVarResult) \
				{ \
					vRet = (IDispatch*)this; \
					return vRet.Detach(pVarResult); \
				} \
				return S_OK; \
			} \
			if (pfn) \
				return pfn(pVoid/*_owner*/, pdispparams, pVarResult, pMap->dispid, pMap->name); \
		} \
		return DISP_E_MEMBERNOTFOUND; \
	} \
	static HRESULT CreateInstance(classname* pOwner, IDispatch** ppDisp, BOOL bZeroRefCount=FALSE) \
	{ \
		if (pOwner==NULL) return E_FAIL; \
		/* 如果目标类存在内部自动化对象，直接返回内部对象 */ \
		__if_exists(classname::GetDispatch) { return pOwner->GetDispatch()->QueryInterface(IID_IDispatch, (LPVOID*)ppDisp); } \
		__if_not_exists(classname::GetDispatch) \
		{ \
			if (ppDisp==NULL) return E_POINTER; \
			*ppDisp = NULL; \
			thisClass* pThis = NEW thisClass(pOwner); \
			if (pThis==NULL) return E_OUTOFMEMORY; \
			*ppDisp = (IDispatch*)pThis; \
			if (!bZeroRefCount) pThis->AddRef(); \
			return S_OK; \
		} \
	} \
	static const DispMethodData* __GetDispMapEntry(LPCOLESTR pszByName=NULL/*find by name*/, DISPID* pByDispid=NULL/*find by dispid*/, LPVOID* ppVoid=NULL/*offset of parent*/) \
	{ \
		__if_exists(classname::__GetDispMapEntry) { return classname::__GetDispMapEntry(pszByName,pByDispid,ppVoid); } \
		__if_not_exists(classname::__GetDispMapEntry) \
		{ \
			typedef classname owner_class; \
			static const DispMethodData __map_entry[] = {

#define End_Ref_Disp_Map() \
				{NULL, DISPID_UNKNOWN, NULL, NULL, NULL, NULL} \
			}; \
			if (pszByName==NULL && pByDispid==NULL) return __map_entry; \
			for (int i=0; i<sizeof(__map_entry)/sizeof(__map_entry[0]) - 1; i++) \
			{ \
				if (pByDispid) \
				{ \
					if (__map_entry[i].dispid == *pByDispid) return &__map_entry[i]; \
				} \
				else /*if (pszByName)*/ \
				{ \
					if (lstrcmpiW(__map_entry[i].name, pszByName) == 0) /*return &__map_entry[i];*/ \
						return (__map_entry[i].dispid!=DISPID_UNKNOWN) ? &__map_entry[i] : __GetDispMapEntry(NULL, (DISPID*)&__map_entry[i].reserved, ppVoid); \
				} \
			} \
			return ref_map_class::__GetRefMap(pszByName, pByDispid, ppVoid); \
		} \
	} \
};

//template<> static HRESULT RefCreateInstance<classname>(classname* pOwner, IDispatch** ppDisp) { return RefCreator_##classname::CreateInstance(pOwner, ppDisp); }

#define Implement_Ref_Creator(classname, ...) \
	Begin_Ref_Disp_Map(classname, __VA_ARGS__) \
	End_Ref_Disp_Map()

// 两个版本中，ppDisp 的类型应该是 IDispatch**，pVar 的类型应该是 VARIANT*
#define CreateRefInstance(classname, pOwner, ppDisp) RefCreator_##classname::CreateInstance((classname*)(pOwner), ppDisp)
#define CreateRefInstanceV(classname, pOwner, pVar) (((pVar)->vt=VT_DISPATCH), CreateRefInstance(classname, pOwner, &(pVar)->pdispVal))
// 慎用！！！以下两个版本中，创建返回对象后，引用计数保持为0，仅在特殊情况下使用
#define CreateRefInstanceWithZeroRefCount(classname, pOwner, ppDisp) RefCreator_##classname::CreateInstance((classname*)(pOwner), ppDisp, TRUE)
#define CreateRefInstanceWithZeroRefCountV(classname, pOwner, pVar) (((pVar)->vt=VT_DISPATCH), CreateRefInstanceWithZeroRefCount(classname, pOwner, &(pVar)->pdispVal))


//////////////////////////////////////////////////////////////////////////
// 简单类型集合对象模板，可以添加、修改、删除数据，对象本身可直接作为类成员以方便数组访问
template<typename T, typename refcreator=int>
class DispCollection : public ATL::CSimpleArray<T>
{
	static const DISPID IndexBegin = 20000;
public:
	HRESULT GetLength(VARIANT* val)
	{
		val->vt = VT_I4;
		val->lVal = GetSize();
		return S_OK;
	}
	bool AddObject(IDispatch* disp)
	{
		__if_exists(refcreator::ref_map_class) {
			T* pt = (T*)refcreator::ref_map_class::__Disp2Class(disp);
			if (pt)
			{
				return ATL::CSimpleArray<T>::Add(*pt) != FALSE;
			}
		}
		__if_not_exists(refcreator::ref_map_class) { __if_exists(T::__ClassName) {
			T* pt = (T*)Disp2Class(disp, T::__ClassName());
			if (pt)
			{
				return ATL::CSimpleArray<T>::Add(*pt) != FALSE;
			}
		}}
		return false;
	}
	bool AddValue(T tVal)
	{
		__if_not_exists(refcreator::ref_map_class) { __if_not_exists(T::__ClassName) {
			return ATL::CSimpleArray<T>::Add(tVal) != FALSE;
		}}
		return false;
	}
	void RemoveValue(long index)
	{
		if (index>=0 && index<GetSize())
			ATL::CSimpleArray<T>::RemoveAt(index);
	}

	BOOL __DispidOfName(LPCOLESTR szName, DISPID* pDispid)
	{
		// 仅处理索引值
		CComVariant vIdx = szName;
		if (SUCCEEDED(vIdx.ChangeType(VT_I4)) && vIdx.lVal>=0 && vIdx.lVal<GetSize())
			return *pDispid=(DISPID)vIdx.lVal+IndexBegin, TRUE;
		return FALSE;
	}

	HRESULT __Invoke(DISPID dispid, WORD wFlags, DISPPARAMS* pdispparams, VARIANT *pVarResult)
	{
		// 仅处理索引值
		if (dispid<IndexBegin || dispid>=IndexBegin+GetSize()) return DISP_E_MEMBERNOTFOUND;

		// 只能调用GET/PUT
		if (wFlags==DISPATCH_PROPERTYGET)
		{
			T& t = ATL::CSimpleArray<T>::operator[](dispid - IndexBegin);
			CComVariant vRet;
			__if_exists(T::GetDispatch) { vRet = t.GetDispatch(); }
			__if_not_exists(T::GetDispatch) {
				__if_exists(refcreator::CreateInstance) { vRet.vt=VT_DISPATCH; refcreator::CreateInstance(&t, &vRet.pdispVal); }
				__if_not_exists(refcreator::CreateInstance) { vRet = t; }
			}
			return vRet.Detach(pVarResult);
		}
		else if ((wFlags==DISPATCH_PROPERTYPUT || wFlags==DISPATCH_PROPERTYPUTREF))
		{
			if (pdispparams->cArgs==0) return DISP_E_PARAMNOTFOUND;
			T& t = ATL::CSimpleArray<T>::operator[](dispid - IndexBegin);
			__if_exists(refcreator::ref_map_class) {
				if (pdispparams->rgvarg->vt != VT_DISPATCH) return E_INVALIDARG;
				T* v = refcreator::ref_map_class::__Disp2Class(pdispparams->rgvarg->pdispVal); 
				if (v == NULL) return E_INVALIDARG;
				t = *v;
				return S_OK;
			}
			__if_not_exists(refcreator::ref_map_class) {
				__if_exists(T::__ClassName) {
					if (pdispparams->rgvarg->vt != VT_DISPATCH) return E_INVALIDARG;
					T* v = (T*)Disp2Class(pdispparams->rgvarg->pdispVal, T::__ClassName());
					if (v == NULL) return E_INVALIDARG;
					t = *v;
					return S_OK;
				}
				__if_not_exists(T::__ClassName) {
					HRESULT hr = CVarTypeInfoEx<T>::Assign(t, pdispparams->rgvarg);
					return hr;
				}
			}
		}
		return E_FAIL;
	}

	static const DispMethodData* __GetDispMapEntry(LPCOLESTR pszByName=NULL/*find by name*/, DISPID* pByDispid=NULL/*find by dispid*/, LPVOID* ppVoid=NULL/*offset of parent*/)
	{ 
		typedef DispCollection<T,refcreator> owner_class; 
		static const DispMethodData __map_entry[] = {

			Disp_PropertyGet(10001, Length)
			__if_exists(refcreator::ref_map_class) {
				Disp_Method_Member(10010, Add, AddObject, bool, 1, IDispatch*)
			}
			__if_not_exists(refcreator::ref_map_class) { __if_exists(T::__ClassName) {
				Disp_Method_Member(10010, Add, AddObject, bool, 1, IDispatch*)
			}}
			__if_not_exists(refcreator::ref_map_class) { __if_not_exists(T::__ClassName) {
				Disp_Method_Member(10010, Add, AddValue, bool, 1, T)
			}}
			Disp_Method_Member(10011, Remove, RemoveValue, void, 1, long)

			{NULL, DISPID_UNKNOWN, NULL, NULL, NULL, NULL} 
		}; 
		if (pszByName==NULL && pByDispid==NULL) return __map_entry; 
		for (int i=0; i<sizeof(__map_entry)/sizeof(__map_entry[0]) - 1; i++) 
		{ 
			if (pByDispid) 
			{ 
				if (__map_entry[i].dispid == *pByDispid) return &__map_entry[i]; 
			} 
			else /*if (pszByName)*/ 
			{ 
				if (lstrcmpiW(__map_entry[i].name, pszByName) == 0) return &__map_entry[i]; 
			} 
		} 
		return NULL; 
	}
};

#define Implement_DispCollection(classname, type) \
	class classname : public DispCollection<type __if_exists(RefCreator_##type){,RefCreator_##type}> \
	{ \
	public: \
		Begin_Disp_Map(classname, DispCollection<type __if_exists(RefCreator_##type){,RefCreator_##type}>) \
		End_Disp_Map() \
	};

//////////////////////////////////////////////////////////////////////////
// ArrayRefCreator 是一个数组自动化类，它引用已有的数组缓冲区，使该缓冲区的内容能被自动化读取和设置
// 特性：
//	* 缓冲区指针和元素个数由外部指定，必须在 CreateInstance 时指定此参数
//	* 不支持添加和删除元素
//	* 支持元素项的读取和设置
//	* 仅支持 Length 只读属性和数字索引式访问
template<typename T, typename refcreator=int>
class ArrayRefCreator : public IDispatch
{
	typedef ArrayRefCreator<T,refcreator> thisClass;
private:
	T* _buf; // 外部传入的缓冲区指针
	UINT _elements; // 缓冲区中元素的个数
	ULONG _refcount;

	ArrayRefCreator(LPVOID buffer, UINT elements) : _buf((T*)buffer), _elements(elements), _refcount(1) {}

public:
	void operator =(const ArrayRefCreator<T,refcreator>& other) {} // 禁止赋值

	/* IDispatch Methods*/
	STDMETHOD_(ULONG, AddRef)() { return ++_refcount; }
	STDMETHOD_(ULONG, Release)()
	{
		if(--_refcount==0)
		{
			delete this;
			return 0;
		};
		return _refcount;
	}
	STDMETHOD(QueryInterface)(REFIID iid, LPVOID* ppvObj)
	{
		if (!_buf) return E_UNEXPECTED;
		if (!ppvObj) return E_POINTER;
		*ppvObj = NULL;
		if (IsEqualIID(iid, __uuidof(IUnknown)) ||
			IsEqualIID(iid, __uuidof(IDispatch)))
			*ppvObj = (IDispatch*)this;
		if (*ppvObj)
		{
			AddRef();
			return S_OK;
		}
		return E_NOINTERFACE;
	}
	STDMETHOD(GetTypeInfoCount)(UINT *pctinfo) { *pctinfo=0; return E_NOTIMPL; }
	STDMETHOD(GetTypeInfo)(UINT /*iTInfo*/, LCID /*lcid*/, ITypeInfo **ppTInfo) { *ppTInfo = NULL; return E_NOTIMPL; }
	STDMETHOD(GetIDsOfNames)(REFIID riid, OLECHAR ** rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId)
	{
		ATLASSERT(cNames == 1);
		if (cNames != 1) return E_NOTIMPL;
		if (!_buf) return E_UNEXPECTED;

		*rgDispId = DISPID_UNKNOWN;
		if (lstrcmpiW(*rgszNames, L"length")==0)
			return (*rgDispId=1), S_OK;

		CComVariant vIdx = *rgszNames;
		if (SUCCEEDED(vIdx.ChangeType(VT_UI4)) && vIdx.ulVal<_elements)
			return (*rgDispId=(DISPID)vIdx.ulVal+10), S_OK;

		return DISP_E_UNKNOWNNAME;
	}
	STDMETHOD(Invoke)(DISPID dispid, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pdispparams, VARIANT *pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr)
	{
		if (!_buf) return E_UNEXPECTED;

		if (dispid==1 && wFlags==DISPATCH_PROPERTYGET)
		{
			pVarResult->vt = VT_I4;
			pVarResult->lVal = (LONG)_elements;
			return S_OK;
		}

		// 仅处理索引值
		if (dispid<10 || dispid>=(LONG)_elements+10) return DISP_E_MEMBERNOTFOUND;

		T& t = _buf[dispid - 10];
		// 只能调用GET/PUT
		if (wFlags==DISPATCH_PROPERTYGET)
		{
			CComVariant vRet;
			__if_exists(T::GetDispatch) { vRet = t.GetDispatch(); }
			__if_not_exists(T::GetDispatch) {
				__if_exists(refcreator::CreateInstance) { vRet.vt=VT_DISPATCH; refcreator::CreateInstance(&t, &vRet.pdispVal); }
				__if_not_exists(refcreator::CreateInstance) { vRet = t; }
			}
			return vRet.Detach(pVarResult);
		}
		else if ((wFlags==DISPATCH_PROPERTYPUT || wFlags==DISPATCH_PROPERTYPUTREF))
		{
			if (pdispparams->cArgs==0) return DISP_E_PARAMNOTFOUND;
			__if_exists(refcreator::ref_map_class) {
				if (pdispparams->rgvarg->vt != VT_DISPATCH) return E_INVALIDARG;
				T* v = refcreator::ref_map_class::__Disp2Class(pdispparams->rgvarg->pdispVal); 
				if (v == NULL) return E_INVALIDARG;
				t = *v;
				return S_OK;
			}
			__if_not_exists(refcreator::ref_map_class) {
				__if_exists(T::__ClassName) {
					if (pdispparams->rgvarg->vt != VT_DISPATCH) return E_INVALIDARG;
					T* v = (T*)Disp2Class(pdispparams->rgvarg->pdispVal, T::__ClassName());
					if (v == NULL) return E_INVALIDARG;
					t = *v;
					return S_OK;
				}
				__if_not_exists(T::__ClassName) {
					HRESULT hr = CVarTypeInfoEx<T>::Assign(t, pdispparams->rgvarg);
					return hr;
				}
			}
		}

		return DISP_E_MEMBERNOTFOUND;
	}

	static HRESULT CreateInstance(LPVOID buffer, UINT elements, VARIANT* pResult)
	{
		if (buffer==NULL || pResult==NULL) return E_UNEXPECTED;

		thisClass* pThis = new thisClass(buffer, elements);
		if (pThis==NULL) return E_OUTOFMEMORY;
		pResult->vt = VT_DISPATCH;
		pResult->pdispVal = (IDispatch *)pThis;
		return S_OK;
	}
};

#define ArrayRef(classname) ArrayRefCreator<classname __if_exists(RefCreator_##classname){,RefCreator_##classname}>


//////////////////////////////////////////////////////////////////////////
// 辅助工具
// 
class DispParams
{
	VARIANT *_argv;
	UINT _argc; // 实际要计算的参数个数，默认跟_dp->cArgs相同。
public:
	DispParams(DISPPARAMS* dp, UINT nonoptional=0) : _argv(NULL), _argc(0)
	{
		// 如果nonoptional > _dp->cArgs，则设置无效
		if (dp)
		{
			_argv = dp->rgvarg;
			if (nonoptional > dp->cArgs) nonoptional = 0;
			_argc = dp->cArgs - nonoptional;
		}
	}

	UINT Count() const { return _argc; }

	bool IsEmpty(UINT index) const
	{
		if (_argv==NULL || index>=_argc) return true;
		switch (_argv[_argc-index-1].vt)
		{
		case VT_NULL:
		case VT_EMPTY:
		case VT_ERROR: return true;
		}
		return false;
	}

	VARIANT* operator[] (UINT index)
	{
		if (_argv==NULL || index>=_argc) return NULL;
		return &_argv[_argc-index-1];
	}

	bool GetLong(UINT index, LONG& l)
	{
		if (IsEmpty(index)) return false;
		CComVariant v;
		if (SUCCEEDED(v.ChangeType(VT_I4, &_argv[_argc-index-1])))
			return l=V_I4(&v), true;
		return false;
	}

	bool GetBool(UINT index, BOOL& b)
	{
		if (IsEmpty(index)) return false;
		if (_argv[_argc-index-1].vt==VT_BSTR)
		{
			LPCOLESTR lpszData = _argv[_argc-index-1].bstrVal;
			if (lstrcmpiW(lpszData, L"true")==0 ||
				lstrcmpiW(lpszData, L"yes")==0 ||
				lstrcmpiW(lpszData, L"1")==0)
				b = TRUE;
			else b = FALSE;
			return true;
		}

		CComVariant v;
		if (SUCCEEDED(v.ChangeType(VT_BOOL, &_argv[_argc-index-1])))
			return b=(V_BOOL(&v)!=VARIANT_FALSE), true;
		return false;
	}

	bool GetBool(UINT index, bool& b)
	{
		BOOL B;
		if (GetBool(index, B))
			return (b=!!B), true;
		return false;
	}

	bool GetDouble(UINT index, DOUBLE& d)
	{
		if (IsEmpty(index)) return false;
		CComVariant v;
		if (SUCCEEDED(v.ChangeType(VT_R8, &_argv[_argc-index-1])))
			return d=V_R8(&v), true;
		return false;
	}

	bool GetFloat(UINT index, FLOAT& f)
	{
		if (IsEmpty(index)) return false;
		CComVariant v;
		if (SUCCEEDED(v.ChangeType(VT_R4, &_argv[_argc-index-1])))
			return f=V_R4(&v), true;
		return false;
	}

	bool GetBstr(UINT index, BSTR* str)
	{
		if (IsEmpty(index)) return false;
		CComVariant v;
		if (SUCCEEDED(v.ChangeType(VT_BSTR, &_argv[_argc-index-1])))
		{
			if (str) v.CopyTo(str);
			return true;
		}
		return false;
	}
};

//////////////////////////////////////////////////////////////////////////
class CComResultTool
{
	struct __context
	{
		DWORD threadid;
		LPCSTR file;
		DWORD line;
		BOOL check;

		__context() : threadid(-1), file(NULL), line(-1), check(FALSE) {}
		void remember(LPCSTR f, DWORD l) { file=f; line=l; check=TRUE; }
	};
	class __cs
	{
	public:
		CRITICAL_SECTION cs;
		__cs() { InitializeCriticalSection(&cs); }
		~__cs() { DeleteCriticalSection(&cs); }
	};
	class __lock
	{
		__cs& _cs;
	public:
		__lock(__cs& cs) : _cs(cs) { EnterCriticalSection(&_cs.cs); }
		~__lock() { LeaveCriticalSection(&_cs.cs); }
	};
public:
	HRESULT _hr;

	CComResultTool(HRESULT hr=S_OK) : _hr(hr) {}

	static __context& Context()
	{
		// 最多8个线程
		static __context Contexts[8];
		static __cs cs;

		DWORD tid = ::GetCurrentThreadId();
		__lock lock(cs);
		for (int i=0; i<8; i++)
		{
			if (Contexts[i].threadid==tid)
				return Contexts[i];
		}
		for (int i=0; i<8; i++)
		{
			if (Contexts[i].threadid==-1)
			{
				Contexts[i].threadid = tid;
				return Contexts[i];
			}
		}
		throw L"Out of thread count!";
	}

	CComResultTool& operator= (HRESULT hr)
	{
		_hr = hr;
		__context& c = Context();
		if (FAILED(_hr))
		{
			LPSTR pMsg=NULL;
			::FormatMessageA( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, _hr,
						MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
						(LPSTR)&pMsg, 0, NULL);
			if (pMsg)
			{
				CAtlStringA str;
				str.Format("File: %s\nLine: %d\n\nDescription: %s", c.file, c.line, pMsg);
				::MessageBoxA(NULL, str, "Com Invoking Error", MB_OK | MB_ICONERROR);
				::LocalFree( pMsg );
			}
		}
		c.check = FALSE;
		return *this;
	}

	operator HRESULT() { return _hr; }
};

#ifdef _DEBUG
#define CComResult CComResultTool
#define CheckHResult CComResultTool::Context().remember(__FILE__, __LINE__),
#else
#define CComResult HRESULT
#define CheckHResult
#endif

#pragma warning(pop)

#endif // __MACRO_H__
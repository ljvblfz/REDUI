#ifndef __DUI3DANIMATION_H__
#define __DUI3DANIMATION_H__

#include "DuiD3DImpl.h"

class CCubic;
class iAttributeData;
class iStoryBoard;
	class iFrameSetBase;
		class iKeyFrameSet;
		class iFrameSet; // iFrameSetBase + iKeyFrame
	class iKeyFrame;


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
// 曲线插值计算类
class CCubic
{
public:
	float m_fa;
	float m_fb;
	float m_fc;
	float m_fd;
	float m_fe;

	CCubic(void) : m_fa(0), m_fb(0), m_fc(0), m_fd(0), m_fe(0) {}
	CCubic(float a, float b, float c, float d, float e) : m_fa(a), m_fb(b), m_fc(c), m_fd(d), m_fe(e) {}
	//~CCubic(void){};

	// u = [0..1] 计算曲线样条值，m_fe 忽略
	float eval(float u)
	{
		return ( (((m_fd * u) + m_fc) * u + m_fb) * u + m_fa);
	};
	// 计算折线样条值，m_fe 起作用
	float line(float u)
	{
		return ( (m_fe - m_fa) * u + m_fa);
	};
};

//////////////////////////////////////////////////////////////////////////

// 动画属性值，值类型由参数提供
class iAttributeData : public CComVariant
{
public:
	// 属性数据类型，如果是普通类型（比如VT_I4、VT_R4），则adt跟vt的值相同；
	// 如果是多元数结构类型，则vt==VT_DISPATCH,adt==__ClassName(class)
	// 如果adt==NULL，则表示此值尚未初始化类型
	LPCOLESTR adt;

	iAttributeData() : adt(NULL) {}

	void operator = (const iAttributeData& other) { CopyFrom(&other); adt=other.adt; }

	void Create(LPCOLESTR type); // Create 和 InitFromString 两者都应该各调用一次，但无顺序要求
	bool InitFromString(LPCOLESTR szInit);
	bool CopyFrom(const VARIANT* pSrc); // 从 VARIANT 复制数据，不自动Create
	bool Sum(iAttributeData* src, iAttributeData* src2); // 求和，结果保存在当前对象中，自动创建
	bool Calc(iAttributeData* from, iAttributeData* to, float percent, CCubic* c0=NULL, CCubic* c1=NULL, CCubic* c2=NULL, CCubic* c3=NULL); // Calc 会自动调用Create
	static ULONG GetSplineDim(LPCOLESTR type); // 获取元数据个数，简单类型一般是1，最多4
	bool SetSplineSample(float* s0, float* s1=NULL, float* s2=NULL, float* s3=NULL);
};

// 关键帧，代表一个关键帧时间和值的定义
class iKeyFrame : public KeyFrame
	, public TOrderList<iKeyFrame,true,0,float> // 保存在 KeyFrameSet 中
{
public:
	iKeyFrameSet* m_keyframe_set; // 所属关键帧集
	float m_keytime; // 关键帧时间，格式 [[h:]m:]s
	iAttributeData m_keyvalue; // 关键帧属性值
	CCubic* m_cubic[4]; // 当前帧到下一帧的样条算法，由于非闭合，最后一帧无此值。此值由关键帧集设置，由关键帧集统一分配释放，无须在此释放。最多操作4个原数

	iKeyFrame(iKeyFrameSet* kfs=NULL);
	float key_time() { return m_keytime; }

	void createAttributeData(LPCOLESTR type);

	void setSpline(ULONG count, ULONG index, CCubic* cubics0=NULL, CCubic* cubics1=NULL, CCubic* cubics2=NULL, CCubic* cubics3=NULL);
	//void Dispose() { delete this; }
};

// 关键帧集，绑定一个对象属性
class iFrameSetBase : public TList<iFrameSetBase> // 保存在 StoryBoard 中
{
public:
	iStoryBoard* m_story;

	CComVariant m_target; // 绑定的对象，可以是名称，也可以是对象，不能为空
	CComBSTR m_target_attribute; // 绑定的对象属性，不能为空
	CComBSTR m_datatype; // 对象属性值的类型，例如 "float"
	
	bool m_reverse; // 是否自动翻转，默认 false
	bool m_closure; // 是否闭合，默认false，通过 Reverse = 'closure' 设置闭合模式，不直接提供属性
	ULONG m_repeat; // 重复次数，0表示永远重复，默认 1
	float m_delay; // 动画延迟启动时间，默认 0
	float m_idle; // 重复之间的暂停时间，即要重复之前先保持多久时间，默认 0

	iFrameSetBase(iStoryBoard* sb=NULL);
	virtual ~iFrameSetBase();
	
	struct __tagRuntime
	{
		bool finished; // 运行状态，表示动画已完成，对于永远重复的动画无效
		ULONG count; // 已完成次数
		bool reversing; // 是否正在翻转
		iAttributeData vOld; // 缓存的最后值，方便设置时判断是否相同
		iAttributeData vCurr, vFinished; // 如果状态是 STOP_PENDING，则执行 Stop 时要缓存这两个值
	} __runtime;
	void clearRuntime();

	virtual void step(float fTimeDelta) = 0;
	virtual void onstart();
	virtual void onstop() = 0;
	virtual void onstopPending() = 0;
	virtual IDispatch* getDispatch() = 0;
	virtual LPCOLESTR getName() = 0;

	//void Dispose() { getDispatch()->Release(); }
	void unload();
	bool isFinished();

protected:
	CComDispatchDriver _get_object();
	HRESULT _set_property(iAttributeData* val);
	HRESULT _get_property(VARIANT* val);
};

class iKeyFrameSet : public KeyFrameSet
	, public iFrameSetBase
{
public:
	iKeyFrame* m_keyframes; // 关键帧的集合
	bool m_spline; // 是否使用B样条
	CCubic* m_cubics[4]; // 最多4组样条集，只有关键帧集才支持B样条，每个关键帧都作为采样点

	iKeyFrameSet(iStoryBoard* sb=NULL);
	~iKeyFrameSet();

	float get_duration()
	{
		if (m_keyframes==NULL || m_keyframes->Next()==NULL) return 0;
		return m_keyframes->getAt(m_keyframes->count()-1)->m_keytime;
	}

	virtual void step(float fTimeDelta);
	virtual void onstart();
	virtual void onstop();
	virtual void onstopPending();
	virtual IDispatch* getDispatch() { return KeyFrameSet::GetDispatch(); }
	virtual LPCOLESTR getName() { return Name; }

//private:
	void _clearSpline();
	void _createSpline();
};

// From/To/By 动画类型的帧类型，同时是关键帧集
class iFrameSet : public FrameSet
	, public iFrameSetBase // 作为一种关键帧集
	//, public iKeyFrame // 作为一种关键帧
{
public:
	iAttributeData m_from, m_to, m_by; // From/To/By 值
	DWORD m_has_from:1; // From/To/By值是否已初始化，如果没有初始化，可能需要从对象读取
	DWORD m_has_to:1;
	DWORD m_has_by:1;
	float m_duration; // 周期，默认 1 秒

	iFrameSet(iStoryBoard* sb=NULL);
	~iFrameSet();

	virtual void step(float fTimeDelta);
	virtual void onstart();
	virtual void onstop();
	virtual void onstopPending();
	virtual IDispatch* getDispatch() { return FrameSet::GetDispatch(); }
	virtual LPCOLESTR getName() { return Name; }

	//virtual void Dispose() { delete this; }
};

// 动画演示板，表示一个动画
class iStoryBoard : public StoryBoard
	, public TList<iStoryBoard> // 保存在设备中
{
public:
	iDevice* m_device;
	iFrameSetBase* m_keyframe_sets; // 关键帧集的集合

	bool m_started;	// 时间线是否已启动
	bool m_stopped; // 时间线是否已停止
	bool m_paused;	// 时间线是否已暂停
	// 
	float m_current_time;	// 当前时间。一旦动画开始播放，此时间将置0并逐步增加

	CComVariant m_onstart, m_onstop, m_onpause, m_onresume, m_onstep; // 脚本设置的回调

	// 停止行为类型
	enum
	{
		stop_current = 0,
		stop_beginNow,
		stop_endNow,
		stop_begin,
		stop_end
	};
	int m_stop_behavior;
	bool m_stop_pending; // 是否正在执行停止动画
	float m_stop_time; // 停止动画所需时间

public:
	iStoryBoard(iDevice* device=NULL);
	~iStoryBoard();

	// 判断动画是否正在运行。如果动画在运行，将启动渲染定时器；否则，终止定时器
	bool isRunning(bool bMeOnly=false);
	// step是最重要的方法，每次设备渲染之前先要执行此方法完成属性动画的设置操作
	void step(float fTimeDelta, bool bNext=true);

	void start();
	void stop(int stopBehavior=0, float stopTime=1.f);
	void pause();
	void resume();

	//void Dispose() { delete this; }
	void unload();
};



#endif // __DUI3DANIMATION_H__
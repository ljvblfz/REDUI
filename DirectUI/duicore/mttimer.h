#ifndef __MTTIMER_H__
#define __MTTIMER_H__


//////////////////////////////////////////////////////////////////////////
// multi thread timer
typedef void (CALLBACK* pfnTimerProc)(LPVOID pData, DWORD dwId);

/*
 *	dwPeriod	- 定时器周期，单位毫秒
	dwCount		- 触发次数，如果是0则表示一直触发。如果触发次数达到，定时器任务自动销毁，无需调用 KillMtTimer
	proc		- 回调
	pData		- 用户数据，回调函数的第一个参数
	返回值：	- 定时器任务 ID，回调函数的第二个参数；KillMtTimer的参数。
				  如果增加定时器任务成功，返回值为任务ID（0也为有效ID）。如果失败，返回值 -1 (0xFFFFFFFF)
 */
DWORD SetMtTimer(DWORD dwCookie, DWORD dwPeriod, DWORD dwCount, pfnTimerProc proc, LPVOID pData);
void KillMtTimer(DWORD dwTimerId);

/*
 *	当需要时钟的线程结束前，需要调用此方法清理定时器数据
 */
void OnEndOfThread();

//class MtTimer
//{
//public:
//	static DWORD NewTimer(DWORD dwPeriod/*ms*/, pfnTimerProc proc, LPVOID pData, DWORD dwCount=0); // 返回ID，如果ID是0xFFFFFFFF表示失败
//};


#endif // __MTTIMER_H__
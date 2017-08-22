#ifndef __DUID3DHELPER_H__
#define __DUID3DHELPER_H__

#include <d3dx9.h>
#include "DuiBase.h"

//////////////////////////////////////////////////////////////////////////
#define StateItemImpl0(dev_set, dev_get, value_type) \
	class StateItem_##dev_set : public StateItemBase \
	{ \
		bool applied; \
		struct \
		{ \
			value_type out_old; \
			value_type out_new; \
		} buf; \
	public: \
		StateItem_##dev_set(StateItemBase** head, value_type newval) : StateItemBase(head), applied(false) \
		{ memset(&buf, 0, sizeof(buf)); buf.out_new=newval; } \
		void Release() { delete this; } \
		virtual HRESULT Apply(IDirect3DDevice9* device) \
		{ \
			device->dev_get(&buf.out_old); \
			HRESULT hr = device->dev_set(buf.out_new); \
			applied = SUCCEEDED(hr); \
			return hr; \
		} \
		virtual HRESULT Restore(IDirect3DDevice9* device) \
		{ \
			return applied ? device->dev_set(buf.out_old) : S_FALSE; \
		} \
	};

#define StateItemImpl1(dev_set, dev_get, param_type, value_type) \
	class StateItem_##dev_set : public StateItemBase \
	{ \
		bool applied; \
		struct \
		{ \
			param_type in1; \
			value_type out_old; \
			value_type out_new; \
		} buf; \
	public: \
		StateItem_##dev_set(StateItemBase** head, param_type in1, value_type newval) : StateItemBase(head), applied(false) \
		{ memset(&buf, 0, sizeof(buf)); buf.in1=in1; buf.out_new=newval; } \
		void Release() { delete this; } \
		virtual HRESULT Apply(IDirect3DDevice9* device) \
		{ \
			device->dev_get(buf.in1, &buf.out_old); \
			HRESULT hr = device->dev_set(buf.in1, buf.out_new); \
			applied = SUCCEEDED(hr); \
			return hr; \
		} \
		virtual HRESULT Restore(IDirect3DDevice9* device) \
		{ \
			return applied ? device->dev_set(buf.in1, buf.out_old) : S_FALSE; \
		} \
	};

#define StateItemImpl2(dev_set, dev_get, param_type1, param_type2, value_type) \
	class StateItem_##dev_set : public StateItemBase \
	{ \
		bool applied; \
		struct \
		{ \
			param_type1 in1; \
			param_type2 in2; \
			value_type out_old; \
			value_type out_new; \
		} buf; \
	public: \
		StateItem_##dev_set(StateItemBase** head, param_type1 in1, param_type2 in2, value_type newval) : StateItemBase(head), applied(false) \
		{ memset(&buf, 0, sizeof(buf)); buf.in1=in1; buf.in2=in2; buf.out_new=newval; } \
		void Release() { delete this; } \
		virtual HRESULT Apply(IDirect3DDevice9* device) \
		{ \
			device->dev_get(buf.in1, buf.in2, &buf.out_old); \
			HRESULT hr = device->dev_set(buf.in1, buf.in2, buf.out_new); \
			applied = SUCCEEDED(hr); \
			return hr; \
		} \
		virtual HRESULT Restore(IDirect3DDevice9* device) \
		{ \
			return applied ? device->dev_set(buf.in1, buf.in2, buf.out_old) : S_FALSE; \
		} \
	};

// struct
#define StateItemImpl0_Struct(dev_set, dev_get, value_type) \
	class StateItem_##dev_set : public StateItemBase \
	{ \
		bool applied; \
		struct \
		{ \
			value_type out_old; \
			value_type* out_new; \
		} buf; \
	public: \
		StateItem_##dev_set(StateItemBase** head, value_type* newval) : StateItemBase(head), applied(false) \
		{ memset(&buf, 0, sizeof(buf)); buf.out_new=newval; } \
		void Release() { delete this; } \
		virtual HRESULT Apply(IDirect3DDevice9* device) \
		{ \
			device->dev_get(&buf.out_old); \
			HRESULT hr = device->dev_set(buf.out_new); \
			applied = SUCCEEDED(hr); \
			return hr; \
		} \
		virtual HRESULT Restore(IDirect3DDevice9* device) \
		{ \
			return applied ? device->dev_set(&buf.out_old) : S_FALSE; \
		} \
	};

#define StateItemImpl1_Struct(dev_set, dev_get, param_type, value_type) \
	class StateItem_##dev_set : public StateItemBase \
	{ \
		bool applied; \
		struct \
		{ \
			param_type in1; \
			value_type out_old; \
			value_type* out_new; \
		} buf; \
	public: \
		StateItem_##dev_set(StateItemBase** head, param_type in1, value_type* newval) : StateItemBase(head), applied(false) \
		{ memset(&buf, 0, sizeof(buf)); buf.in1=in1; buf.out_new=newval; } \
		void Release() { delete this; } \
		virtual HRESULT Apply(IDirect3DDevice9* device) \
		{ \
			device->dev_get(buf.in1, &buf.out_old); \
			HRESULT hr = device->dev_set(buf.in1, buf.out_new); \
			applied = SUCCEEDED(hr); \
			return hr; \
		} \
		virtual HRESULT Restore(IDirect3DDevice9* device) \
		{ \
			return applied ? device->dev_set(buf.in1, &buf.out_old) : S_FALSE; \
		} \
	};

#define StateItemImpl2_Struct(dev_set, dev_get, param_type1, param_type2, value_type) \
	class StateItem_##dev_set : public StateItemBase \
	{ \
		bool applied; \
		struct \
		{ \
			param_type1 in1; \
			param_type2 in2; \
			value_type out_old; \
			value_type* out_new; \
		} buf; \
	public: \
		StateItem_##dev_set(StateItemBase** head, param_type1 in1, param_type2 in2, value_type* newval) : StateItemBase(head), applied(false) \
		{ memset(&buf, 0, sizeof(buf)); buf.in1=in1; buf.in2=in2; buf.out_new=newval; } \
		void Release() { delete this; } \
		virtual HRESULT Apply(IDirect3DDevice9* device) \
		{ \
			device->dev_get(buf.in1, buf.in2, &buf.out_old); \
			HRESULT hr = device->dev_set(buf.in1, buf.in2, buf.out_new); \
			applied = SUCCEEDED(hr); \
			return hr; \
		} \
		virtual HRESULT Restore(IDirect3DDevice9* device) \
		{ \
			return applied ? device->dev_set(buf.in1, buf.in2, buf.out_old) : S_FALSE; \
		} \
	};

// Ptr
#define StateItemImpl0_Ptr(dev_set, dev_get, iface_type) \
	class StateItem_##dev_set : public StateItemBase \
	{ \
		bool applied; \
		struct \
		{ \
			CComPtr<iface_type> out_old; \
			iface_type* out_new; \
		} buf; \
	public: \
		StateItem_##dev_set(StateItemBase** head, iface_type* newval) : StateItemBase(head), applied(false) \
		{ /*memset(&buf, 0, sizeof(buf));*/ buf.out_new=newval; } \
		void Release() { delete this; } \
		virtual HRESULT Apply(IDirect3DDevice9* device) \
		{ \
			device->dev_get(&buf.out_old); \
			HRESULT hr = device->dev_set(buf.out_new); \
			applied = SUCCEEDED(hr); \
			return hr; \
		} \
		virtual HRESULT Restore(IDirect3DDevice9* device) \
		{ \
			return applied ? device->dev_set(buf.out_old) : S_FALSE; \
		} \
	};

#define StateItemImpl1_Ptr(dev_set, dev_get, param_type, iface_type) \
	class StateItem_##dev_set : public StateItemBase \
	{ \
		bool applied; \
		struct \
		{ \
			param_type in1; \
			CComPtr<iface_type> out_old; \
			iface_type* out_new; \
		} buf; \
	public: \
		StateItem_##dev_set(StateItemBase** head, param_type in1, iface_type* newval) : StateItemBase(head), applied(false) \
		{ /*memset(&buf, 0, sizeof(buf));*/ buf.in1=in1; buf.out_new=newval; } \
		void Release() { delete this; } \
		virtual HRESULT Apply(IDirect3DDevice9* device) \
		{ \
			device->dev_get(buf.in1, &buf.out_old); \
			HRESULT hr = device->dev_set(buf.in1, buf.out_new); \
			applied = SUCCEEDED(hr); \
			return hr; \
		} \
		virtual HRESULT Restore(IDirect3DDevice9* device) \
		{ \
			return applied ? device->dev_set(buf.in1, buf.out_old) : S_FALSE; \
		} \
	};

#define StateItemImpl2_Ptr(dev_set, dev_get, param_type1, param_type2, iface_type) \
	class StateItem_##dev_set : public StateItemBase \
	{ \
		bool applied; \
		struct \
		{ \
			param_type1 in1; \
			param_type2 in2; \
			CComPtr<iface_type> out_old; \
			iface_type* out_new; \
		} buf; \
	public: \
		StateItem_##dev_set(StateItemBase** head, param_type1 in1, param_type2 in2, iface_type* newval) : StateItemBase(head), applied(false) \
		{ /*memset(&buf, 0, sizeof(buf));*/ buf.in1=in1; buf.in2=in2; buf.out_new=newval; } \
		void Release() { delete this; } \
		virtual HRESULT Apply(IDirect3DDevice9* device) \
		{ \
			device->dev_get(buf.in1, buf.in2, &buf.out_old); \
			HRESULT hr = device->dev_set(buf.in1, buf.in2, buf.out_new); \
			applied = SUCCEEDED(hr); \
			return hr; \
		} \
		virtual HRESULT Restore(IDirect3DDevice9* device) \
		{ \
			return applied ? device->dev_set(buf.in1, buf.in2, buf.out_old) : S_FALSE; \
		} \
	};

#define StateItemImpl_Constant(func, param_type) \
	class StateItem_Set##func : public StateItemBase \
	{ \
		bool applied; \
		struct \
		{ \
			UINT StartRegister; \
			UINT ParamCount; \
			param_type* pConstantData_old; \
			const param_type* pConstantData_new; \
		} buf; \
	public: \
		StateItem_Set##func(StateItemBase** head, UINT StartRegister, const param_type* pConstantData, UINT ParamCount) : StateItemBase(head), applied(false) \
		{ memset(&buf, 0, sizeof(buf)); buf.StartRegister=StartRegister; buf.ParamCount=ParamCount; buf.pConstantData_new=pConstantData; buf.pConstantData_old=NEW param_type[ParamCount]; } \
		void Release() { if (buf.pConstantData_old) delete[] buf.pConstantData_old; delete this; } \
		virtual HRESULT Apply(IDirect3DDevice9* device) \
		{ \
			device->Get##func(buf.StartRegister, buf.pConstantData_old, buf.ParamCount); \
			HRESULT hr = device->Set##func(buf.StartRegister, buf.pConstantData_new, buf.ParamCount); \
			applied = SUCCEEDED(hr); \
			return hr; \
		} \
		virtual HRESULT Restore(IDirect3DDevice9* device) \
		{ \
			return applied ? device->Set##func(buf.StartRegister, buf.pConstantData_old, buf.ParamCount) : S_FALSE; \
		} \
	};


//////////////////////////////////////////////////////////////////////////

class StateItemBase : public TList<StateItemBase>
{
public:
	StateItemBase(StateItemBase** head) { addToList(head); }
	//virtual ~StateItemBase() { removeFromList(); }

	virtual void Release() = 0;
	virtual HRESULT Apply(IDirect3DDevice9* device) = 0;
	virtual HRESULT Restore(IDirect3DDevice9* device) = 0;
};


StateItemImpl1			(LightEnable, GetLightEnable, DWORD, BOOL)
StateItemImpl0			(SetFVF, GetFVF, DWORD)
StateItemImpl0_Ptr		(SetIndices, GetIndices, IDirect3DIndexBuffer9)
StateItemImpl1_Struct	(SetLight, GetLight, DWORD, D3DLIGHT9)
StateItemImpl0_Struct	(SetMaterial, GetMaterial, D3DMATERIAL9)
StateItemImpl0_Ptr		(SetPixelShader, GetPixelShader, IDirect3DPixelShader9)
StateItemImpl1			(SetRenderState, GetRenderState, D3DRENDERSTATETYPE, DWORD)
StateItemImpl2			(SetSamplerState, GetSamplerState, DWORD, D3DSAMPLERSTATETYPE, DWORD)
StateItemImpl0_Struct	(SetScissorRect, GetScissorRect, RECT)
StateItemImpl1			(SetStreamSourceFreq, GetStreamSourceFreq, UINT, UINT)
StateItemImpl1_Ptr		(SetTexture, GetTexture, DWORD, IDirect3DBaseTexture9)
StateItemImpl2			(SetTextureStageState, GetTextureStageState, DWORD, D3DTEXTURESTAGESTATETYPE, DWORD)
StateItemImpl1_Struct	(SetTransform, GetTransform, D3DTRANSFORMSTATETYPE, D3DMATRIX)
StateItemImpl0_Struct	(SetViewport, GetViewport, D3DVIEWPORT9)
StateItemImpl0_Ptr		(SetVertexDeclaration, GetVertexDeclaration, IDirect3DVertexDeclaration9)
StateItemImpl0_Ptr		(SetVertexShader, GetVertexShader, IDirect3DVertexShader9)
StateItemImpl_Constant	(PixelShaderConstantB, BOOL)
StateItemImpl_Constant	(PixelShaderConstantF, float)
StateItemImpl_Constant	(PixelShaderConstantI, int)
StateItemImpl_Constant	(VertexShaderConstantB, BOOL)
StateItemImpl_Constant	(VertexShaderConstantF, float)
StateItemImpl_Constant	(VertexShaderConstantI, int)

// 特殊状态函数实现
class StateItem_SetClipPlane : public StateItemBase
{
	bool applied;
	struct
	{
		DWORD in1;
		float out_old[4];
		float* out_new;
	} buf;
public:
	StateItem_SetClipPlane(StateItemBase** head, DWORD Index,float* pPlane) : StateItemBase(head), applied(false)
	{ memset(&buf, 0, sizeof(buf)); buf.in1=Index; buf.out_new=pPlane; }
	void Release() { delete this; }
	virtual HRESULT Apply(IDirect3DDevice9* device)
	{
		device->GetClipPlane(buf.in1, buf.out_old);
		HRESULT hr = device->SetClipPlane(buf.in1, buf.out_new);
		applied = SUCCEEDED(hr);
		return hr;
	}
	virtual HRESULT Restore(IDirect3DDevice9* device)
	{
		return applied ? device->SetClipPlane(buf.in1, buf.out_old) : S_FALSE;
	}
};

class StateItem_SetStreamSource : public StateItemBase
{
	bool applied;
	struct
	{
		UINT StreamNumber;

		CComPtr<IDirect3DVertexBuffer9> pStreamData_old;
		UINT OffsetInBytes_old;
		UINT Stride_old;

		IDirect3DVertexBuffer9* pStreamData_new;
		UINT OffsetInBytes_new;
		UINT Stride_new;
	} buf;
public:
	StateItem_SetStreamSource(StateItemBase** head, UINT StreamNumber,IDirect3DVertexBuffer9* pStreamData,UINT OffsetInBytes,UINT Stride) : StateItemBase(head), applied(false)
	{
		buf.StreamNumber=StreamNumber;
		buf.OffsetInBytes_old = 0;
		buf.Stride_old = 0;
		buf.pStreamData_new = pStreamData;
		buf.OffsetInBytes_new = OffsetInBytes;
		buf.Stride_new = Stride;
	}
	void Release() { delete this; }
	virtual HRESULT Apply(IDirect3DDevice9* device)
	{
		device->GetStreamSource(buf.StreamNumber, &buf.pStreamData_old, &buf.OffsetInBytes_old, &buf.Stride_old);
		HRESULT hr = device->SetStreamSource(buf.StreamNumber, buf.pStreamData_new, buf.OffsetInBytes_new, buf.Stride_new);
		applied = SUCCEEDED(hr);
		return hr;
	}
	virtual HRESULT Restore(IDirect3DDevice9* device)
	{
		return applied ? device->SetStreamSource(buf.StreamNumber, buf.pStreamData_old, buf.OffsetInBytes_old, buf.Stride_old) : S_FALSE;
	}
};

class StateItem_SetNPatchMode : public StateItemBase
{
	bool applied;
	struct
	{
		float nSegments_old;
		float nSegments_new;
	} buf;
public:
	StateItem_SetNPatchMode(StateItemBase** head, float nSegments) : StateItemBase(head), applied(false)
	{
		buf.nSegments_old = 0;
		buf.nSegments_new = nSegments;
	}
	void Release() { delete this; }
	virtual HRESULT Apply(IDirect3DDevice9* device)
	{
		buf.nSegments_old = device->GetNPatchMode();
		HRESULT hr = device->SetNPatchMode(buf.nSegments_new);
		applied = SUCCEEDED(hr);
		return hr;
	}
	virtual HRESULT Restore(IDirect3DDevice9* device)
	{
		return applied ? device->SetNPatchMode(buf.nSegments_old) : S_FALSE;
	}
};


//////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////// 
////////////////////////////////////////////////////////////////////////// 

#define __no_brackets(...) __VA_ARGS__
#define ImplementStateFunc(func, params, forward_params) \
	STDMETHOD( func##params ) \
	{ \
		StateItem_##func* p = NEW StateItem_##func(&m_items, __no_brackets##forward_params); \
		if (p && m_autoApply) return p->Apply(m_device); \
		return S_OK; \
	} \
	/*HRESULT func##_NoLock##params { return m_device==NULL ? E_UNEXPECTED : m_device->func##forward_params; }*/

//////////////////////////////////////////////////////////////////////////
// AutoStateDevice 用于记录并设置设备状态，并自动恢复原始状态
// 使用 . 操作符可以自动记录/自动恢复设备状态，使用 -> 操作符相当于直接操纵3D设备接口，不记录任何状态改变
// 此类应该在栈上构造，避免使用全局变量或 NEW 出来

class AutoStateDevice //: public ID3DXEffectStateManager
{
	IDirect3DDevice9* m_device;
	StateItemBase* m_items;
	BOOL m_autoApply;

	void _apply(StateItemBase* si);
	void _restore(StateItemBase* si);

public:
	AutoStateDevice(IDirect3DDevice9* device, BOOL autoApply=TRUE);
	~AutoStateDevice();

	void SetDevice(IDirect3DDevice9* dev) { m_device = dev; }

	void Apply();
	void Restore();

	IDirect3DDevice9* operator ->() { return m_device; }

	//STDMETHOD(QueryInterface)(REFIID iid, LPVOID *ppvObj)
	//{
	//	if (!ppvObj)
	//		return E_POINTER;

	//	*ppvObj = NULL;
	//	if ((iid==__uuidof(IUnknown)) //||
	//		/*(iid==__uuidof(ID3DXEffectStateManager))*/)
	//	{
	//		*ppvObj = (ID3DXEffectStateManager*)this;
	//		return S_OK;
	//	}
	//	return E_NOINTERFACE;
	//}
	//STDMETHOD_(ULONG, AddRef)() { return 1; }
	//STDMETHOD_(ULONG, Release)() { return 1; }

	ImplementStateFunc(LightEnable, (DWORD Index,BOOL Enable), (Index,Enable))
	ImplementStateFunc(SetFVF, (DWORD FVF), (FVF))
	ImplementStateFunc(SetClipPlane, (DWORD Index,float* pPlane), (Index, pPlane))
	ImplementStateFunc(SetIndices, (IDirect3DIndexBuffer9 *pIndexData), (pIndexData))
	ImplementStateFunc(SetLight, (DWORD Index,CONST D3DLIGHT9* pLight), (Index, (D3DLIGHT9*)pLight))
	ImplementStateFunc(SetMaterial, (CONST D3DMATERIAL9* pMaterial), ((D3DMATERIAL9*)pMaterial))
	ImplementStateFunc(SetNPatchMode, (float nSegments), (nSegments))
	ImplementStateFunc(SetPixelShader, (IDirect3DPixelShader9* pShader), (pShader))
	ImplementStateFunc(SetRenderState, (D3DRENDERSTATETYPE State,DWORD Value), (State, Value))
	ImplementStateFunc(SetSamplerState, (DWORD Sampler,D3DSAMPLERSTATETYPE Type,DWORD Value), (Sampler, Type, Value))
	ImplementStateFunc(SetScissorRect, (RECT* pRect), (pRect))
	ImplementStateFunc(SetStreamSource, (UINT StreamNumber,IDirect3DVertexBuffer9* pStreamData,UINT OffsetInBytes,UINT Stride), (StreamNumber, pStreamData, OffsetInBytes, Stride))
	ImplementStateFunc(SetStreamSourceFreq, (UINT StreamNumber,UINT Setting), (StreamNumber, Setting))
	ImplementStateFunc(SetTexture, (DWORD Stage,IDirect3DBaseTexture9* pTexture), (Stage, pTexture))
	ImplementStateFunc(SetTextureStageState, (DWORD Stage,D3DTEXTURESTAGESTATETYPE Type,DWORD Value), (Stage, Type, Value))
	ImplementStateFunc(SetTransform, (D3DTRANSFORMSTATETYPE State,CONST D3DMATRIX* pMatrix), (State, (D3DMATRIX*)pMatrix))
	ImplementStateFunc(SetViewport, (D3DVIEWPORT9* pViewport), (pViewport))
	ImplementStateFunc(SetVertexDeclaration, (IDirect3DVertexDeclaration9* pDecl), (pDecl))
	ImplementStateFunc(SetVertexShader, (IDirect3DVertexShader9* pShader), (pShader))
	//STDMETHOD( SetVertexShader )(IDirect3DVertexShader9* pShader)
	//{
	//	StateItem_SetVertexShader* p = NEW StateItem_SetVertexShader(&m_items, pShader);
	//	if (p && m_autoApply)
	//		return p->Apply(m_device);
	//	return S_OK;
	//} 

	ImplementStateFunc(SetPixelShaderConstantB, (UINT StartRegister,const BOOL *pConstantData,UINT ConstantCount), (StartRegister,pConstantData,ConstantCount))
	ImplementStateFunc(SetPixelShaderConstantF, (UINT StartRegister,const float *pConstantData,UINT ConstantCount), (StartRegister,pConstantData,ConstantCount))
	ImplementStateFunc(SetPixelShaderConstantI, (UINT StartRegister,const int *pConstantData,UINT ConstantCount), (StartRegister,pConstantData,ConstantCount))
	ImplementStateFunc(SetVertexShaderConstantB, (UINT StartRegister,const BOOL *pConstantData,UINT ConstantCount), (StartRegister,pConstantData,ConstantCount))
	ImplementStateFunc(SetVertexShaderConstantF, (UINT StartRegister,const float *pConstantData,UINT ConstantCount), (StartRegister,pConstantData,ConstantCount))
	ImplementStateFunc(SetVertexShaderConstantI, (UINT StartRegister,const int *pConstantData,UINT ConstantCount), (StartRegister,pConstantData,ConstantCount))
};

#endif // __DUID3DHELPER_H__
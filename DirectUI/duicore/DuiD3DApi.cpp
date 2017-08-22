#include "stdafx.h"


#pragma warning(push)
#pragma warning(disable:4273)

// D3DX9_NO.dll 可能有多个版本，NO 的数字从24 - 43，每个版本都有相同的API实现（不同版本的数量可能不同），加载时从大到小尝试哪个DLL可以加载
#include <d3dx9.h>
#include "../../common/apiloader.h"

#define __foreach_d3dxapi(v) \
	/* Matrix API */ \
	v(D3DXMATRIX*, D3DXMatrixMultiply, (D3DXMATRIX *pOut, CONST D3DXMATRIX *pM1, CONST D3DXMATRIX *pM2), (pOut,pM1,pM2)) \
	v(D3DXMATRIX*, D3DXMatrixInverse, (D3DXMATRIX *pOut, FLOAT *pDeterminant, CONST D3DXMATRIX *pM), (pOut,pDeterminant,pM)) \
	v(D3DXMATRIX*, D3DXMatrixMultiplyTranspose, (D3DXMATRIX *pOut, CONST D3DXMATRIX *pM1, CONST D3DXMATRIX *pM2), (pOut,pM1,pM2)) \
	v(D3DXMATRIX*, D3DXMatrixReflect, (D3DXMATRIX *pOut, CONST D3DXPLANE *pPlane), (pOut,pPlane)) \
	v(D3DXMATRIX*, D3DXMatrixRotationX, (D3DXMATRIX *pOut, FLOAT Angle), (pOut,Angle)) \
	v(D3DXMATRIX*, D3DXMatrixRotationY, (D3DXMATRIX *pOut, FLOAT Angle), (pOut,Angle)) \
	v(D3DXMATRIX*, D3DXMatrixRotationZ, (D3DXMATRIX *pOut, FLOAT Angle), (pOut,Angle)) \
	v(D3DXMATRIX*, D3DXMatrixRotationAxis, (D3DXMATRIX *pOut, CONST D3DXVECTOR3 *pV, FLOAT Angle), (pOut,pV,Angle)) \
	v(D3DXMATRIX*, D3DXMatrixRotationQuaternion, (D3DXMATRIX *pOut, CONST D3DXQUATERNION *pQ), (pOut,pQ)) \
	v(D3DXMATRIX*, D3DXMatrixRotationYawPitchRoll, (D3DXMATRIX *pOut, FLOAT Yaw, FLOAT Pitch, FLOAT Roll), (pOut,Yaw,Pitch,Roll)) \
	v(D3DXMATRIX*, D3DXMatrixScaling, (D3DXMATRIX *pOut, FLOAT sx, FLOAT sy, FLOAT sz), (pOut,sx,sy,sz)) \
	v(D3DXMATRIX*, D3DXMatrixTranslation, (D3DXMATRIX *pOut, FLOAT x, FLOAT y, FLOAT z), (pOut,x,y,z)) \
	v(D3DXMATRIX*, D3DXMatrixTranspose, (D3DXMATRIX *pOut, CONST D3DXMATRIX *pM), (pOut,pM)) \
	v(D3DXMATRIX*, D3DXMatrixShadow, (D3DXMATRIX *pOut, CONST D3DXVECTOR4 *pLight, CONST D3DXPLANE *pPlane), (pOut,pLight,pPlane)) \
	v(D3DXMATRIX*, D3DXMatrixLookAtLH, (D3DXMATRIX *pOut, CONST D3DXVECTOR3 *pEye, CONST D3DXVECTOR3 *pAt, CONST D3DXVECTOR3 *pUp), (pOut,pEye,pAt,pUp)) \
	v(D3DXMATRIX*, D3DXMatrixLookAtRH, (D3DXMATRIX *pOut, CONST D3DXVECTOR3 *pEye, CONST D3DXVECTOR3 *pAt, CONST D3DXVECTOR3 *pUp), (pOut,pEye,pAt,pUp)) \
	v(FLOAT, D3DXMatrixDeterminant, (CONST D3DXMATRIX *pM), (pM)) \
	v(D3DXMATRIX*, D3DXMatrixPerspectiveFovLH, (D3DXMATRIX *pOut, FLOAT fovy, FLOAT Aspect, FLOAT zn, FLOAT zf), (pOut,fovy,Aspect,zn,zf)) \
	v(D3DXMATRIX*, D3DXMatrixOrthoLH, (D3DXMATRIX *pOut, FLOAT w, FLOAT h, FLOAT zn, FLOAT zf), (pOut,w,h,zn,zf)) \
	v(D3DXMATRIX*, D3DXMatrixOrthoOffCenterLH, (D3DXMATRIX *pOut, FLOAT l, FLOAT r, FLOAT b, FLOAT t, FLOAT zn, FLOAT zf), (pOut,l,r,b,t,zn,zf)) \
	/* Vector3 API */ \
	v(D3DXVECTOR3*, D3DXVec3TransformCoord, (D3DXVECTOR3 *pOut, CONST D3DXVECTOR3 *pV, CONST D3DXMATRIX *pM), (pOut,pV,pM)) \
	v(D3DXVECTOR3*, D3DXVec3TransformNormal, (D3DXVECTOR3 *pOut, CONST D3DXVECTOR3 *pV, CONST D3DXMATRIX *pM), (pOut,pV,pM)) \
	v(D3DXVECTOR3*, D3DXVec3Normalize, (D3DXVECTOR3 *pOut, CONST D3DXVECTOR3 *pV), (pOut,pV)) \
	/* Plane */ \
	v(D3DXPLANE*, D3DXPlaneFromPointNormal, (D3DXPLANE *pOut, CONST D3DXVECTOR3 *pPoint, CONST D3DXVECTOR3 *pNormal), (pOut,pPoint,pNormal)) \
	v(D3DXVECTOR3*, D3DXPlaneIntersectLine, (D3DXVECTOR3 *pOut, CONST D3DXPLANE *pP, CONST D3DXVECTOR3 *pV1, CONST D3DXVECTOR3 *pV2), (pOut,pP,pV1,pV2)) \
	/* Normal API */ \
	v(HRESULT, D3DXComputeNormals, (LPD3DXBASEMESH pMesh, CONST DWORD *pAdjacency), (pMesh,pAdjacency)) \
	v(HRESULT, D3DXComputeTangentFrameEx, (ID3DXMesh *pMesh, DWORD dwTextureInSemantic, DWORD dwTextureInIndex, DWORD dwUPartialOutSemantic, DWORD dwUPartialOutIndex, DWORD dwVPartialOutSemantic, DWORD dwVPartialOutIndex, DWORD dwNormalOutSemantic, DWORD dwNormalOutIndex, DWORD dwOptions, CONST DWORD *pdwAdjacency, FLOAT fPartialEdgeThreshold, FLOAT fSingularPointThreshold, FLOAT fNormalEdgeThreshold, ID3DXMesh **ppMeshOut, ID3DXBuffer **ppVertexMapping), \
				(pMesh,dwTextureInSemantic,dwTextureInIndex,dwUPartialOutSemantic,dwUPartialOutIndex,dwVPartialOutSemantic,dwVPartialOutIndex,dwNormalOutSemantic,dwNormalOutIndex,dwOptions,pdwAdjacency,fPartialEdgeThreshold,fSingularPointThreshold,fNormalEdgeThreshold,ppMeshOut,ppVertexMapping)) \
	v(UINT, D3DXGetDeclLength, (CONST D3DVERTEXELEMENT9 *pDecl), (pDecl)) \
	v(HRESULT, D3DXFrameDestroy, (LPD3DXFRAME pFrameRoot, LPD3DXALLOCATEHIERARCHY pAlloc), (pFrameRoot,pAlloc)) \
	v(HRESULT, D3DXLoadMeshFromXInMemory, (LPCVOID Memory, DWORD SizeOfMemory, DWORD Options, LPDIRECT3DDEVICE9 pD3DDevice, LPD3DXBUFFER *ppAdjacency, LPD3DXBUFFER *ppMaterials, LPD3DXBUFFER *ppEffectInstances, DWORD *pNumMaterials, LPD3DXMESH *ppMesh), \
				(Memory,SizeOfMemory,Options,pD3DDevice,ppAdjacency,ppMaterials,ppEffectInstances,pNumMaterials,ppMesh)) \
	v(HRESULT, D3DXLoadMeshHierarchyFromXInMemory, (LPCVOID Memory, DWORD SizeOfMemory, DWORD MeshOptions, LPDIRECT3DDEVICE9 pD3DDevice, LPD3DXALLOCATEHIERARCHY pAlloc, LPD3DXLOADUSERDATA pUserDataLoader, LPD3DXFRAME *ppFrameHierarchy, LPD3DXANIMATIONCONTROLLER *ppAnimController), \
				(Memory,SizeOfMemory,MeshOptions,pD3DDevice,pAlloc,pUserDataLoader,ppFrameHierarchy,ppAnimController)) \
	v(LPD3DXFRAME, D3DXFrameFind, (CONST D3DXFRAME *pFrameRoot, LPCSTR Name), (pFrameRoot,Name)) \
	v(HRESULT, D3DXCreateVolumeTextureFromFileInMemory, (LPDIRECT3DDEVICE9 pDevice, LPCVOID pSrcData, UINT SrcDataSize, LPDIRECT3DVOLUMETEXTURE9* ppVolumeTexture), (pDevice,pSrcData,SrcDataSize,ppVolumeTexture)) \
	v(HRESULT, D3DXCreateCubeTextureFromFileInMemory, (LPDIRECT3DDEVICE9 pDevice, LPCVOID pSrcData, UINT SrcDataSize, LPDIRECT3DCUBETEXTURE9* ppCubeTexture), (pDevice,pSrcData,SrcDataSize,ppCubeTexture)) \
	v(HRESULT, D3DXCreateTextureFromFileInMemory, (LPDIRECT3DDEVICE9 pDevice, LPCVOID pSrcData, UINT SrcDataSize, LPDIRECT3DTEXTURE9* ppTexture), (pDevice,pSrcData,SrcDataSize,ppTexture)) \
	v(HRESULT, D3DXGetImageInfoFromFileInMemory, (LPCVOID pSrcData, UINT SrcDataSize, D3DXIMAGE_INFO* pSrcInfo), (pSrcData,SrcDataSize,pSrcInfo)) \
	v(HRESULT, D3DXDeclaratorFromFVF, (DWORD FVF, D3DVERTEXELEMENT9 pDeclarator[MAX_FVF_DECL_SIZE]), (FVF,pDeclarator)) \
	v(HRESULT, D3DXComputeBoundingBox, (CONST D3DXVECTOR3 *pFirstPosition,DWORD NumVertices,DWORD dwStride,D3DXVECTOR3 *pMin,D3DXVECTOR3 *pMax), (pFirstPosition,NumVertices,dwStride,pMin,pMax)) \
	v(HRESULT, D3DXComputeBoundingSphere, (CONST D3DXVECTOR3 *pFirstPosition, DWORD NumVertices, DWORD dwStride, D3DXVECTOR3 *pCenter, FLOAT *pRadius), (pFirstPosition,NumVertices,dwStride,pCenter,pRadius)) \
	v(BOOL, D3DXBoxBoundProbe, (CONST D3DXVECTOR3 *pMin, CONST D3DXVECTOR3 *pMax, CONST D3DXVECTOR3 *pRayPosition, CONST D3DXVECTOR3 *pRayDirection), (pMin,pMax,pRayPosition,pRayDirection)) \
	v(BOOL, D3DXSphereBoundProbe, (CONST D3DXVECTOR3 *pCenter, FLOAT Radius, CONST D3DXVECTOR3 *pRayPosition, CONST D3DXVECTOR3 *pRayDirection), (pCenter,Radius,pRayPosition,pRayDirection)) \
	v(HRESULT, D3DXCreateMeshFVF, (DWORD NumFaces, DWORD NumVertices, DWORD Options, DWORD FVF, LPDIRECT3DDEVICE9 pD3DDevice, LPD3DXMESH* ppMesh), (NumFaces,NumVertices,Options,FVF,pD3DDevice,ppMesh)) \
	v(HRESULT, D3DXCreateBox, (LPDIRECT3DDEVICE9 pDevice, FLOAT Width, FLOAT Height, FLOAT Depth, LPD3DXMESH* ppMesh, LPD3DXBUFFER* ppAdjacency), (pDevice,Width,Height,Depth,ppMesh,ppAdjacency)) \
	v(HRESULT, D3DXCreateCylinder, (LPDIRECT3DDEVICE9 pDevice, FLOAT Radius1, FLOAT Radius2, FLOAT Length, UINT Slices, UINT Stacks, LPD3DXMESH* ppMesh, LPD3DXBUFFER* ppAdjacency), \
				(pDevice,Radius1,Radius2,Length,Slices,Stacks,ppMesh,ppAdjacency)) \
	v(HRESULT, D3DXCreatePolygon, (LPDIRECT3DDEVICE9 pDevice, FLOAT Length, UINT Sides, LPD3DXMESH* ppMesh, LPD3DXBUFFER* ppAdjacency), (pDevice,Length,Sides,ppMesh,ppAdjacency)) \
	v(HRESULT, D3DXCreateSphere, (LPDIRECT3DDEVICE9 pDevice, FLOAT Radius, UINT Slices, UINT Stacks, LPD3DXMESH* ppMesh, LPD3DXBUFFER* ppAdjacency), (pDevice,Radius,Slices,Stacks,ppMesh,ppAdjacency)) \
	v(HRESULT, D3DXCreateTorus, (LPDIRECT3DDEVICE9 pDevice, FLOAT InnerRadius, FLOAT OuterRadius, UINT Sides, UINT Rings, LPD3DXMESH* ppMesh, LPD3DXBUFFER* ppAdjacency), (pDevice,InnerRadius,OuterRadius,Sides,Rings,ppMesh,ppAdjacency)) \
	v(HRESULT, D3DXCreateTeapot, (LPDIRECT3DDEVICE9 pDevice, LPD3DXMESH* ppMesh, LPD3DXBUFFER* ppAdjacency), (pDevice,ppMesh,ppAdjacency)) \
	v(HRESULT, D3DXCreateTextW, (LPDIRECT3DDEVICE9 pDevice, HDC hDC, LPCWSTR pText, FLOAT Deviation, FLOAT Extrusion, LPD3DXMESH* ppMesh, LPD3DXBUFFER* ppAdjacency, LPGLYPHMETRICSFLOAT pGlyphMetrics), \
				(pDevice,hDC,pText,Deviation,Extrusion,ppMesh,ppAdjacency,pGlyphMetrics)) \
	v(HRESULT, D3DXIntersect, (LPD3DXBASEMESH pMesh, CONST D3DXVECTOR3 *pRayPos, CONST D3DXVECTOR3 *pRayDir, BOOL *pHit, DWORD *pFaceIndex, FLOAT *pU, FLOAT *pV, FLOAT *pDist, LPD3DXBUFFER *ppAllHits, DWORD *pCountOfHits), \
				(pMesh,pRayPos,pRayDir,pHit,pFaceIndex,pU,pV,pDist,ppAllHits,pCountOfHits)) \
	v(HRESULT, D3DXCreateRenderToSurface, (LPDIRECT3DDEVICE9 pDevice, UINT Width, UINT Height, D3DFORMAT Format, BOOL DepthStencil, D3DFORMAT DepthStencilFormat, LPD3DXRENDERTOSURFACE*  ppRenderToSurface), \
				(pDevice,Width,Height,Format,DepthStencil,DepthStencilFormat,ppRenderToSurface)) \
	v(HRESULT, D3DXCreateEffectPool, (LPD3DXEFFECTPOOL* ppPool), (ppPool)) \
	v(HRESULT, D3DXCreateEffect, (LPDIRECT3DDEVICE9 pDevice, LPCVOID pSrcData, UINT SrcDataLen, CONST D3DXMACRO* pDefines, LPD3DXINCLUDE pInclude, DWORD Flags, LPD3DXEFFECTPOOL pPool, LPD3DXEFFECT* ppEffect, LPD3DXBUFFER* ppCompilationErrors), \
				(pDevice,pSrcData,SrcDataLen,pDefines,pInclude,Flags,pPool,ppEffect,ppCompilationErrors)) \
	v(HRESULT, D3DXCreateEffectFromFileW, (LPDIRECT3DDEVICE9 pDevice, LPCWSTR pSrcFile, CONST D3DXMACRO* pDefines, LPD3DXINCLUDE pInclude, DWORD Flags, LPD3DXEFFECTPOOL pPool, LPD3DXEFFECT* ppEffect, LPD3DXBUFFER* ppCompilationErrors), \
				(pDevice,pSrcFile,pDefines,pInclude,Flags,pPool,ppEffect,ppCompilationErrors)) \
	v(HRESULT, D3DXLoadSurfaceFromMemory, (LPDIRECT3DSURFACE9 pDestSurface, CONST PALETTEENTRY* pDestPalette, CONST RECT* pDestRect, LPCVOID pSrcMemory, D3DFORMAT SrcFormat, UINT SrcPitch, CONST PALETTEENTRY* pSrcPalette, CONST RECT* pSrcRect, DWORD Filter, D3DCOLOR ColorKey), \
				(pDestSurface, pDestPalette, pDestRect, pSrcMemory, SrcFormat, SrcPitch, pSrcPalette, pSrcRect, Filter, ColorKey)) \


#define do_d3dxapi(ret, name, params_decl, params_var) \
	typedef ret (WINAPI * pfn##name) params_decl; \
	pfn##name fn##name = NULL; \
	extern "C" ret WINAPI name params_decl \
	{ \
		ATLASSERT(fn##name); \
		return fn##name params_var; \
	} \

__foreach_d3dxapi(do_d3dxapi)
#undef do_d3dxapi

//#define do_d3dxapi(ret,name,params_decl,params_var) \
//	pfn##name name = NULL;
//
////__foreach_d3dxapi(do_d3dxapi)
//#undef do_d3dxapi

BOOL g_d3dxapi_valid = FALSE;

class d3dxapi_loader
{
	HMODULE hmod;
public:
	d3dxapi_loader() : hmod(NULL)
	{
		for (long i=53/*43*/; i>=24; i--)
			if (_load(i)) break;
		if (!hmod) _load();
		if (!hmod) ::MessageBoxW(NULL, L"DirectX9 hasn't been installed!", L"Warning", MB_OK | MB_ICONERROR);
		else if (!g_d3dxapi_valid) ::MessageBoxW(NULL, L"Version of DirectX9 is too old!", L"Warning", MB_OK | MB_ICONERROR);
	}
	~d3dxapi_loader()
	{
		if (hmod)
		{
			::FreeLibrary(hmod);
			hmod = NULL;
		}
	}

private:
	bool _load(long number=0)
	{
		CStringW dllname(L"dxapi.dll");
		if (number > 0)
		{
#ifdef _DEBUG
			dllname.Format(L"d3dx9d_%d.dll", number);
			hmod = ::LoadLibraryW(dllname);
			if (hmod==NULL)
#endif // _DEBUG
			{
				dllname.Format(L"d3dx9_%d.dll", number);
			}
		}
		if (hmod==NULL)
			hmod = ::LoadLibraryW(dllname);
		if (hmod)
		{
			g_d3dxapi_valid = TRUE;

#define do_d3dxapi(ret,name, params_decl, params_var) \
	fn##name = (pfn##name)::GetProcAddress(hmod, #name); \
	if (fn##name == NULL) \
	{ \
		g_d3dxapi_valid = FALSE; \
		::MessageBoxW(NULL, CStringW(L"API '" L#name L"' isn't found in ") + dllname, L"Warning", MB_OK); \
		::FreeLibrary(hmod); \
		hmod=NULL; \
		return FALSE; \
	}

			__foreach_d3dxapi(do_d3dxapi)
#undef do_d3dxapi
		}
		//else
		//	::MessageBoxW(NULL, dllname + L" hasn't be found!", L"Warning", MB_OK | MB_ICONWARNING);
		
		return (hmod!=NULL);
	}
};

d3dxapi_loader _d3dxapi_loader;


//////////////////////////////////////////////////////////////////////////
// d3d9.dll
//#pragma comment(linker, "/nodefaultlib[d3d9.lib]")

#ifdef _DEBUG
#define __foreach_d3d9api(v) \
	v(IDirect3D9*, Direct3DCreate9, (UINT SDKVersion), (SDKVersion)) \
	__if_exists(D3DPERF_BeginEvent) { \
		v(int, D3DPERF_BeginEvent, (D3DCOLOR col, LPCWSTR wszName), (col,wszName)) \
		v(int, D3DPERF_EndEvent, (), ()) \
	}
#else
#define __foreach_d3d9api(v) \
	v(IDirect3D9*, Direct3DCreate9, (UINT SDKVersion), (SDKVersion))
#endif // _DEBUG

ApiLoader d3d9api(L"d3d9.dll");

#define do_api(ret, name, params_decl, params_var) \
	typedef ret (WINAPI * pfn##name) params_decl; \
	pfn##name fn##name = (pfn##name)d3d9api(#name); \
	extern "C" ret WINAPI name params_decl \
	{ \
		ATLASSERT(fn##name); \
		return fn##name params_var; \
	} \

__foreach_d3d9api(do_api)
#undef do_api

//////////////////////////////////////////////////////////////////////////
// uxtheme.dll
//#include <uxtheme.h>
//#pragma comment(linker, "/nodefaultlib[uxtheme.lib]")

//#define __foreach_uxthemeapi(v) \
//	v(void, SetThemeAppProperties, (DWORD dwFlags), (dwFlags))
//
//ApiLoader uxthemeapi(L"uxtheme.dll");
//
//#define do_api(ret, name, params_decl, params_var) \
//	typedef ret (WINAPI * pfn##name) params_decl; \
//	pfn##name fn##name = (pfn##name)uxthemeapi(#name); \
//	extern "C" ret WINAPI name params_decl \
//	{ \
//		ATLASSERT(fn##name); \
//		return fn##name params_var; \
//	} \
//
//__foreach_uxthemeapi(do_api)
//#undef do_api


//////////////////////////////////////////////////////////////////////////
// riched20.dll or riched32.dll
#include <Richedit.h>
#include <TextServ.h>

#define __foreach_richedapi(v) \
	v(HRESULT, CreateTextServices, (IUnknown *punkOuter, ITextHost *pITextHost, IUnknown **ppUnk), (punkOuter,pITextHost,ppUnk)) \


ApiLoader richedapi(L"riched20.dll", L"riched32.dll");

#define do_api(ret, name, params_decl, params_var) \
	typedef ret (WINAPI * pfn##name) params_decl; \
	pfn##name fn##name = (pfn##name)richedapi(#name); \
	extern "C" ret WINAPI name params_decl \
	{ \
		ATLASSERT(fn##name); \
		return fn##name params_var; \
	} \

__foreach_richedapi(do_api)
#undef do_api


//////////////////////////////////////////////////////////////////////////
// winmm.dll
//#include <mmsystem.h>
//#pragma comment(linker, "/nodefaultlib[winmm.lib]")

#if 1
	#pragma comment(lib, "winmm.lib")
#else
#define __foreach_winmmapi(v) \
	v(DWORD, timeGetTime, (), ()) \

ApiLoader winmmapi(L"winmm.dll");

#define do_api(ret, name, params_decl, params_var) \
	typedef ret (WINAPI * pfn##name) params_decl; \
	pfn##name fn##name = (pfn##name)winmmapi(#name); \
	extern "C" ret WINAPI name params_decl \
	{ \
		ATLASSERT(fn##name); \
		return fn##name params_var; \
	} \

__foreach_winmmapi(do_api)
#undef do_api
#endif // _DEBUG


//////////////////////////////////////////////////////////////////////////
// shell32.dll
//#include <mmsystem.h>
//#pragma comment(linker, "/nodefaultlib[winmm.lib]")

//#define __foreach_shellapi(v) \
//	v(HINSTANCE, ShellExecuteW, (HWND hwnd, LPCWSTR lpOperation, LPCWSTR lpFile, LPCWSTR lpParameters, LPCWSTR lpDirectory, INT nShowCmd), (hwnd,lpOperation,lpFile,lpParameters,lpDirectory,nShowCmd)) \
//
//ApiLoader shellapi(L"winmm.dll");
//
//#define do_api(ret, name, params_decl, params_var) \
//	typedef ret (WINAPI * pfn##name) params_decl; \
//	pfn##name fn##name = (pfn##name)shellapi(#name); \
//	extern "C" ret WINAPI name params_decl \
//	{ \
//		ATLASSERT(fn##name); \
//		return fn##name params_var; \
//	} \
//
//__foreach_shellapi(do_api)
//#undef do_api


#pragma warning(pop)


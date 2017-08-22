#ifndef __DUI3DOBJECT_H__
#define __DUI3DOBJECT_H__

#include "DuiD3DImpl.h"

class iCamera;
class iSceneTarget;
class iScene;
class iAmbientLight;
//class iLight;
	class iPointLight;
	class iDirectionLight;
	class iSpotLight;
class iLightGroup;
class iModelBase;
	class iModel;
	class iQuadModel;
	class iBoxModel;
	class iCylinderModel;
	class iPolygonModel;
	class iSphereModel;
	class iTorusModel;
	class iTextModel;
	class iTeapotModel;
	class iXModel;
	class iNullModel;
class iEntity;


#define __foreach_model(v) \
	v(Model) \
	v(QuadModel) \
	v(BoxModel) \
	v(CylinderModel) \
	v(PolygonModel) \
	v(SphereModel) \
	v(TorusModel) \
	v(TextModel) \
	v(TeapotModel) \
	v(XModel) \
	v(NullModel)

#define cookie_child 1
#define cookie_sibling 2

// scene m_target
class iSceneTarget : public iRenderTarget
{
public:
	iScene* m_scene;

public:
	iSceneTarget(iScene* scene);
	~iSceneTarget();

	virtual void render_content();
};

// camera
class iCamera : public Camera
	//, TList<iCamera>
{
public:
	iScene* m_scene;

	iCamera(iScene* scene);
	~iCamera();

	void reset_params();


	D3DXVECTOR3 m_right; // 摄像机右方
	D3DXVECTOR3 m_up; // 摄像机上方
	D3DXVECTOR3 m_ahead; // 摄像机前方

	//D3DXMATRIX m_world; // 指示相机方向，第一行表示右，第二行表示上，第三行表示前，第四行表示眼睛位置（暂时不用）
	//
	//const D3DXVECTOR3* getWorldRight()	{ return (D3DXVECTOR3*)&m_world._11; }
	//const D3DXVECTOR3* getWorldUp()		{ return (D3DXVECTOR3*)&m_world._21; }
	//const D3DXVECTOR3* getWorldAhead()	{ return (D3DXVECTOR3*)&m_world._31; }
	//const D3DXVECTOR3* getWorldEyePt()	{ return (D3DXVECTOR3*)&m_world._41; }

	D3DXMATRIX get_view_matrix();
	D3DXMATRIX get_projection_matrix();
};

// ambient light
class iAmbientLight : public AmbientLight
{
public:
	iAmbientLight(bool enable);
};

// point light
class iPointLight : public PointLight
{
public:
	iPointLight();
	//~iPointLight();
};

// direction light
class iDirectionLight : public DirectionLight
{
public:
	iDirectionLight();
	//~iDirectionLight();
};

// spot light
class iSpotLight : public SpotLight
{
public:
	iSpotLight();
	//~iSpotLight();
};

// lights group
class iLightGroup //: public LightGroup
{
public:
	iAmbientLight m_ambientLight; // 用户环境光
	Light m_lights[8]; // 光源集合，最多8个光源

	iLightGroup();
	~iLightGroup();

	//bool setLight(ULONG index, Light* light);
	static HRESULT CALLBACK OnUpdated(long index, LPCOLESTR name, CComVariant& value, LPVOID data);
};

// 效果参数，用于保存效果文件中的参数值，这些值由XML初始化或者由脚本设定
class iEffectParams
{
public:
	CComBSTR params_string; // 保存 "g_fTime:2.5; g_vPos:0 0 -1.5;"

	CSimpleMap<D3DXHANDLE, EffectParamValue*> params;

	iEffectParams() : _effect(NULL), _cached(false) {}
	~iEffectParams();
	void clear();

	void set_params_string(LPCOLESTR szParams);
	EffectParamValue* get_param_value(iEffect* e, D3DXHANDLE hParam, bool bAutoCreate=false);

private:
	iEffect* _effect;
	bool _cached;
	void _cache(iEffect* e);
};

// 材质信息
class iMaterial : public Material
{
public:
	CComPtr<IDirect3DDevice9> device;
	long index; // 材质对应的纹理索引，取子2D集中的项，[0-x]，如果是 AutoValue 表示动态获取子2D项，-1表示无效
	CComBSTR texname; // 材质对应的纹理名称，可以是图片文件名，也可以是图片资源ID，也可以是控件ID。具体的纹理需要运算获得。
	CComPtr<IDirect3DBaseTexture9> texture; // 如果纹理是一个文件或资源，读取后缓存到这里。纹理可能是普通纹理，Volumn纹理或者Cube纹理。如果是控件纹理，应是NULL

	CComBSTR technique; // 如果应用了效果，则在这里指定技术名称。如果未指定技术，则采用系统默认效果
	iEffect* effect; // 缓存的效果，根据技术查找所属效果
	iEffectParams effect_params; // 材质中缓存的效果参数

	iMaterial();
	~iMaterial();

	bool load(IDirect3DDevice9* dev);
	virtual void on_reset();
	void resetName(LPCOLESTR szName);
	void resetName(long lName); // 设置动态纹理索引。动态纹理通常是当前场景所属表面的子表面。
private:
	CDownload download;
	static void CALLBACK onDownloadComplete(LPVOID pData, LPSTREAM pStream);
};

typedef BOOL (* FINDMODELPROC)(iModelBase* pModel, LPVOID data); 

// model base
// 模型是一种资源，保存在设备中，当在场景中实际使用时，需要创建一个实体对象。
class iModelBase : public TList<iModelBase>
				, public TList<iModelBase, cookie_child>
				, public TList<iModelBase, cookie_sibling>
{
	friend iEntity;
public:
	iDevice* m_device;
	CMarkupNode m_node;
	iModelBase* m_parent;
	CComBSTR m_name;

	iModelBase* m_children;
	iModelBase* m_siblings;

	CComPtr<ID3DXMesh> m_mesh; // 如果没有动画，则应该提供一个有效的MESH
	iMaterial* m_materials; // 材质数组
	DWORD m_matNum; // 材质数量
	bool m_useMaterials; // 是否使用材质，默认 true

	Transform3D m_local_transform; // 相对父模型的本地变换
	//CSimpleArray<iMaterial*> m_mapped_materials; // 映射的材质，用于替换模型中的原始材质，通过名称映射

	// 缓存的MESH数据
	struct _MeshInfo
	{
		DWORD numVertices;
		DWORD numFaces;
		DWORD bytesPerVertex;
		CComPtr<IDirect3DIndexBuffer9> ib;
		CComPtr<IDirect3DVertexBuffer9> vb;
		CComPtr<IDirect3DVertexDeclaration9> decl;
		D3DXVECTOR3 boundMin, boundMax, boundCenter;
		D3DXVECTOR3 sphereCenter;
		float sphereRadius;

		_MeshInfo() : numFaces(0), numVertices(0), bytesPerVertex(0), boundMin(0,0,0), boundMax(0,0,0), boundCenter(0,0,0), sphereCenter(0,0,0), sphereRadius(0) {}
		void clear() { /*numFaces=numVertices=bytesPerVertex=0;*/ ib=NULL;vb=NULL;decl=NULL; }
	} m_meshInfo;

	iEntity* m_entities; // 使用该模型的实体列表
	iEffectParams effect_params; // 模型中缓存的效果参数

	CComVariant onTransform;
	CComVariant onRender;
	CComVariant onPrepareEffect;
	CComVariant onMouseEnter;
	CComVariant onMouseLeave;
	CComVariant onClick;
	CComVariant onDblClick;

	iModelBase(iDevice* device=NULL);
	~iModelBase();

	virtual void on_reset();
	void unload();

	void setParent(iDevice* device);
	void setParent(iModelBase* parent);

	virtual void createMesh() = 0; // 初始化或者改变MESH属性后，需要重新创建MESH
	virtual iEntity* createEntity();
	virtual void pre_render();
	virtual void render();
	virtual IDispatch* getDispatch() = 0;
	virtual void getMaterials(CDispatchArray* pArr);

	// helper
	void setFVF(DWORD fvf); // 改变顶点的 FVF
	void setVertexDecl(const D3DVERTEXELEMENT9* pDecl, bool bSplitVertexForOptimalTangents = false); // 改变顶点申明

	void resetMesh(); // 如果创建MESH的参数发生了变化，调用此函数重新创建MESH，但不影响材质信息

	void enumAllModels(FINDMODELPROC proc, LPVOID data);

	static iModelBase* fromObject(IUnknown* unk); // 从组件获取 iModelBase 指针
	static void createModelByNode(HANDLE node, iDevice* device=NULL, iModelBase* parent=NULL, IDispatch** ppDisp=NULL);
	// 递归设置变换矩阵
	void updateMatrices(const D3DXMATRIX* mtParent);

protected:
	static BOOL _enumModel(iModelBase* pStart, FINDMODELPROC proc, LPVOID data);
	void _updateMeshInfo(); // 缓存常用MESH数据
	D3DXMATRIX m_world; // 缓存的世界矩阵，在渲染之前，必须由 updateMatrices 来更新
};

#define impletement_model_virtual() \
	virtual IDispatch* getDispatch() { return GetDispatch(); }

// Null model
class iNullModel : public NullModel
	, public iModelBase
{
public:
	iNullModel(iDevice* device=NULL) : iModelBase(device) {}

	virtual void createMesh() {}
	impletement_model_virtual()
};

// custom model
class iModel : public Model
				, public iModelBase
{
public:
	static const DWORD m_defFVF = D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1;
	struct ModelVertex
	{
		D3DXVECTOR3 position;
		D3DXVECTOR3 normal;
		float u,v;

		ModelVertex() : normal(0,0,1) {}
	};

	iModel(iDevice* device=NULL);
	~iModel();

	virtual void createMesh();
	impletement_model_virtual()
};

class iQuadModel : public QuadModel
				, public iModelBase
{
public:
	iQuadModel(iDevice* device=NULL);
	~iQuadModel();

	virtual void createMesh();
	impletement_model_virtual()
};

class iBoxModel : public BoxModel
				, public iModelBase
{
public:
	iBoxModel(iDevice* device=NULL);
	~iBoxModel();

	virtual void createMesh();
	impletement_model_virtual()
};

class iCylinderModel : public CylinderModel
				, public iModelBase
{
public:
	iCylinderModel(iDevice* device=NULL);
	~iCylinderModel();

	virtual void createMesh();
	impletement_model_virtual()
};

class iPolygonModel : public PolygonModel
				, public iModelBase
{
public:
	iPolygonModel(iDevice* device=NULL);
	~iPolygonModel();

	virtual void createMesh();
	impletement_model_virtual()
};

class iSphereModel : public SphereModel
				, public iModelBase
{
public:
	iSphereModel(iDevice* device=NULL);
	~iSphereModel();

	virtual void createMesh();
	impletement_model_virtual()
};

class iTorusModel : public TorusModel
				, public iModelBase
{
public:
	iTorusModel(iDevice* device=NULL);
	~iTorusModel();

	virtual void createMesh();
	impletement_model_virtual()
};

class iTextModel : public TextModel
				, public iModelBase
{
public:
	iTextModel(iDevice* device=NULL);
	~iTextModel();

	virtual void createMesh();
	impletement_model_virtual()
};

class iTeapotModel : public TeapotModel
				, public iModelBase
{
public:
	iTeapotModel(iDevice* device=NULL);
	~iTeapotModel();

	virtual void createMesh();
	impletement_model_virtual()
};

// 如果XMODEL拥有动画控制器，则模型为每个实体创建一个 xAnimInstance
class iAnimInstance
{
public:
	iXModel* model;
	CComPtr<ID3DXAnimationController> spAC;
	CComPtr<ID3DXAnimationSet> spAS; // 当前动画
	DWORD curTrack;	// 当前TRACE。一个设备通常有2个TRACK
	float curTime;	// 当前时间
	float speedAdjust;	// 速度调整因子，默认1.0，越大速度越慢
	float moveTransitionTime;	// 从一个动画切换到另一个动画的时间，越大越慢。默认0.25

	iAnimInstance(iXModel* m, ID3DXAnimationController* ac);
};

// x mesh model
class iXModel : public iModelBase
		, public XModel
		, public iObjectState
		//, public TList<iXModel> // 兄弟网格
{
	CDownload download;

public:
	struct MeshContainerEx;
	struct FrameEx;

	// mesh 帧容器信息
	struct MeshContainerEx : public D3DXMESHCONTAINER
	{
		CComBSTR name; // 不用基类的Name，转换成UNICODE
		FrameEx* frame; // 所属帧

		//IDirect3DTexture9**  exTextures;		// Array of texture pointers  
		iMaterial* exMaterials; // 材质数组，不用基类的pMaterials

		// Skinned mesh variables
		CComPtr<ID3DXMesh>   exSkinMesh;			// 蒙皮网格
		D3DXMATRIX*			 exBoneOffsets;			// 骨骼偏移矩阵数组，每个骨骼一个
		D3DXMATRIX**		 exFrameCombinedMatrixPointer;	// 帧矩阵指针数组，每个骨骼一个
		
		MeshContainerEx()
		{
			frame=NULL; exMaterials=NULL; exBoneOffsets=NULL; exFrameCombinedMatrixPointer=NULL;
			ZeroMemory((D3DXMESHCONTAINER*)this, sizeof(D3DXMESHCONTAINER));
		}
	};

	// mesh 帧信息
	struct FrameEx : public D3DXFRAME
	{
		CComBSTR name; // 不用基类的Name，转换成UNICODE
		D3DXMATRIX exCombinedTransformationMatrix;
	};

	// mesh 帧系列分配器
	class MeshAllocator : public ID3DXAllocateHierarchy
	{
	public:
		STDMETHOD(CreateFrame)(LPCSTR Name, LPD3DXFRAME *ppNewFrame );
		STDMETHOD(CreateMeshContainer)(LPCSTR Name, 
			CONST D3DXMESHDATA *pMeshData, 
			CONST D3DXMATERIAL *pMaterials, 
			CONST D3DXEFFECTINSTANCE *pEffectInstances, 
			DWORD NumMaterials, 
			CONST DWORD *pAdjacency, 
			LPD3DXSKININFO pSkinInfo, 
			LPD3DXMESHCONTAINER *ppNewMeshContainer);
		STDMETHOD(DestroyFrame)(LPD3DXFRAME pFrameToFree);
		STDMETHOD(DestroyMeshContainer)(LPD3DXMESHCONTAINER pMeshContainerToFree);
	};

	// 非动画对象，普通MESH，缓存到这里
	CComPtr<ID3DXMesh> m_meshCache;
	// Animation Objects
	FrameEx* m_frameRoot;	// 根帧
	MeshContainerEx* m_meshRoot;	// 根容器，在递归设置骨骼矩阵时缓存
	CComPtr<ID3DXAnimationController> m_animController;
	
	D3DXMATRIX* m_boneMatrices;	// 骨骼矩阵数组
	DWORD m_maxBones;	// 所有子MESH中的最大骨骼数

public:
	iXModel(iDevice* device=NULL, LPCOLESTR xurl=NULL, LPCOLESTR name=NULL);
	~iXModel();

	bool load(LPCOLESTR xurl, LPCOLESTR name=NULL);
	void unload();

	virtual void on_reset();
	virtual iEntity* createEntity();
	virtual void createMesh();
	virtual void getMaterials(CDispatchArray* pArr);
	virtual void pre_render();
	impletement_model_virtual()

private:

	static void CALLBACK onDownloadComplete(LPVOID pData, LPSTREAM pStream);

	CComPtr<IStream> _stream; // cache
	void _create();

	// 递归设置骨骼矩阵
	void updateBonesPtr(FrameEx* frame);
	void updateGetMaterials(FrameEx* frame, CDispatchArray* pArr);
	void clearCaches(FrameEx* frame);
};

//////////////////////////////////////////////////////////////////////////
// animation set
class iAnimationSet : public AnimationSet
{
public:
	CComPtr<ID3DXAnimationSet> m_as;
};


typedef BOOL (* FINDENTITYPROC)(iEntity* entity, LPVOID data);
//////////////////////////////////////////////////////////////////////////
// entity
// 一个实体对象代表一个模型的实例，因此一种模型可以创建多个实体对象，不同对象使用不同的世界矩阵
// 实体列表保存在模型中
class iEntity : public Entity
			, public TList<iEntity> // 保存在模型中
			, public TList<iEntity, cookie_child> // 子实体
			, public TList<iEntity, cookie_sibling> // 兄弟实体
{
public:
	iScene* m_scene;
	// 缓存的XML节点，如果场景中隐式指定实体，则此项无效
	CMarkupNode m_node;
	iEntity* m_parent;
	iModelBase* m_model;	// 实体使用的模型
	iAnimInstance* m_anim; // 如果模型具有动画控制器，会设置到这里
	CDispatchArray* m_animations; // 如果模型具有动画控制器，此成员有效，用来获取动画集集合
	D3DXMATRIX m_mtWorld;	// 实体的初始世界矩阵，初始值从XML静态读取。矩阵的动态转换因子从 Entity 继承，通常用脚本设置转换因子
	iMaterial m_Material; // 当前应用的材质
	iEffectParams effect_params; // 实体中缓存的效果参数

	iEntity* m_children; // 子实体
	iEntity* m_siblings; // 兄弟实体

	iEntity(iModelBase* model, iScene* scene=NULL, iEntity* parent=NULL);
	~iEntity();

	void setParent(iScene* scene); // 作为场景中的根实体
	void setParent(iEntity* parent); // 作为某实体的子实体

	void render();
	virtual void on_reset();

	void unload();

	D3DXMATRIX* updateWorldMatrix();

	bool fireMouseEnter(iEntity* eOld=NULL);
	bool fireMouseLeave(iEntity* eNew=NULL);
	bool fireClick();
	bool fireDblClick();

	// 射线相交信息
	struct IntersectInfo
	{
		BOOL hit;	// 是否命中。如果完全命中，值为1；如果命中边界盒，值为2；如果命中边界球，值为3
		DWORD face;	// 面索引
		float u, v, dist;	// 命中点的 BARY 坐标；命中点到射线原点的距离
		BOOL isBox; // 在 XML 属性中初始化这两个字段
		BOOL isSphere;

		IntersectInfo() { ZeroMemory(this, sizeof(IntersectInfo)); }
	};
	IntersectInfo m_hit;
	void updateIntersectInfo(iEntity*& eDst/*, iEntity* parent=NULL*/);

	// 如果有动画，下面的方法有效
	void setAnim(iAnimInstance* ai);
	void frameMove(float elapsedTime);
	UINT getAnimationSetCount();
	void setAnimationSet(ID3DXAnimationSet* as);
	void setAnimationSet(DWORD index);
	bool findAnimationSet(LPCOLESTR name, ID3DXAnimationSet** ppAnimationSet=NULL);
	void resetTime();

	static void createEntityByNode(HANDLE node, iScene* scene=NULL, iEntity* parent=NULL);

	void enumAllEntities(FINDENTITYPROC proc, LPVOID data, bool meFirst=true);
	void enumAllParents(FINDENTITYPROC proc, LPVOID data, bool meFirst=true);

	iEntity* getCoParent(iEntity* e1, iEntity* e2); // 寻找两个实体的最接近的公共父实体

private:
	// 递归渲染一个帧，包括所有的子容器、子帧、兄弟帧
	void drawFrame(iXModel::FrameEx* frame=NULL);
	// 渲染一个容器
	void drawMeshContainer(iXModel::MeshContainerEx* cont);

	// 递归设置变换矩阵
	static void updateFrameMatrices(iXModel::FrameEx* frame, const D3DXMATRIX* mtParent);

	static BOOL _enumEntity(iEntity* pStart, FINDENTITYPROC proc, LPVOID data, bool meFirst=true);
	static BOOL _enumParent(iEntity* pStart, FINDENTITYPROC proc, LPVOID data, bool meFirst=true);

	bool _fireMouseEnter(iEntity* eStop);
	bool _fireMouseLeave(iEntity* eStop);
};


// scene
class iScene : public Scene
			, public TList<iScene> // 作为资源保存在设备中
			, public IExtension
			, public ISinkIsMyEvent
{
	DECLARE_CLASS_NAME2(iScene, Scene)
	//DECLARE_GET_CLASS_NAME(iScene)
	//DECLARE_MODULE_HANDLE()
	DECLARE_NO_DELETE_THIS(iScene)
	DECLARE_CLASS_CAST(iScene, ISinkIsMyEvent)

public:
	iDevice* m_device;
	iSceneTarget m_target;
	iCamera m_camera;	// 摄像机
	iLightGroup m_lightGroup; // 光源集合
	iEntity* m_entities; // 实体对象列表
	static const DWORD m_defAmbientLight = 0xffffffff; // 默认环境光，如果用户环境光禁用，则启用默认环境光（白色）

	D3DXMATRIX m_world; // 世界矩阵，仅在特定条件下（比如做镜像）起作用
	bool m_mirror; // 是否有镜像
	bool m_current_mirror; // 当前是否在镜像状态
	//D3DXPLANE m_mirror_plane; // 镜子所处的面，由原点和法线创建
	D3DXVECTOR3 m_mirror_orig; // 默认 0,-1, 0
	D3DXVECTOR3 m_mirror_dir; // 默认 0, 1, 0

	bool m_rendered;

	bool m_mouse_inside; // 鼠标光标是否在场景内
	COLOR m_mouse_light; // 鼠标跟随灯光的颜色，如果全 0 表示不使用此灯光
	POINT m_ptMouse;
	iEntity* m_hover_entity; // 如果光标在实体上，则此实体是“热”实体。有可能是一个子孙实体

	long m_current_child_index; // 用于自动读取子GDI纹理的索引号，每获取一次就增加一次
public:
	iScene(iDevice* device=NULL);
	~iScene();
	//virtual void Dispose();

	virtual void on_reset();

	void render_scene();
	void render();
	void pre_render();

	D3DXMATRIX get_world_matrix();
	LPCSTR get_technique();

	void update_mirror_matrix();
	void set_current_mirror(bool bMirror);
	bool get_current_mirror();

	float width();
	float height();

	// 依据视口坐标（相对视口左上角），计算坐标点在当前场景中的射线，返回值为射线起点，pvDir为输出的射线方向（可选）
	D3DXVECTOR3 point_to_ray(POINT pt, D3DXVECTOR3* pvDir=NULL, D3DXMATRIX* pmtWorld=NULL);
	void update_state(); // 根据鼠标位置，更新当前状态，例如设置HOVER实体

	// 这个方法被网格调用，用以寻找对应的纹理
	// 场景应该首先自己处理纹理搜索，如果搜索不到，再调用设备的搜索函数
	IDirect3DTexture9* find_texture(iMaterial* m, bool bImageFirst=false);

	virtual BOOL IsMyEvent(HANDLE h, DuiEvent* pEvent);

	void unload();
};

#endif // __DUI3DOBJECT_H__
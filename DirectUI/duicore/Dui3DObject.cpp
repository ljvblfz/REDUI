#include "stdafx.h"
#include "Dui3DObject.h"
#include "DuiStyleParser.h"

//////////////////////////////////////////////////////////////////////////
iSceneTarget::iSceneTarget( iScene* scene ) :  m_scene(scene)
{
	ATLASSERT(m_scene);
}

iSceneTarget::~iSceneTarget()
{

}

void iSceneTarget::render_content()
{
	m_scene->render_scene();
}

//////////////////////////////////////////////////////////////////////////
iScene::iScene( iDevice* device ) : m_device(device)
		, m_target(this)
		, m_camera(this)
		, m_entities(NULL)
		, m_rendered(false)
		, m_mouse_inside(false)
		, m_hover_entity(NULL)
		, m_mirror(false), m_current_mirror(false), m_mirror_orig(0,-1.0f,0), m_mirror_dir(0,1.0f,0)
		, m_current_child_index(0)
{
	update_mirror_matrix();
	LightEnabled = true;
	Usage = SU_CONTENT;
	if (m_device)
		addToList(&m_device->m_scenes, false);
}

iScene::~iScene()
{
	if (m_entities) m_entities->TList<iEntity,cookie_sibling>::removeAll();
}

// void iScene::Dispose()
// {
// 	delete this;
// }

void iScene::on_reset()
{
	if (m_entities) m_entities->on_reset();

	if (next) next->on_reset();
}

void iScene::render_scene()
{
	if (m_target.m_quad == NULL) return;

	EnterRuntimeContext<iScene> ctx(m_device, this);

	PixEvent pe(L"Scene");

	{
		// 如果场景用于取代内容，子控件没有机会渲染自己，必须在这里提供渲染机会，以便用于动态纹理
		//if (Usage==SU_ALL || Usage==SU_CONTENT)
		{
			iQuadObject* c = m_target.m_quad->m_children;
			while (c)
			{
				c->SetAsTarget(TRUE);
				if (c->has_target())
					c->m_rendertarget->render();

				c = c->next;
			}
		}

		AutoStateDevice device(m_device->m_device);
		CComResult hr;
		CheckHResult hr = device.SetFVF(D3DFVF_VERTEX);
		//CheckHResult hr = device.SetRenderState(D3DRS_LIGHTING,  !!LightEnabled); // 没有使用光照，所以必须关闭光照，否则绘制的图元都是黑色

		//if (LightEnabled)
		//{
		//	// 设置环境光，如果已禁用用户定义的环境光，则启用默认环境光（白色）
		//	CheckHResult hr = device.SetRenderState(D3DRS_AMBIENT, m_lightGroup.m_ambientLight.m_Enabled ? 
		//		m_lightGroup.m_ambientLight.m_Color
		//		//D3DCOLOR_COLORVALUE(m_lightGroup.m_ambientLight.m_Color.r, m_lightGroup.m_ambientLight.m_Color.g, m_lightGroup.m_ambientLight.m_Color.b, m_lightGroup.m_ambientLight.m_Color.a)
		//		: m_defAmbientLight);

		//	DWORD l = 0;
		//	// 设置固定的 8 组灯光
		//	for (DWORD i=0; i<8; i++)
		//	{
		//		//CheckHResult hr = device.SetLight(i, &m_lightGroup.m_lights[i].m_Light);
		//		//CheckHResult hr = device.LightEnable(i, m_lightGroup.m_lights[i].m_Enabled!=false);
		//		// 先关闭所有灯光
		//		CheckHResult hr = device.LightEnable(i, FALSE);

		//		if (m_lightGroup.m_lights[i].m_Enabled)
		//		{
		//			CheckHResult hr = device->LightEnable(l, TRUE); // 不用记录状态，所以这里用 ->
		//			CheckHResult hr = device.SetLight(l, &m_lightGroup.m_lights[i].m_Light);
		//			l++;
		//		}
		//	}
		//}

		//D3DMATERIALCOLORSOURCE mcs = D3DMCS_MATERIAL;
		//CheckHResult hr = device->GetRenderState(D3DRS_AMBIENTMATERIALSOURCE, (DWORD*)&mcs); // default: D3DMCS_MATERIAL
		//CheckHResult hr = device->GetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, (DWORD*)&mcs); // default: D3DMCS_COLOR1 - DIFFUCE VERTEX COLOR
		//CheckHResult hr = device->GetRenderState(D3DRS_EMISSIVEMATERIALSOURCE, (DWORD*)&mcs); // default: D3DMCS_MATERIAL
		//CheckHResult hr = device->GetRenderState(D3DRS_SPECULARMATERIALSOURCE, (DWORD*)&mcs); // default: D3DMCS_COLOR2 - specular vertex color

		//CheckHResult hr = device.SetRenderState(D3DRS_COLORVERTEX, FALSE);
		//CheckHResult hr = device.SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_MATERIAL);
		//CheckHResult hr = device.SetRenderState(D3DRS_SPECULARMATERIALSOURCE, D3DMCS_MATERIAL);

		// 设置摄像机的投影和视口矩阵
		//CheckHResult hr = device.SetTransform(D3DTS_PROJECTION, &m_camera.get_projection_matrix());
		//CheckHResult hr = device.SetTransform(D3DTS_VIEW, &m_camera.get_view_matrix());

		m_device->m_system_effect.set_scene_params();
		D3DXVECTOR4 vOldPos, vOldDir, vOldColor, vOldPlane;
		D3DXHANDLE hMousePos = m_device->m_system_effect.m_effect->GetParameterByName(NULL, "vMousePosition");
		//D3DXHANDLE hMouseDir = m_device->m_system_effect.m_effect->GetParameterByName(NULL, "vMouseDir");
		D3DXHANDLE hMouseColor = m_device->m_system_effect.m_effect->GetParameterByName(NULL, "vMouseColor");
		if (m_mouse_inside)
		{
			m_device->m_system_effect.m_effect->GetVector(hMousePos, &vOldPos);
			//m_device->m_system_effect.m_effect->GetVector(hMouseDir, &vOldDir);
			m_device->m_system_effect.m_effect->GetVector(hMouseColor, &vOldColor);

			D3DXVECTOR3 vDir, vPos = point_to_ray(m_ptMouse, &vDir), vIntersect;
			D3DXPLANE plane;
			::D3DXPlaneFromPointNormal(&plane, &D3DXVECTOR3(0,0,0), &m_camera.m_Position);
			::D3DXPlaneIntersectLine(&vIntersect, &plane, &vPos, &(vPos+vDir));
			vIntersect += m_camera.m_Position;
			//vIntersect = -vIntersect;
			m_device->m_system_effect.m_effect->SetValue(hMousePos, vIntersect, sizeof(D3DXVECTOR3));
			//m_device->m_system_effect.m_effect->SetValue(hMouseDir, vIntersect, sizeof(D3DXVECTOR3));
			m_device->m_system_effect.m_effect->SetValue(hMouseColor, &m_mouse_light.r, sizeof(D3DXVECTOR3));
		}

		D3DXHANDLE hPlane = m_device->m_system_effect.m_effect->GetParameterByName(NULL, "g_mMirrorPlane");
		if (m_mirror)
		{
			m_device->m_system_effect.m_effect->GetVector(hPlane, &vOldPlane);
			D3DXPLANE plane;
			D3DXPlaneFromPointNormal(&plane, &m_mirror_orig, &m_mirror_dir);
			m_device->m_system_effect.m_effect->SetValue(hPlane, plane, sizeof(D3DXPLANE));
		}
		m_device->fire_event(&RenderCallback, GetDispatch());

		// render normal scene
		set_current_mirror(false);
		m_device->m_system_effect.set_scene_mirror_params();
		m_current_child_index = 0;
		if (m_entities) m_entities->render();

		// render mirror scene
		if (m_mirror)
		{
			set_current_mirror(true);
			m_device->m_system_effect.set_scene_mirror_params();

			m_current_child_index = 0;
			if (m_entities) m_entities->render();
			
			set_current_mirror(false);
			m_device->m_system_effect.set_scene_mirror_params();

			m_device->m_system_effect.m_effect->SetVector(hPlane, &vOldPlane);
		}

		if (m_mouse_inside)
		{
			m_device->m_system_effect.m_effect->SetVector(hMousePos, &vOldPos);
			//m_device->m_system_effect.m_effect->SetVector(hMouseDir, &vOldDir);
			m_device->m_system_effect.m_effect->SetVector(hMouseColor, &vOldColor);
		}
	}
}

void iScene::render()
{
	if (!m_rendered)
		m_target.render();

	CComResult hr;
	AutoStateDevice device(m_device->m_device);
	if (m_device->isLayered)
		CheckHResult hr = device.SetRenderState(D3DRS_BLENDOPALPHA, D3DBLENDOP_MAX);

	if (m_device->m_system_effect.m_effect)
	{
		m_device->m_system_effect.m_effect->SetTexture("g_tImage", m_target.texture());
		m_device->m_system_effect.set_gray(!m_target.m_quad->isEnable);
	}
	BeginSystemEffect(m_device, "ImageLayer");
	//CheckHResult hr = device.SetFVF(D3DFVF_VERTEX);
	CheckHResult hr = m_device->m_device->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, m_device->update_target_vertex(m_target.m_quad->m_vertexs), sizeof(VERTEX));
	EndEffect();

	m_rendered = false;
}

void iScene::pre_render()
{
	if (next)
		next->pre_render();

	m_target.render();
	m_rendered = true;
}

IDirect3DTexture9* iScene::find_texture( iMaterial* m, bool bImageFirst/*=false*/ )
{
	//if (m==NULL) return NULL;
	if (m==NULL || (m->index==-1 && m->texname.Length()==0))
	{
		// 寻找实体中定义的纹理属性
		if (m_device->m_runtime.entity && m_device->m_runtime.entity->m_Material.index==-1 && m_device->m_runtime.entity->m_Material.texname.Length()==0)
			return NULL;
		m = &m_device->m_runtime.entity->m_Material;
	}

	if (bImageFirst)
	{
		IDirect3DTexture9* tex = m_device->find_texture(m->texname);
		if (tex) return tex;
	}

	iQuadObject* q = m_target.m_quad->m_children;
	if (q)
	{
		// get by name
		if (m->index==-1)
		{
			while (q)
			{
				if (q->m_name && lstrcmpiW(q->m_name,m->texname)==0)
					return q->m_rendertarget->m_rendertarget.p;

				q = q->next;
			}
		}
		else if (m->index==AutoValue)
		{
			m_current_child_index++;
			if ((DWORD)m_current_child_index>=q->TOrderList<iQuadObject>::count())
				m_current_child_index = 0;
			iQuadObject* c = (iQuadObject*)q->TOrderList<iQuadObject>::getAt((DWORD)m_current_child_index);
			return c->m_rendertarget->m_rendertarget.p;
		}
		else if (m->index>=0 && (DWORD)m->index<q->TOrderList<iQuadObject>::count())
		{
			iQuadObject* c = (iQuadObject*)q->TOrderList<iQuadObject>::getAt((DWORD)m->index);
			return c->m_rendertarget->m_rendertarget.p;
		}
	}
	return bImageFirst ? NULL : m_device->find_texture(m->texname);
}

float iScene::width()
{
	if (m_target.m_quad == NULL) return 1.f;
	return (float)m_target.m_quad->m_width;
}

float iScene::height()
{
	if (m_target.m_quad == NULL) return 1.f;
	return (float)m_target.m_quad->m_height;
}

D3DXVECTOR3 iScene::point_to_ray( POINT pt, D3DXVECTOR3* pvDir/*=NULL*/, D3DXMATRIX* pmtWorld/*=NULL*/ )
{
	D3DXVECTOR3 vRayOrig(0,0,0);
	D3DXVECTOR3 vRayDir;

	// Get the inverse view matrix
	D3DXMATRIX mtProj = m_camera.get_projection_matrix();
	D3DXMATRIX mtView = m_camera.get_view_matrix();
	//D3DXMATRIX mtWorld; D3DXMatrixIdentity(&mtWorld);
	D3DXMATRIX mtWorldView = (pmtWorld ? *pmtWorld * mtView : mtView);
	D3DXMATRIX m;

	// Compute the vector of the pick ray in screen space
	D3DXVECTOR3 v;
	v.x = ( ( ( 2.0f * pt.x ) / width() ) - 1 ) / mtProj._11;
	v.y = -( ( ( 2.0f * pt.y ) / height() ) - 1 ) / mtProj._22;
	v.z = 1.0f;
	D3DXMatrixInverse( &m, NULL, &mtWorldView );

	//D3DXVec3TransformCoord(&vRayOrig, &vRayOrig, &m);

	//vRayDir = v;
	//D3DXVec3TransformNormal(&vRayDir, &vRayDir, &m);
	//D3DXVec3Normalize(&vRayDir, &vRayDir);

	// Transform the screen space pick ray into 3D space
	vRayDir.x = v.x * m._11 + v.y * m._21 + v.z * m._31;
	vRayDir.y = v.x * m._12 + v.y * m._22 + v.z * m._32;
	vRayDir.z = v.x * m._13 + v.y * m._23 + v.z * m._33;
	vRayOrig.x = m._41;
	vRayOrig.y = m._42;
	vRayOrig.z = m._43;

	if (pvDir)
		*pvDir = vRayDir;

	return vRayOrig;
}

D3DXMATRIX iScene::get_world_matrix()
{
	return (m_mirror && m_current_mirror) ? m_world : ::Matrix::GetIdentityMatrix();
}

LPCSTR iScene::get_technique()
{
	return (m_mirror && m_current_mirror) ? "SystemMirror" : "SystemDefault";
}

void iScene::set_current_mirror( bool bMirror )
{
	m_current_mirror = bMirror;

	////if (m_mirror)
	//{
	//	m_device->m_device->SetRenderState(D3DRS_CLIPPLANEENABLE, D3DCLIPPLANE0);
	//	//if (m_current_mirror)
	//	{
	//		D3DXPLANE plane;
	//		::D3DXPlaneFromPointNormal(&plane, &D3DXVECTOR3(0,-.5f,0)/*m_mirror_orig*/, &m_mirror_dir);
	//		m_device->m_device->SetClipPlane(0, plane);
	//	}
	//}
	////else
	////{
	////	m_device->m_device->SetRenderState(D3DRS_CLIPPLANEENABLE, 0);
	////}
}

bool iScene::get_current_mirror()
{
	return m_current_mirror;
}

void iScene::update_mirror_matrix()
{
	// 构造镜像矩阵
	D3DXPLANE plane;
	::D3DXPlaneFromPointNormal(&plane, &m_mirror_orig, &m_mirror_dir);
	::D3DXMatrixReflect(&m_world, &plane);
}

void iScene::update_state()
{
	if (!m_mouse_inside)
	{
		if (m_hover_entity) m_hover_entity->fireMouseLeave();
		m_hover_entity = NULL;
		return;
	}

	iEntity* e = NULL;
	if (m_entities) m_entities->updateIntersectInfo(e);

	if (e != m_hover_entity)
	{
		if (m_hover_entity) m_hover_entity->fireMouseLeave(e);
		iEntity* eOld = m_hover_entity;
		m_hover_entity = e;
		if (m_hover_entity) m_hover_entity->fireMouseEnter(eOld);
	}
}

BOOL iScene::IsMyEvent( HANDLE h, DuiEvent* pEvent )
{
	DuiEvent& event = *pEvent;
	if (Usage!=SU_ALL && Usage!=SU_CONTENT) return FALSE;

	EnterRuntimeContext<iScene> context(m_device, this);

	DUI_EVENTTYPE et = DuiNode::NameToEventType(event.Name);
	switch (et)
	{
	case DUIET_click:
		if (m_hover_entity==NULL) return FALSE;
		m_hover_entity->fireClick();
		return TRUE;
	case DUIET_dblclick:
		if (m_hover_entity==NULL) return FALSE;
		m_hover_entity->fireDblClick();
		return TRUE;
	}

	return FALSE;
}

void iScene::unload()
{
	RenderCallback.Clear();
	if (m_entities) m_entities->unload();

	if (next) next->unload();
}

//////////////////////////////////////////////////////////////////////////
iCamera::iCamera( iScene* scene ) : m_scene(scene)
{
	reset_params();
}

iCamera::~iCamera()
{

}

void iCamera::reset_params()
{
	m_Position = D3DXVECTOR3(0,0,-5.f);
	m_right = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
	m_up = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	m_ahead = D3DXVECTOR3(0.0f, 0.0f, 1.0f);

	m_Pitch = m_Yaw = m_Roll = 0.0f;
	m_Zoom = 1.0f;
	m_Fov = 45.0f;
	m_NearPlane = ZNear;
	m_FarPlane = ZFar;
	m_Ortho = false;
}

D3DXMATRIX iCamera::get_view_matrix()
{
	D3DXMATRIX mtTrans, mtView;
	D3DXMatrixIdentity (&mtView);

	D3DXVECTOR3 vUp=m_up, vRight=m_right, vAhead=m_ahead;

	// Yaw is rotation around the y axis (up)
	D3DXMATRIX mtYaw;
	D3DXMatrixRotationAxis(&mtYaw, &vUp, D3DXToRadian(m_Yaw));
	D3DXVec3TransformCoord(&vAhead, &vAhead, &mtYaw); 
	D3DXVec3TransformCoord(&vRight, &vRight, &mtYaw); 

	// Pitch is rotation around the x axis (right)
	D3DXMATRIX mtPitch;
	D3DXMatrixRotationAxis (&mtPitch, &vRight, D3DXToRadian(m_Pitch));
	D3DXVec3TransformCoord(&vAhead, &vAhead, &mtPitch); 
	D3DXVec3TransformCoord(&vUp, &vUp, &mtPitch); 

	// Roll is rotation around the z axis (ahead)
	D3DXMATRIX mtRoll;
	D3DXMatrixRotationAxis(&mtRoll, &vAhead, D3DXToRadian(m_Roll));
	D3DXVec3TransformCoord(&vRight, &vRight, &mtRoll); 
	D3DXVec3TransformCoord(&vUp, &vUp, &mtRoll); 

	// Build the view matrix from the transformed camera axis
	mtView._11 = vRight.x; mtView._12 = vUp.x; mtView._13 = vAhead.x;
	mtView._21 = vRight.y; mtView._22 = vUp.y; mtView._23 = vAhead.y;
	mtView._31 = vRight.z; mtView._32 = vUp.z; mtView._33 = vAhead.z;

	mtView._41 = - D3DXVec3Dot(&m_Position, &vRight); 
	mtView._42 = - D3DXVec3Dot(&m_Position, &vUp);
	mtView._43 = - D3DXVec3Dot(&m_Position, &vAhead);

	// ---- Zoom ----
	D3DXMatrixScaling (&mtTrans, m_Zoom, m_Zoom, m_Zoom);
	D3DXMatrixMultiply (&mtView, &mtTrans, &mtView);

	return mtView;
}

D3DXMATRIX iCamera::get_projection_matrix()
{
	D3DXMATRIX mtProj;
	if (m_Ortho)
		D3DXMatrixOrthoLH(&mtProj, (float)m_scene->width(), (float)m_scene->height(), m_NearPlane, m_FarPlane);
	else
		D3DXMatrixPerspectiveFovLH(&mtProj, D3DXToRadian(m_Fov), (float)m_scene->width()/(float)m_scene->height(), m_NearPlane, m_FarPlane);

	return mtProj;
}

HRESULT Camera::SetProperties( VARIANT* val )
{
	CComVariant v;
	HRESULT hr = v.ChangeType(VT_BSTR, val);
	if (FAILED(hr)) return hr;

	CStrArray strs;
	if (SplitStringToArray(V_BSTR(&v), strs, L",;"))
	{
		for (int i=0; i<strs.GetSize(); i++)
		{
			CStrArray strItems;
			if (SplitStringToArray(strs[i], strItems, L":=") && strItems.GetSize()>=2)
			{
				double d;
				if (strItems[0].CompareNoCase(L"posx")==0) { if (TryLoad_double_FromString(strItems[1], d)) m_Position.x = (float)d; }
				else if (strItems[0].CompareNoCase(L"posy")==0) { if (TryLoad_double_FromString(strItems[1], d)) m_Position.y = (float)d; }
				else if (strItems[0].CompareNoCase(L"posz")==0) { if (TryLoad_double_FromString(strItems[1], d)) m_Position.z = (float)d; }
#define try_load_float(prop) else if (strItems[0].CompareNoCase(L#prop)==0) { if (TryLoad_double_FromString(strItems[1], d)) m_##prop = (float)d; }
#define try_load_bool(prop) else if (strItems[0].CompareNoCase(L#prop)==0) { m_##prop = !!ParseBoolString(strItems[1]); }
				try_load_bool(Ortho)
				try_load_float(Pitch)
				try_load_float(Yaw)
				try_load_float(Roll)
				try_load_float(NearPlane)
				try_load_float(FarPlane)
#undef try_load_float
#undef try_load_bool
			}
		}
	}

	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
iModelBase::iModelBase(iDevice* device) : m_device(device?device:iDevice::get_active_device())
		, TList<iModelBase,cookie_child>(&iModelBase::m_children)
		, TList<iModelBase,cookie_sibling>(&iModelBase::m_siblings)
		, m_parent(NULL), m_children(NULL), m_siblings(NULL)
		, m_entities(NULL)
		, m_matNum(0), m_materials(NULL), m_useMaterials(true)
{
	//ATLASSERT(m_device);
	if (m_device)
		TList<iModelBase>::addToList(&m_device->m_models);
}

iModelBase::~iModelBase()
{
	if (m_entities) m_entities->TList<iEntity>::removeAll();

	if (m_materials) m_materials = ((delete[] m_materials), NULL);

	//for (int i=0; i<m_mapped_materials.GetSize(); i++)
	//	if (m_mapped_materials[i]) delete m_mapped_materials[i];
	//m_mapped_materials.RemoveAll();

	if (m_children) m_children->TList<iModelBase,cookie_sibling>::removeAll();
	if (m_siblings) m_siblings->TList<iModelBase,cookie_sibling>::removeAll();
}

void iModelBase::setParent(iDevice* device)
{
	if (device==NULL) return;

	bool useRefCount = true;
	if (m_device && m_device!=device)
	{
		TList<iModelBase>::removeFromList(false);
		useRefCount = false;
	}

	m_device = device;
	if (m_device)
		TList<iModelBase>::addToList(&m_device->m_models, useRefCount);
}

void iModelBase::setParent(iModelBase* parent)
{
	if (parent==NULL) return;
	m_device = parent->m_device;

	bool useRefCount = true;
	if (m_parent && m_parent!=parent)
	{
		TList<iModelBase,cookie_sibling>::removeFromList(false);
		useRefCount = false;
	}

	m_parent = parent;
	TList<iModelBase,cookie_sibling>::addToList(&m_parent->m_children, useRefCount);
}

void iModelBase::setFVF( DWORD fvf )
{
	if (m_mesh.p == NULL) return;

	DWORD oldfvf = m_mesh->GetFVF();
	if (oldfvf == fvf) return;

	D3DVERTEXELEMENT9 decl[MAX_FVF_DECL_SIZE];
	D3DXDeclaratorFromFVF(fvf, decl);
	setVertexDecl(decl, false);

	//CComPtr<ID3DXMesh> tmpMesh;
	//if (FAILED(m_mesh->CloneMeshFVF(m_mesh->GetOptions(), fvf, m_device->m_device.p, &tmpMesh))) return;
	//m_mesh = NULL;
	//m_mesh = tmpMesh;

	//if (!(oldfvf & D3DFVF_NORMAL) && (fvf & D3DFVF_NORMAL))
	//	D3DXComputeNormals(m_mesh, NULL);
}

void iModelBase::setVertexDecl( const D3DVERTEXELEMENT9* pDecl, bool bSplitVertexForOptimalTangents/* = false*/ )
{
	if (m_mesh.p == NULL || pDecl==NULL) return;

	CComPtr<ID3DXMesh> tmpMesh;
	if (FAILED(m_mesh->CloneMesh(m_mesh->GetOptions(), pDecl, m_device->m_device.p, &tmpMesh)))
		return;

	// 检测原始顶点申明是否包含法线和切线
	bool bHadNormal = false;
	bool bHadTangent = false;
	D3DVERTEXELEMENT9 aOldDecl[MAX_FVF_DECL_SIZE];
	if (SUCCEEDED(m_mesh->GetDeclaration(aOldDecl)))
	{
		for (UINT i=0, num=D3DXGetDeclLength(aOldDecl); i<num; i++)
		{
			if (aOldDecl[i].Usage == D3DDECLUSAGE_NORMAL)
				bHadNormal = true;
			if (aOldDecl[i].Usage == D3DDECLUSAGE_TANGENT)
				bHadTangent = true;
		}
	}

	// 检测新顶点申明是否包含法线和切线
	bool bHaveNormalNow = false;
	bool bHaveTangentNow = false;
	D3DVERTEXELEMENT9 aNewDecl[MAX_FVF_DECL_SIZE];
	if (SUCCEEDED(tmpMesh->GetDeclaration(aNewDecl)))
	{
		for (UINT i=0, num=D3DXGetDeclLength(aNewDecl); i<num; i++)
		{
			if (aNewDecl[i].Usage == D3DDECLUSAGE_NORMAL)
				bHaveNormalNow = true;
			if (aNewDecl[i].Usage == D3DDECLUSAGE_TANGENT)
				bHaveTangentNow = true;
		}
	}

	m_mesh = NULL;
	m_mesh = tmpMesh;
	tmpMesh = NULL;
	if (!bHadNormal && bHaveNormalNow)
		D3DXComputeNormals(m_mesh, NULL);

	if (bHaveNormalNow && !bHadTangent && bHaveTangentNow)
	{
		DWORD* pdwAdjacency = NEW DWORD[m_mesh->GetNumFaces() * 3];
		if (pdwAdjacency == NULL) return;

		CComResult hr;
		CheckHResult hr = m_mesh->GenerateAdjacency(1e-6f, pdwAdjacency);

		float fPartialEdgeThreshold;
		float fSingularPointThreshold;
		float fNormalEdgeThreshold;
		if( bSplitVertexForOptimalTangents )
		{
			fPartialEdgeThreshold = 0.01f;
			fSingularPointThreshold = 0.25f;
			fNormalEdgeThreshold = 0.01f;
		}
		else
		{
			fPartialEdgeThreshold = -1.01f;
			fSingularPointThreshold = 0.01f;
			fNormalEdgeThreshold = -1.01f;
		}

		// Compute tangents, which are required for normal mapping
		CheckHResult hr = D3DXComputeTangentFrameEx( m_mesh,
			D3DDECLUSAGE_TEXCOORD, 0,
			D3DDECLUSAGE_TANGENT, 0,
			D3DX_DEFAULT, 0,
			D3DDECLUSAGE_NORMAL, 0,
			0, pdwAdjacency,
			fPartialEdgeThreshold, fSingularPointThreshold, fNormalEdgeThreshold,
			&tmpMesh, NULL );

		delete[] pdwAdjacency;
		if (FAILED(hr)) return;
		m_mesh = NULL;
		m_mesh = tmpMesh;
	}
}

iEntity* iModelBase::createEntity()
{
	return NEW iEntity(this);
}

void iModelBase::render()
{
	if (m_mesh.p == NULL)
	{
		createMesh();
		if (m_mesh.p)
		{
			setFVF(m_mesh->GetFVF() | D3DFVF_NORMAL);
			if (m_matNum==0)
			{
				m_matNum = 1;
				m_materials = NEW iMaterial[m_matNum]; // 至少创建一个材质
				// 如果只有一个材质，则可以在模型节点中直接读取材质属性
				if (m_materials)
					m_materials->ParseAttribute(&m_node);
			}
			// 读取材质属性
			DWORD i=0;
			for (CMarkupNode child=m_node.GetChild(); child.IsValid(); child=child.GetSibling())
			{
				if (lstrcmpiW(child.GetName(), L"material")==0)
				{
					m_materials[i].ParseAttribute(&child);
					i++;
					if (i>=m_matNum)
						break;
				}
			}
		}
	}
	if (m_mesh.p == NULL)
	{
		if (m_children) m_children->render();
		if (m_siblings) m_siblings->render();
		return;
	}

	{
		EnterRuntimeContext<iModelBase> ctx_model(m_device, this);

		m_device->m_system_effect.set_model_params(&m_world);
		m_device->fire_event(&onRender, getDispatch());

		PixEvent pe(L"Model");
		//// 如果模型不带材质，则使用实体材质
		//if (m_matNum==0)
		//{
		//	oldm = m_device->m_runtime.set_material(&entity->m_Material);
		//	oldt = m_device->m_runtime.set_texture(entity->m_Material.texture.p ? entity->m_Material.texture.p : entity->m_scene->find_texture(entity->m_Material.texname));
		//	//m_device->m_device->SetMaterial(&entity->m_Material.material);
		//	//m_device->m_device->SetTexture(0, entity->m_Material.texture.p ? entity->m_Material.texture.p : entity->m_scene->find_texture(entity->m_Material.texname));

		//	//m_device->m_system_effect.m_effect->SetTexture("g_tImage", entity->m_Material.texture.p ? entity->m_Material.texture.p : entity->m_scene->find_texture(entity->m_Material.texname));
		//	BeginSystemEffect(m_device, "TexturedSpecular");
		//	m_device->fire_event(&entity->m_Material.onRender, entity->m_Material.GetDispatch());
		//	m_mesh->DrawSubset(0);
		//	EndEffect();
		//	m_device->m_runtime.set_material(oldm);
		//	m_device->m_runtime.set_texture(oldt);
		//	return;
		//}

		//// 优先渲染所有不透明子集，透明的放在后面渲染，可有效提高渲染效率
		//for (DWORD i=0; i<m_matNum; i++)
		//{
		//	iMaterial* m = NULL;
		//	if (m_useMaterials && m_materials)
		//		m = &m_materials[i];
		//	else if (entity->m_Material.m_Enabled)
		//		m = &entity->m_Material;

		//	if (m)
		//	{
		//		if (m->material.Diffuse.a < 1.0f) continue; // 跳过透明部分
		//		//m_device->m_device->SetMaterial(&m->material);
		//		//m_device->m_device->SetTexture(0, m->texture.p ? m->texture.p : entity->m_scene->find_texture(m->texname));
		//		oldm = m_device->m_runtime.set_material(m);
		//		oldt = m_device->m_runtime.set_texture(m->texture.p ? m->texture.p : entity->m_scene->find_texture(m->texname));
		//		m_device->fire_event(&m->onRender, m->GetDispatch());
		//	}

		//	//m_device->m_system_effect.m_effect->SetTexture("g_tImage", m->texture.p ? m->texture.p : entity->m_scene->find_texture(m->texname));
		//	BeginSystemEffect(m_device, "TexturedSpecular");
		//	m_mesh->DrawSubset(i);
		//	EndEffect();
		//	m_device->m_runtime.set_material(oldm);
		//	m_device->m_runtime.set_texture(oldt);
		//}

		//if (m_useMaterials)
		{
			for (DWORD i=0; i</*max(1,*/m_matNum/*)*/; i++)
			{
				iMaterial* m = /*(m_matNum>0 ?*/ &m_materials[i] /*: &m_device->m_runtime.entity->m_Material)*/;

				//if (m)
				{
					//if (m->material.Diffuse.a == 1.0f) continue; // 跳过不透明部分
					//m_device->m_device->SetMaterial(&m->material);
					//m_device->m_device->SetTexture(0, m->texture.p ? m->texture.p : entity->m_scene->find_texture(m->texname));
					EnterRuntimeContext<iMaterial> ctx_material(m_device, m);
					EnterRuntimeContext<IDirect3DBaseTexture9> ctx_texture(m_device, m->texture.p ? m->texture.p : m_device->m_runtime.scene->find_texture(m));
					//oldm = m_device->m_runtime.set_material(m);
					//oldt = m_device->m_runtime.set_texture(m->texture.p ? m->texture.p : m_device->m_runtime.scene->find_texture(m));
					m_device->fire_event(&m->onRender/*, m->GetDispatch()*/); // 因为材质不带引用计数，这里不能传递this，否则脚本引擎可能缓存材质对象，导致布局关闭时释放出错

					if (m->effect==NULL)
					{
						if (m->technique)
							m->effect = iEffect::findTechnique(m_device->m_effects, (m->technique));
						if (m->effect==NULL)
						{
							m->effect = &m_device->m_system_effect;
							m->technique.Empty(); // L"SystemDefault";
						}
					}
					if (m->effect == &m_device->m_system_effect)
						m_device->m_system_effect.set_texture_params();

					UINT numPass = m->effect->begin((m->technique ? m->technique : m_device->m_runtime.scene->get_technique()));

					// 效果有效，此时调用三个对象的 onPrepareEffect 事件，用来设置个性化效果参数
					m->effect->m_current_operator = iEffect::operator_material;
					m_device->fire_event(&m->onPrepareEffect/*, m->GetDispatch()*/);

					m->effect->m_current_operator = iEffect::operator_model;
					m_device->fire_event(&m_device->m_runtime.model->onPrepareEffect, m_device->m_runtime.model->getDispatch());

					m->effect->m_current_operator = iEffect::operator_entity;
					m_device->fire_event(&m_device->m_runtime.entity->onPrepareEffect, m_device->m_runtime.entity->GetDispatch());

					m->effect->m_current_operator = iEffect::operator_none;

					for (UINT ip=0; ip<numPass; ip++)
					{
						m->effect->beginPass(ip);
						m_mesh->DrawSubset(i);
						m->effect->endPass();
					}
					m->effect->end();
				}
			}
		}
	}

	if (m_children) m_children->render();
	if (m_siblings) m_siblings->render();
}

void iModelBase::getMaterials( CDispatchArray* pArr )
{
	ATLASSERT(pArr);
	for (DWORD i=0; i<m_matNum; i++)
	{
		pArr->Add(m_materials[i].GetDispatch(), m_materials[i].texname);
	}

	if (m_children) m_children->getMaterials(pArr);
	if (m_siblings) m_siblings->getMaterials(pArr);
}

void iModelBase::_updateMeshInfo()
{
	if (m_mesh.p == NULL) return;

	D3DVERTEXELEMENT9 decl[MAX_FVF_DECL_SIZE];
	m_meshInfo.numVertices = m_mesh->GetNumVertices();
	m_meshInfo.numFaces = m_mesh->GetNumFaces();
	m_meshInfo.bytesPerVertex = m_mesh->GetNumBytesPerVertex();
	m_mesh->GetIndexBuffer(&m_meshInfo.ib);
	m_mesh->GetVertexBuffer(&m_meshInfo.vb);
	m_mesh->GetDeclaration(decl);
	m_device->m_device->CreateVertexDeclaration(decl, &m_meshInfo.decl);
	D3DXVECTOR3* p = NULL;
	m_meshInfo.vb->Lock(0, 0, (void**)&p, D3DLOCK_READONLY);
	D3DXComputeBoundingBox(p, m_meshInfo.numVertices, m_meshInfo.bytesPerVertex, &m_meshInfo.boundMin, &m_meshInfo.boundMax);
	m_meshInfo.boundCenter = (m_meshInfo.boundMin + m_meshInfo.boundMax) / 2;
	D3DXComputeBoundingSphere(p, m_meshInfo.numVertices, m_meshInfo.bytesPerVertex, &m_meshInfo.sphereCenter, &m_meshInfo.sphereRadius);
	m_meshInfo.vb->Unlock();
	m_meshInfo.clear();

	if (m_children) m_children->_updateMeshInfo();
	if (m_siblings) m_siblings->_updateMeshInfo();
}

iModelBase* iModelBase::fromObject( IUnknown* unk )
{
	if (unk==NULL) return NULL;

#define processModel(model) { model* p = DISP_CAST(unk, model); if (p) return (iModelBase*)(i##model*)p; }
	__foreach_model(processModel)
#undef processModel

	return NULL;
}

void iModelBase::resetMesh()
{
	//m_meshInfo.clear();
	m_mesh = NULL;
	//createMesh();

	if (m_children) m_children->resetMesh();
	if (m_siblings) m_siblings->resetMesh();
}

void iModelBase::on_reset()
{
	effect_params.clear();

	for (DWORD i=0; i<m_matNum; i++)
		m_materials[i].on_reset();

	if (m_children) m_children->on_reset();
	if (m_siblings) m_siblings->on_reset();
}

void iModelBase::unload()
{
	onTransform.Clear();
	onRender.Clear();
	onPrepareEffect.Clear();
	onMouseEnter.Clear();
	onMouseLeave.Clear();
	onClick.Clear();
	onDblClick.Clear();

	if (m_children) m_children->unload();
	if (m_siblings) m_siblings->unload();
	if (next) next->unload();
}

void iModelBase::updateMatrices( const D3DXMATRIX* mtParent )
{
	D3DXMatrixMultiply(&m_world, m_local_transform.GetMatrix(), mtParent);

	if (m_children) m_children->updateMatrices(&m_world);
	if (m_siblings) m_siblings->updateMatrices(mtParent);
}

#define BuildModelResource(model) \
	i##model* mm = (i##model*)model::CreateInstance(); \
	m = mm; \
	if (mm==NULL) return; \
	if (parent) mm->setParent(parent); \
	else mm->setParent(device); \
	if (ppDisp) { *ppDisp = mm->getDispatch(); mm->getDispatch()->AddRef(); } \
	__if_exists(i##model::ParseAttribute) { mm->ParseAttribute(n); }

void iModelBase::createModelByNode( HANDLE node, iDevice* device/*=NULL*/, iModelBase* parent/*=NULL*/, IDispatch** ppDisp/*=NULL*/ )
{
	ATLASSERT(node && (device || parent));
	//if (n==NULL || (device==NULL && parent==NULL)) return;
	CMarkupNode* n = (CMarkupNode*)node;

	// 如果未指定模型类型，则如果有 src，则此模型是XModel，否则是自定义模型
	LPCOLESTR type = n->GetAttributeValue(L"type");
	LPCOLESTR src = n->GetAttributeValue(L"src");
	LPCOLESTR points = n->GetAttributeValue(L"points");

	iModelBase* m = NULL;
	// XModel 外部 X 文件
	if (lstrcmpiW(type, L"x")==0 || (type==NULL && src))
	{
		BuildModelResource(XModel)
	}

	// QuadModel 方形模型
	else if (lstrcmpiW(type, L"quad")==0)
	{
		BuildModelResource(QuadModel)
	}

	// BoxModel 长方体模型
	else if (lstrcmpiW(type, L"box")==0)
	{
		BuildModelResource(BoxModel)
	}

	// CylinderModel 圆柱体模型
	else if (lstrcmpiW(type, L"cylinder")==0)
	{
		BuildModelResource(CylinderModel)
	}

	// PolygonModel 多边形模型
	else if (lstrcmpiW(type, L"polygon")==0)
	{
		BuildModelResource(PolygonModel)
	}

	// SphereModel 球体模型
	else if (lstrcmpiW(type, L"sphere")==0)
	{
		BuildModelResource(SphereModel)
	}

	// TorusModel 圆环体（轮胎）模型
	else if (lstrcmpiW(type, L"torus")==0)
	{
		BuildModelResource(TorusModel)
	}

	// TextModel 文字模型
	else if (lstrcmpiW(type, L"text")==0)
	{
		BuildModelResource(TextModel)
	}

	// TeapotModel 茶壶模型
	else if (lstrcmpiW(type, L"teapot")==0)
	{
		BuildModelResource(TeapotModel)
	}

	// Model 用户自定义模型
	else if (/*lstrcmpiW(type, L"custom")==0 ||*/ (/*type==NULL &&*/ points))
	{
		BuildModelResource(Model)
	}

	// NullModel 空模型
	else // if (lstrcmpiW(type, L"null")==0)
	{
		BuildModelResource(NullModel)
	}

	m->m_node = *n;

	// 变换属性可能也在模型节点属性里
	m->m_local_transform.ParseAttribute((HANDLE)n);

	m->m_name = n->GetAttributeValue(L"name", L"id");
	m->effect_params.set_params_string(n->GetAttributeValue(L"EffectParameters", L"Parameters"));

	m->onTransform = n->GetAttributeValue(L"onTransform");
	m->onRender = n->GetAttributeValue(L"onRender", L"Render");
	m->onPrepareEffect = n->GetAttributeValue(L"onPrepareEffect", L"PrepareEffect");
	m->onMouseEnter = n->GetAttributeValue(L"onMouseEnter", L"onMouseOver", L"MouseEnter", L"MouseOver");
	m->onMouseLeave = n->GetAttributeValue(L"onMouseLeave", L"onMouseOut", L"MouseLeave", L"MouseOut");
	m->onClick = n->GetAttributeValue(L"onClick", L"Click");
	m->onDblClick = n->GetAttributeValue(L"onDblClick", L"DblClick");

	// 处理子节点，可能包括材质、子模型
	for (CMarkupNode child=n->GetChild(); child.IsValid(); child=child.GetSibling())
	{
		LPCOLESTR tag = child.GetName();
		if (lstrcmpiW(tag, L"Model")==0)
		{
			createModelByNode(&child, NULL, m);
		}
		//else if (lstrcmpiW(tag, L"Material")==0)
		//{
		//	// 仅当有名称属性时才创建一个映射的材质
		//	iMaterial* pMat = new iMaterial;
		//	if (pMat)
		//	{
		//		if (/*child.GetAttributeValue(L"name", L"id") &&*/ m->m_mapped_materials.Add(pMat))
		//		{
		//			pMat->ParseAttribute((HANDLE)&child);
		//		}
		//		else delete pMat;
		//	}
		//}
		else if (lstrcmpiW(tag, L"Transform")==0)
		{
			m->m_local_transform.ParseAttribute((HANDLE)&child);
		}
	}
}

BOOL iModelBase::_enumModel( iModelBase* pStart, FINDMODELPROC proc, LPVOID data )
{
	if (!proc(pStart, data)) return FALSE;

	if (pStart->m_children && !_enumModel(pStart->m_children, proc, data)) return FALSE;
	if (pStart->m_siblings && !_enumModel(pStart->m_siblings, proc, data)) return FALSE;
	return TRUE; // continue
}

void iModelBase::enumAllModels( FINDMODELPROC proc, LPVOID data )
{
	if (proc==NULL) return;
	_enumModel(this, proc, data);
}

void iModelBase::pre_render()
{
	if (next) next->pre_render();
}

//////////////////////////////////////////////////////////////////////////
STDMETHODIMP iXModel::MeshAllocator::CreateFrame( LPCSTR Name, LPD3DXFRAME *ppNewFrame )
{
	if (ppNewFrame==NULL) return E_POINTER;
	*ppNewFrame = NULL;

	FrameEx* frame = NEW FrameEx;
	if (frame==NULL) return E_OUTOFMEMORY;
	ZeroMemory(frame, sizeof(FrameEx));


	//if (Name)
		frame->name = Name; // Name 有可能为NULL

	D3DXMatrixIdentity(&frame->TransformationMatrix);
	D3DXMatrixIdentity(&frame->exCombinedTransformationMatrix);

	*ppNewFrame = frame;
	return S_OK;
}

STDMETHODIMP iXModel::MeshAllocator::CreateMeshContainer( LPCSTR Name, CONST D3DXMESHDATA *pMeshData, CONST D3DXMATERIAL *pMaterials, CONST D3DXEFFECTINSTANCE *pEffectInstances, DWORD NumMaterials, CONST DWORD *pAdjacency, LPD3DXSKININFO pSkinInfo, LPD3DXMESHCONTAINER *ppNewMeshContainer )
{
	if (ppNewMeshContainer==NULL) return E_POINTER;
	*ppNewMeshContainer = NULL;

	// The mesh type (D3DXMESHTYPE_MESH, D3DXMESHTYPE_PMESH or D3DXMESHTYPE_PATCHMESH)
	if (pMeshData->Type!=D3DXMESHTYPE_MESH) return E_FAIL; // 暂时仅处理MESH类型

	// 创建容器
	MeshContainerEx* cont = NEW MeshContainerEx;
	if (cont==NULL) return E_OUTOFMEMORY;
	//ZeroMemory(cont, sizeof(MeshContainerEx));

	cont->name = Name;

	// 复制MESH数据
	cont->MeshData = *pMeshData;
	cont->MeshData.pMesh->AddRef();

	// 复制邻接信息
	DWORD dwFaces = pMeshData->pMesh->GetNumFaces();
	cont->pAdjacency = NEW DWORD[dwFaces*3];
	memcpy_s(cont->pAdjacency, sizeof(DWORD)*dwFaces*3, pAdjacency, sizeof(DWORD)*dwFaces*3);

	CComPtr<IDirect3DDevice9> device;
	pMeshData->pMesh->GetDevice(&device);

	// 创建材质，如果没有材质，则创建一个默认材质
	cont->NumMaterials = max(NumMaterials, 1);
	cont->exMaterials = NEW iMaterial[cont->NumMaterials];
	//ZeroMemory(cont->exMaterials, sizeof(iMaterial)*cont->NumMaterials);
	if (NumMaterials>0)
	{
		for (DWORD i=0; i<NumMaterials; i++)
		{
			cont->exMaterials[i].material = pMaterials[i].MatD3D;
			cont->exMaterials[i].texname = (pMaterials[i].pTextureFilename);
			// TODO:这里需要创建纹理
			cont->exMaterials[i].load(device);
		}
	}
	// 默认创建一个材质
	else
	{
		cont->exMaterials[0].material.Diffuse.a = 1.0f;
		cont->exMaterials[0].material.Diffuse.r = 0.5f;
		cont->exMaterials[0].material.Diffuse.g = 0.5f;
		cont->exMaterials[0].material.Diffuse.b = 0.5f;
		cont->exMaterials[0].material.Specular = cont->exMaterials[0].material.Diffuse;
	}

	// 处理蒙皮信息
	if (pSkinInfo)
	{
		cont->pSkinInfo = pSkinInfo;
		pSkinInfo->AddRef();

		// 处理骨骼信息
		DWORD dwNumBones = pSkinInfo->GetNumBones();
		cont->exBoneOffsets = NEW D3DXMATRIX[dwNumBones];
		cont->exFrameCombinedMatrixPointer = NEW D3DXMATRIX*[dwNumBones]; // 分配待用
		ZeroMemory(cont->exFrameCombinedMatrixPointer, sizeof(D3DXMATRIX*)*dwNumBones);
		for (DWORD i=0; i<dwNumBones; i++)
			cont->exBoneOffsets[i] = *pSkinInfo->GetBoneOffsetMatrix(i);
	}

	// 处理特效
	if (pEffectInstances)
	{
		// TODO
	}

	*ppNewMeshContainer = cont;
	return S_OK;
}

STDMETHODIMP iXModel::MeshAllocator::DestroyFrame( LPD3DXFRAME pFrameToFree )
{
	FrameEx* frame = (FrameEx*)pFrameToFree;
	delete frame;
	return S_OK;
}

STDMETHODIMP iXModel::MeshAllocator::DestroyMeshContainer( LPD3DXMESHCONTAINER pMeshContainerToFree )
{
	MeshContainerEx* cont = (MeshContainerEx*)pMeshContainerToFree;
	if (cont==NULL) return S_OK;

	if (cont->MeshData.pMesh) cont->MeshData.pMesh = (cont->MeshData.pMesh->Release(), NULL);

	if (cont->pAdjacency) cont->pAdjacency = (delete[] cont->pAdjacency, NULL);

	if (cont->exMaterials) cont->exMaterials = (delete[] cont->exMaterials, NULL);

	if (cont->exFrameCombinedMatrixPointer) cont->exFrameCombinedMatrixPointer = (delete[] cont->exFrameCombinedMatrixPointer, NULL);

	if (cont->exBoneOffsets) cont->exBoneOffsets = (delete[] cont->exBoneOffsets, NULL);

	if (cont->pSkinInfo) cont->pSkinInfo = (cont->pSkinInfo->Release(), NULL);

	delete cont;
	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
iAnimInstance::iAnimInstance( iXModel* m, ID3DXAnimationController* ac ) : model(m)
			, curTrack(0)
			, curTime(0.0f)
			, speedAdjust(1.0f)
			, moveTransitionTime(0.25f)
{
	ATLASSERT(model && ac);
	CComResult hr;
	CheckHResult hr = ac->CloneAnimationController(ac->GetMaxNumAnimationOutputs(),
												ac->GetMaxNumAnimationSets(),
												ac->GetMaxNumTracks(),
												ac->GetMaxNumEvents(),
												&spAC);
	if (SUCCEEDED(hr))
	{
		for (UINT i=0, num=spAC->GetMaxNumTracks(); i<num; i++)
			spAC->SetTrackEnable(i, FALSE);
	}
}

//////////////////////////////////////////////////////////////////////////
iXModel::iXModel( iDevice* device, LPCOLESTR xurl/*=NULL*/, LPCOLESTR name/*=NULL*/ ) : iModelBase(device)
			, download(onDownloadComplete, this)
			, m_frameRoot(NULL)
			, m_meshRoot(NULL)
			, m_boneMatrices(NULL)
			, m_maxBones(0)
{
	load(xurl, name);
}

iXModel::~iXModel()
{
	unload();
}

bool iXModel::load( LPCOLESTR xurl, LPCOLESTR name/*=NULL*/ )
{
	if (xurl==NULL) return false;

	if (name) m_name = name;
	unload();

	if (::PathIsURLW(xurl))
		return SUCCEEDED(download.Download(xurl));

	CComPtr<IStream> spStream;
	HRESULT hr = CDownload::LoadUrlToStream(xurl, &spStream, NULL);
	if (SUCCEEDED(hr))
		onDownloadComplete(this, spStream.p);
	return isLoaded==TRUE;
}

void iXModel::unload()
{
	m_animController = NULL;
	if (m_frameRoot)
	{
		MeshAllocator allocator;
		m_frameRoot = (D3DXFrameDestroy(m_frameRoot, &allocator), NULL);
		m_meshRoot = NULL;

		if (m_boneMatrices) m_boneMatrices = (delete[] m_boneMatrices, NULL);
		m_maxBones = 0;
	}
	m_mesh = NULL;
	m_meshCache = NULL;
}

void iXModel::_create()
{
	if (m_meshCache.p==NULL && m_meshRoot==NULL && _stream.p)
	{
		HRESULT _hr = m_device->m_device->TestCooperativeLevel();
		if (_hr == D3DERR_DEVICENOTRESET) return;

		STATSTG ss;
		CComResult hr = _stream->Stat(&ss, STATFLAG_NONAME);
		ULONG cb = (ULONG)ss.cbSize.QuadPart;
		LPBYTE pBuf = NEW BYTE[cb];
		ULONG cbRead = 0;
		hr = _stream->Read(pBuf, cb, &cbRead);
		if (SUCCEEDED(hr))
		{
			// 首先尝试加载动画集
			MeshAllocator allocator;
			hr = D3DXLoadMeshHierarchyFromXInMemory(pBuf, cbRead, D3DXMESH_MANAGED, m_device->m_device.p, &allocator, NULL, (LPD3DXFRAME *)&m_frameRoot, &m_animController);
			if (hr == S_OK && m_animController.p)
			{
				isLoaded = TRUE;

				if (m_frameRoot)
				{
					if (m_frameRoot)
						m_meshRoot = (MeshContainerEx*)m_frameRoot->pMeshContainer;
					updateBonesPtr(m_frameRoot);

					if (m_maxBones>0)
					{
						m_boneMatrices = NEW D3DXMATRIX[m_maxBones];
						ZeroMemory(m_boneMatrices, sizeof(D3DXMATRIX)*m_maxBones);
					}
				}
				// 更新已有实体的动画实例
				if (m_animController.p)
				{
					iEntity* e = m_entities;
					while (e)
					{
						iAnimInstance* ai = NEW iAnimInstance(this, m_animController);
						if (ai)
						{
							if (ai->spAC.p == NULL) delete ai;
							else e->setAnim(ai);
						}
						e = e->TList<iEntity>::Next();
					}
				}
			}
			// 不是动画，则尝试加载普通MESH数据
			else
			{
				if (m_frameRoot)
				{
					MeshAllocator allocator;
					m_frameRoot = (D3DXFrameDestroy(m_frameRoot, &allocator), NULL);
					m_meshRoot = NULL;

					if (m_boneMatrices) m_boneMatrices = (delete[] m_boneMatrices, NULL);
					m_maxBones = 0;
				}

				isLoaded = TRUE;

				CComPtr<ID3DXBuffer> adjucencyBuf, mtrlBuf;
				if (SUCCEEDED(hr=D3DXLoadMeshFromXInMemory(pBuf, cbRead, D3DXMESH_MANAGED, m_device->m_device.p, &adjucencyBuf, &mtrlBuf, NULL, &m_matNum, &m_meshCache)) &&
					SUCCEEDED(hr=m_meshCache->OptimizeInplace(D3DXMESHOPT_COMPACT | D3DXMESHOPT_ATTRSORT | D3DXMESHOPT_VERTEXCACHE, (DWORD*)adjucencyBuf->GetBufferPointer(), NULL, NULL, NULL)))
				{
					// 设置材质，如果需要则创建纹理
					if (m_matNum > 0)
					{
						D3DXMATERIAL* pMtrl = (D3DXMATERIAL*)mtrlBuf->GetBufferPointer();
						m_materials = NEW iMaterial[m_matNum];
						if (m_materials)
						{
							for (DWORD i=0; i<m_matNum; i++)
							{
								m_materials[i].material = pMtrl[i].MatD3D;
								m_materials[i].texname = (pMtrl[i].pTextureFilename);
								m_materials[i].load(m_device->m_device);
							}
						}
					}

					// 设置要真正渲染的MESH
					createMesh();
					setFVF(m_mesh->GetFVF() | D3DFVF_NORMAL);
				}
			}
		}

		delete[] pBuf;
		_stream = NULL;
	}
}

void CALLBACK iXModel::onDownloadComplete( LPVOID pData, LPSTREAM pStream )
{
	iXModel* pThis = (iXModel*)pData;
	if (pThis==NULL || pStream==NULL) return;
	pThis->_stream = pStream;
	pThis->_create(); // 尝试加载一次，如果设备还未复位，则延迟加载
}

void iXModel::updateBonesPtr( FrameEx* frame )
{
	if (frame==NULL) return;

	MeshContainerEx* cont = (MeshContainerEx*)frame->pMeshContainer;
	if (cont)
	{
		cont->frame = frame;

		// 如果容器有蒙皮数据，则设置骨骼矩阵
		if (cont->pSkinInfo && cont->MeshData.pMesh)
		{
			// 为蒙皮复制一个网格
			D3DVERTEXELEMENT9 decl[MAX_FVF_DECL_SIZE];
			if (FAILED(cont->MeshData.pMesh->GetDeclaration(decl))) return;
			cont->MeshData.pMesh->CloneMesh(D3DXMESH_MANAGED, decl, m_device->m_device.p, &cont->exSkinMesh);

			// 统计最大骨骼数
			DWORD num = cont->pSkinInfo->GetNumBones();
			m_maxBones = max(m_maxBones, num);

			// 对每个蒙皮骨骼，更新帧中的矩阵指针缓存
			for (DWORD i=0; i<num; i++)
			{
				// 寻找骨骼对应的帧
				FrameEx* f = (FrameEx*)D3DXFrameFind(m_frameRoot, cont->pSkinInfo->GetBoneName(i));
				if (f)
					cont->exFrameCombinedMatrixPointer[i] = &f->exCombinedTransformationMatrix;
			}
		}
	}

	if (frame->pFrameFirstChild)
		updateBonesPtr((FrameEx*)frame->pFrameFirstChild);

	if (frame->pFrameSibling)
		updateBonesPtr((FrameEx*)frame->pFrameSibling);
}

iEntity* iXModel::createEntity()
{
	_create();
	iEntity* e = iModelBase::createEntity();
	if (e && isLoaded && m_animController.p)
	{
		iAnimInstance* ai = NEW iAnimInstance(this, m_animController);
		if (ai)
		{
			if (ai->spAC.p == NULL) delete ai;
			else e->setAnim(ai);
		}
	}
	return e;
}

void iXModel::createMesh()
{
	if (m_mesh.p==NULL && m_meshCache)
	{
		m_mesh = m_meshCache;
		_updateMeshInfo();
	}
}

void iXModel::getMaterials( CDispatchArray* pArr )
{
	if (m_animController)
	{
		updateGetMaterials(m_frameRoot, pArr);
	}
	else
		__super::getMaterials(pArr);
}

void iXModel::updateGetMaterials( FrameEx* frame, CDispatchArray* pArr )
{
	if (frame==NULL) return;

	MeshContainerEx* cont = (MeshContainerEx*)frame->pMeshContainer;
	while (cont)
	{
		for (DWORD i=0; i<cont->NumMaterials; i++)
			pArr->Add(cont->exMaterials[i].GetDispatch(), cont->exMaterials[i].texname);

		cont = (MeshContainerEx*)cont->pNextMeshContainer;
	}

	if (frame->pFrameFirstChild)
		updateGetMaterials((FrameEx*)frame->pFrameFirstChild, pArr);

	if (frame->pFrameSibling)
		updateGetMaterials((FrameEx*)frame->pFrameSibling, pArr);
}

void iXModel::clearCaches( FrameEx* frame )
{
	if (frame==NULL) return;

	MeshContainerEx* cont = (MeshContainerEx*)frame->pMeshContainer;
	while (cont)
	{
		for (DWORD i=0; i<cont->NumMaterials; i++)
			cont->exMaterials[i].on_reset();

		cont = (MeshContainerEx*)cont->pNextMeshContainer;
	}

	if (frame->pFrameFirstChild)
		clearCaches((FrameEx*)frame->pFrameFirstChild);

	if (frame->pFrameSibling)
		clearCaches((FrameEx*)frame->pFrameSibling);
}

void iXModel::on_reset()
{
	__super::on_reset();

	clearCaches (m_frameRoot);
}

void iXModel::pre_render()
{
	_create();
	__super::pre_render();
}
//////////////////////////////////////////////////////////////////////////
//iLight::iLight( )
//{
//}
//
//iLight::~iLight()
//{
//}

//////////////////////////////////////////////////////////////////////////
iLightGroup::iLightGroup()
		: m_ambientLight(false)
{
	ZeroMemory(m_lights, sizeof(m_lights));
}

iLightGroup::~iLightGroup()
{
}

//bool iLightGroup::setLight( ULONG index, Light* light )
//{
//	if (index >= 8) return false;
//	if (light==NULL)
//		m_lights[index].m_Enabled = false;
//	else
//	{
//		m_lights[index].m_Enabled = light->m_Enabled;
//		m_lights[index].m_Light = light->m_Light;
//	}
//	return true;
//}

HRESULT CALLBACK iLightGroup::OnUpdated( long index, LPCOLESTR name, CComVariant& value, LPVOID data )
{
	if (index<0 || index>=8 || data==NULL) return E_INVALIDARG;
	iLightGroup* pThis = (iLightGroup*)data;
	if (value.vt == VT_NULL)
		return (pThis->m_lights[index].m_Enabled = false), S_OK;
	else if (value.vt==VT_DISPATCH && value.pdispVal)
	{
		Light* light = (Light*)DISP_CAST(value.pdispVal, PointLight);
		if (light==NULL) light = (Light*)DISP_CAST(value.pdispVal, SpotLight);
		if (light==NULL) light = (Light*)DISP_CAST(value.pdispVal, DirectionLight);
		if (light)
		{
			pThis->m_lights[index].m_Enabled = light->m_Enabled;
			pThis->m_lights[index].m_Light = light->m_Light;
			return S_OK;
		}
	}
	return E_INVALIDARG;
}

//////////////////////////////////////////////////////////////////////////
iAmbientLight::iAmbientLight(bool enable)
{
	m_Enabled = enable;
	m_Color = 0xffffffff; //D3DXCOLOR(1.0f,1.0f,1.0f,0.0f);
}

//////////////////////////////////////////////////////////////////////////
void __init_light(D3DLIGHT9& light)
{
	ZeroMemory(&light, sizeof(light));
	light.Diffuse.a = light.Diffuse.r = light.Diffuse.g = light.Diffuse.b = 1.0f;
	//light.Ambient.a = light.Ambient.r = light.Ambient.g = light.Ambient.b = 1.0f;
	//light.Specular.a = light.Specular.r = light.Specular.g = light.Specular.b = 1.0f;
	light.Direction.x = 0; light.Direction.y = 0; light.Direction.z = 1.0f;
	light.Range = 10000.0f;
	light.Falloff = 1.0f;
	// 通常衰减值设置成 0,1,0，表示随距离线性衰减，计算公式为 Atten = 1/( att0 + att1 * d + att2 * d^2)
	light.Attenuation1 = 1.0f; light.Attenuation0 = light.Attenuation2 = 0;
	light.Phi = light.Theta = 0;
}

//////////////////////////////////////////////////////////////////////////
iPointLight::iPointLight()
{
	m_Enabled = false;
	__init_light(m_Light);
	m_Light.Type = D3DLIGHT_POINT;
}

//////////////////////////////////////////////////////////////////////////
iDirectionLight::iDirectionLight()
{
	m_Enabled = false;
	__init_light(m_Light);
	m_Light.Type = D3DLIGHT_DIRECTIONAL;
}

//////////////////////////////////////////////////////////////////////////
iSpotLight::iSpotLight()
{
	m_Enabled = false;
	__init_light(m_Light);
	m_Light.Type = D3DLIGHT_SPOT;
}

//////////////////////////////////////////////////////////////////////////
iMaterial::iMaterial() : download(onDownloadComplete, this), index(-1)
{
	m_Enabled = true;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = D3DXCOLOR(1.f, 1.f, 1.f, 0);
	effect = NULL;
}

iMaterial::~iMaterial()
{
	//ATLTRACE(L"material release(%d)\n", __disp.AddRef()-1);
}

#ifndef AutoValue
	#define AutoValue -20040620
#endif // AutoValue
void iMaterial::resetName(LPCOLESTR szName)
{
	if (texname==szName) return;
	if (lstrcmpiW(szName, L"auto")==0)
	{
		resetName(AutoValue);
		return;
	}

	index = -1;

	if (texname)
		texture = NULL;
	texname = szName;
	if (device)
		load(device);
}

void iMaterial::resetName( long lName )
{
	index = lName;
}

bool iMaterial::load(IDirect3DDevice9* dev)
{
	if (dev==NULL || texname.Length() == 0 || texture.p) return false;
	device = dev;

	if (::PathIsURLW(texname))
		return SUCCEEDED(download.Download(texname));

	CComPtr<IStream> spStream;
	HRESULT hr = CDownload::LoadUrlToStream(texname, &spStream, NULL);
	if (SUCCEEDED(hr))
		onDownloadComplete(this, spStream.p);
	return texture.p!=NULL;
}

void CALLBACK iMaterial::onDownloadComplete( LPVOID pData, LPSTREAM pStream )
{
	iMaterial* pThis = (iMaterial*)pData;
	if (pThis==NULL || pStream==NULL) return;

	STATSTG ss;
	HRESULT hr = pStream->Stat(&ss, STATFLAG_NONAME);
	ULONG cb = (ULONG)ss.cbSize.QuadPart;
	LPBYTE pBuf = NEW BYTE[cb];
	ULONG cbRead = 0;
	hr = pStream->Read(pBuf, cb, &cbRead);
	if (SUCCEEDED(hr))
	{
		D3DXIMAGE_INFO ImgInfo;
		hr = D3DXGetImageInfoFromFileInMemory(pBuf, cbRead, &ImgInfo);
		if (SUCCEEDED(hr))
		{
			pThis->texture = NULL;
			switch (ImgInfo.ResourceType)
			{
			case D3DRTYPE_TEXTURE:
				{
					CComPtr<IDirect3DTexture9> tex;
					hr = D3DXCreateTextureFromFileInMemory(pThis->device, pBuf, cbRead, &tex);
					if (SUCCEEDED(hr))
						pThis->texture = tex;
				}
				break;
			case D3DRTYPE_CUBETEXTURE:
				{
					CComPtr<IDirect3DCubeTexture9> tex;
					hr = D3DXCreateCubeTextureFromFileInMemory(pThis->device, pBuf, cbRead, &tex);
					if (SUCCEEDED(hr))
						pThis->texture = tex;
				}
				break;
			case D3DRTYPE_VOLUMETEXTURE:
				{
					CComPtr<IDirect3DVolumeTexture9> tex;
					hr = D3DXCreateVolumeTextureFromFileInMemory(pThis->device, pBuf, cbRead, &tex);
					if (SUCCEEDED(hr))
						pThis->texture = tex;
				}
				break;
			}
		}
	}

	delete[] pBuf;
}

void iMaterial::on_reset()
{
	effect_params.clear();
}

//////////////////////////////////////////////////////////////////////////
iEntity::iEntity( iModelBase* model, iScene* scene/*=NULL*/, iEntity* parent/*=NULL*/ ) : m_scene(scene), m_parent(parent), m_model(model)
		, TList<iEntity, cookie_child>(&iEntity::m_children)
		, TList<iEntity, cookie_sibling>(&iEntity::m_siblings)
		, m_anim(NULL), m_animations(NULL)
		, m_siblings(NULL), m_children(NULL)
{
	__disp.SetOwner(this); // 这句必须要有，否则无法删除内存

	ATLASSERT(m_model);
	D3DXMatrixIdentity(&m_mtWorld);

	m_Enabled = true;
	m_Material.m_Enabled = false;

	TList<iEntity>::addToList(&m_model->m_entities);
	GetDispatch()->Release(); // 在模型里直接 NEW 的，所以引用计数为 1，上面的 addToList 又增加了1次，所以必须释放一次
	//GetDispatch()->AddRef();

	if (parent)
	{
		m_scene = parent->m_scene;
		// 这里无需增加引用计数，肯定不是脚本创建的
		TList<iEntity,cookie_sibling>::addToList(&parent->m_children, false);
	}
	else if (m_scene)
	{
		TList<iEntity,cookie_sibling>::addToList(&m_scene->m_entities, false);
	}
}

iEntity::~iEntity()
{
	if (m_anim) m_anim = ((delete m_anim), NULL);
	if (m_animations) m_animations = ((delete m_animations), NULL);
	if (m_children) m_children->TList<iEntity, cookie_sibling>::removeAll();
}

void iEntity::setParent(iScene* scene)
{
	if (scene==NULL) return;
	bool useRefCount = true;
	if (m_scene && m_scene!=scene)
	{
		TList<iEntity,cookie_sibling>::removeFromList(false);
		useRefCount = false;
	}

	m_scene = scene;
	if (m_scene)
	{
		TList<iEntity,cookie_sibling>::addToList(&m_scene->m_entities, useRefCount);
	}
}

void iEntity::setParent(iEntity* parent)
{
	if (parent==NULL) return;
	m_scene = parent->m_scene;

	bool useRefCount = true;
	if (m_parent && m_parent!=parent)
	{
		TList<iEntity,cookie_sibling>::removeFromList(false);
		useRefCount = false;
	}

	m_parent = parent;
	if (m_parent)
	{
		TList<iEntity,cookie_sibling>::addToList(&m_parent->m_children, useRefCount);
	}
}

void iEntity::render()
{
	ATLASSERT(m_scene);

	//D3DXMATRIX mtWorld;
	//m_scene->m_device->m_device->GetTransform(D3DTS_WORLD, &mtWorld);
	if (m_Enabled && (!m_scene->get_current_mirror() || !m_node.HasAttribute(L"nomirror")))
	{
		{
			EnterRuntimeContext<iEntity> ctx_entity(m_scene->m_device, this);
			EnterRuntimeContext<iModelBase> ctx_model(m_scene->m_device, m_model);

			AutoStateDevice device(m_scene->m_device->m_device);
			// 模型的变换事件响应的 THIS 是实体指针。先调用模型回调，然后调用实体回调
			//VARIANT* cbTransform = &onTransform;
			//if (onTransform.vt!=VT_BSTR && onTransform.vt!=VT_DISPATCH)
			//	cbTransform = (m_anim ? &m_anim->model->onTransform : &m_model->onTransform);
			m_scene->m_device->fire_event(m_anim ? &m_anim->model->onTransform : &m_model->onTransform, GetDispatch());
			m_scene->m_device->fire_event(&onTransform, GetDispatch());

			device.SetTransform(D3DTS_WORLD, updateWorldMatrix());

			if (m_scene->m_hover_entity == this && (m_node.HasAttribute(L"wireframe") || m_model->m_node.HasAttribute(L"wireframe")))
				device.SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

			//m_scene->m_device->m_system_effect.set_entity_params();
			m_scene->m_device->fire_event(&onRender, GetDispatch());

			if (m_anim)
			{
				if (!m_scene->get_current_mirror() || !m_anim->model->m_node.HasAttribute(L"nomirror"))
				{
					frameMove(m_scene->m_device->elapsed_time());
					m_scene->m_device->fire_event(&m_anim->model->onRender, m_anim->model->GetDispatch());

					if (m_anim->model->m_frameRoot)
						drawFrame(m_anim->model->m_frameRoot);
				}
			}
			else if (m_model)
			{
				//m_scene->m_device->m_runtime.set_model(m_model);
				//m_scene->m_device->fire_event(&m_model->onRender, m_model->getDispatch());
				if (!m_scene->get_current_mirror() || !m_model->m_node.HasAttribute(L"nomirror"))
				{
					m_model->updateMatrices((CONST D3DXMATRIX*)updateWorldMatrix());
					m_model->render();
				}
			}
		}

		if (m_children) m_children->render();
	}

	if (m_siblings) m_siblings->render();
}

D3DXMATRIX* iEntity::updateWorldMatrix()
{
	// 在这里计算世界矩阵并保存到 m_mtWorld
	m_mtWorld = *m_WorldTransform.GetMatrix();
	if (m_parent)
		m_mtWorld *= *m_parent->updateWorldMatrix();

	return &m_mtWorld;
}

void iEntity::setAnim( iAnimInstance* ai )
{
	ATLASSERT(m_anim==NULL);
	m_anim = ai;
	if (m_anim)
	{
		m_animations = CDispatchArray::New();
		if (m_animations==NULL) return;
		m_animations->AddRef();

		UINT num = m_anim->spAC->GetNumAnimationSets();
		for (UINT i=0; i<num; i++)
		{
			CComPtr<ID3DXAnimationSet> as;
			m_anim->spAC->GetAnimationSet(i, &as);
			ATLASSERT(as);
			CStringW name (as->GetName());
			CComPtr<IDispatch> disp;
			AnimationSet* ias = NULL;
			if (SUCCEEDED(AnimationSet::CreateInstance(&disp, &ias)))
			{
				static_cast<iAnimationSet*>(ias)->m_as = as;
				m_animations->Add(disp, name);
			}
		}
	}
}

UINT iEntity::getAnimationSetCount()
{
	if (m_anim==NULL || m_anim->spAC.p==NULL) return 0;
	return m_anim->spAC->GetNumAnimationSets();
}

void iEntity::setAnimationSet( ID3DXAnimationSet* as )
{
	if (m_anim==NULL || m_anim->spAC.p==NULL || as==m_anim->spAS.p) return;

	m_anim->spAS = as;
	if (m_anim->spAS==NULL) return; // 设置NULL时停止播放动画

	// 如果需要一个平滑的动画切换，我们应该用两个TRACK，新的TRACK设置成新的动画，旧的动画使用旧的TRACK
	// 两个TRACK能被同时处理，这样在动画切换时不至于有明显的跳跃感

	DWORD newTrack = (m_anim->curTrack==0 ? 1 : 0);
	CComResult hr = m_anim->spAC->SetTrackAnimationSet(newTrack, as);
	as = NULL;

	// 清理两个TRACK的所有当前事件
	CheckHResult hr = m_anim->spAC->UnkeyAllTrackEvents(m_anim->curTrack);
	CheckHResult hr = m_anim->spAC->UnkeyAllTrackEvents(newTrack);

	// 禁用旧动画
	// 在一定时间后终止旧动画
	CheckHResult hr = m_anim->spAC->KeyTrackEnable(m_anim->curTrack, FALSE, m_anim->curTime + m_anim->moveTransitionTime);
	// 从当前时间开始，在 m_moveTransitionTime 秒内完成旧动画
	CheckHResult hr = m_anim->spAC->KeyTrackSpeed(m_anim->curTrack, 0.0f, m_anim->curTime, m_anim->moveTransitionTime, D3DXTRANSITION_LINEAR);
	// 设置旧动画权重
	CheckHResult hr = m_anim->spAC->KeyTrackWeight(m_anim->curTrack, 0.0f, m_anim->curTime, m_anim->moveTransitionTime, D3DXTRANSITION_LINEAR);

	// 开启新动画TRACK
	CheckHResult hr = m_anim->spAC->SetTrackEnable(newTrack, TRUE);
	// 设置新动画速度
	CheckHResult hr = m_anim->spAC->KeyTrackSpeed(newTrack, 1.0f, m_anim->curTime, m_anim->moveTransitionTime, D3DXTRANSITION_LINEAR);
	// 设置新动画权重
	CheckHResult hr = m_anim->spAC->KeyTrackWeight(newTrack, 1.0f, m_anim->curTime, m_anim->moveTransitionTime, D3DXTRANSITION_LINEAR);

	m_anim->curTrack = newTrack;
}

void iEntity::setAnimationSet( DWORD index )
{
	if (m_anim==NULL || m_anim->spAC.p==NULL) return;

	if (index >= m_anim->spAC->GetNumAnimationSets()) index = 0;
	CComPtr<ID3DXAnimationSet> as;
	HRESULT hr = m_anim->spAC->GetAnimationSet(index, &as);
	setAnimationSet(as);
}

bool iEntity::findAnimationSet( LPCOLESTR name, ID3DXAnimationSet** ppAnimationSet/*=NULL*/ )
{
	if (m_anim==NULL || m_anim->spAC.p==NULL || name==NULL || *name==0) return false;

	CComPtr<ID3DXAnimationSet> as;
	HRESULT hr = m_anim->spAC->GetAnimationSetByName(CStringA(name), &as);
	if (FAILED(hr)) return false;

	if (ppAnimationSet)
		*ppAnimationSet = as.Detach();
	return true;
}

void iEntity::frameMove( float elapsedTime )
{
	if (m_anim==NULL)
	{
		// 非动画在这里控制
		return;
	}

	elapsedTime /= m_anim->speedAdjust;

	CComResult hr;
	if (m_anim->spAC && m_anim->spAS)
	{
		CheckHResult hr = m_anim->spAC->AdvanceTime(elapsedTime, NULL);
		m_anim->curTime += elapsedTime;

		updateFrameMatrices(m_anim->model->m_frameRoot, &m_mtWorld);

		if (m_anim->model->m_frameRoot && m_anim->model->m_meshRoot->pSkinInfo)
		{
			DWORD numBones = m_anim->model->m_meshRoot->pSkinInfo->GetNumBones();

			// 更新骨骼矩阵数组
			for (DWORD i=0; i<numBones; i++)
				D3DXMatrixMultiply(&m_anim->model->m_boneMatrices[i], &m_anim->model->m_meshRoot->exBoneOffsets[i], m_anim->model->m_meshRoot->exFrameCombinedMatrixPointer[i]);

			// 从原始网格中复制顶点数据到蒙皮网格
			if (m_anim->model->m_meshRoot->exSkinMesh)
			{
				LPVOID src=NULL, dst=NULL;
				CheckHResult hr = m_anim->model->m_meshRoot->MeshData.pMesh->LockVertexBuffer(D3DLOCK_READONLY, &src);
				CheckHResult hr = m_anim->model->m_meshRoot->exSkinMesh->LockVertexBuffer(0, &dst);

				CheckHResult hr = m_anim->model->m_meshRoot->pSkinInfo->UpdateSkinnedMesh(m_anim->model->m_boneMatrices, NULL, src, dst);

				CheckHResult hr = m_anim->model->m_meshRoot->exSkinMesh->UnlockVertexBuffer();
				CheckHResult hr = m_anim->model->m_meshRoot->MeshData.pMesh->UnlockVertexBuffer();
			}
		}
	}
}

void iEntity::drawMeshContainer( iXModel::MeshContainerEx* cont )
{
	if (cont==NULL) return;
	ATLASSERT(cont->frame);

	AutoStateDevice device(m_scene->m_device->m_device);
	device->SetTransform(D3DTS_WORLD, &cont->frame->exCombinedTransformationMatrix); // 用 -> 表示不用恢复状态
	m_scene->m_device->m_system_effect.set_model_params(&cont->frame->exCombinedTransformationMatrix);

	LPD3DXMESH pMesh = (cont->pSkinInfo ? cont->exSkinMesh : cont->MeshData.pMesh);
	for (DWORD i=0; i<cont->NumMaterials; i++)
	{
		//device.SetMaterial(&cont->exMaterials[i].material);
		//device.SetTexture(0, cont->exMaterials[i].texture.p ? cont->exMaterials[i].texture.p : m_scene->find_texture(cont->exMaterials[i].texname));

		iMaterial* m = &cont->exMaterials[i];

		{
			EnterRuntimeContext<iMaterial> ctx_material(m_scene->m_device, m);
			EnterRuntimeContext<IDirect3DBaseTexture9> ctx_texture(m_scene->m_device, m->texture.p ? m->texture.p : m_scene->find_texture(m));
			m_scene->m_device->fire_event(&m->onRender, m->GetDispatch());

			if (m->effect==NULL)
			{
				if (m->technique)
					m->effect = iEffect::findTechnique(m_scene->m_device->m_effects, (m->technique));
				if (m->effect==NULL)
				{
					m->effect = &m_scene->m_device->m_system_effect;
					m->technique.Empty(); // L"SystemDefault";
				}
			}
			if (m->effect == &m_scene->m_device->m_system_effect)
				m_scene->m_device->m_system_effect.set_texture_params();

			UINT numPass = m->effect->begin(m->technique ? m->technique : m_scene->get_technique());

			// 效果有效，此时调用三个对象的 onPrepareEffect 事件，用来设置个性化效果参数
			m_scene->m_device->fire_event(&m->onPrepareEffect, m->GetDispatch());
			m_scene->m_device->fire_event(&m_scene->m_device->m_runtime.model->onPrepareEffect, m_scene->m_device->m_runtime.model->getDispatch());
			m_scene->m_device->fire_event(&m_scene->m_device->m_runtime.entity->onPrepareEffect, m_scene->m_device->m_runtime.entity->GetDispatch());

			for (UINT ip=0; ip<numPass; ip++)
			{
				m->effect->beginPass(ip);
				pMesh->DrawSubset(i);
				m->effect->endPass();
			}
			m->effect->end();
		}
	}
}

void iEntity::drawFrame( iXModel::FrameEx* frame/*=NULL*/ )
{
	if (frame==NULL && m_anim) frame = m_anim->model->m_frameRoot;
	if (frame==NULL) return;

	iXModel::MeshContainerEx* cont = (iXModel::MeshContainerEx*)frame->pMeshContainer;
	while (cont)
	{
		drawMeshContainer(cont);
		cont = (iXModel::MeshContainerEx*)cont->pNextMeshContainer;
	}

	if (frame->pFrameFirstChild)
		drawFrame((iXModel::FrameEx*)frame->pFrameFirstChild);

	if (frame->pFrameSibling)
		drawFrame((iXModel::FrameEx*)frame->pFrameSibling);
}

void iEntity::updateFrameMatrices( iXModel::FrameEx* frame, const D3DXMATRIX* mtParent )
{
	if (frame==NULL) return;

	if (mtParent)
		D3DXMatrixMultiply(&frame->exCombinedTransformationMatrix, &frame->TransformationMatrix, mtParent);
	else
		frame->exCombinedTransformationMatrix = frame->TransformationMatrix;

	if (frame->pFrameFirstChild)
		updateFrameMatrices((iXModel::FrameEx*)frame->pFrameFirstChild, &frame->exCombinedTransformationMatrix);

	if (frame->pFrameSibling)
		updateFrameMatrices((iXModel::FrameEx*)frame->pFrameSibling, mtParent);
}

void iEntity::resetTime()
{
	if (m_anim)
	{
		if (m_anim->spAC.p)
			m_anim->spAC->ResetTime();
		m_anim->curTime = 0;
	}
}

void iEntity::on_reset()
{
	effect_params.clear();
	m_Material.effect_params.clear();
	if (m_model) m_model->on_reset();
	if (m_anim && m_anim->model) m_anim->model->on_reset();

	if (m_children) m_children->on_reset();
	if (m_siblings) m_siblings->on_reset();
}

void iEntity::unload()
{
	onTransform.Clear();
	onRender.Clear();
	onPrepareEffect.Clear();
	onMouseEnter.Clear();
	onMouseLeave.Clear();
	onClick.Clear();
	onDblClick.Clear();

	if (m_children) m_children->unload();
	if (m_siblings) m_siblings->unload();
}

void iEntity::createEntityByNode( HANDLE node, iScene* scene/*=NULL*/, iEntity* parent/*=NULL*/ )
{
	ATLASSERT(node && (scene || parent));
	iDevice* device = (parent ? parent->m_scene->m_device : scene->m_device);
	CMarkupNode* n = (CMarkupNode*)node;

	LPCOLESTR tag = n->GetName();
	iModelBase* m = NULL;
	bool isEntity = true;
	if (lstrcmpiW(tag, L"Entity")==0)
	{
		m = device->find_model(n->GetAttributeValue(L"model"));
		if (m==NULL)
		{
			iModelBase* parent = NULL;
			IDispatch** ppDisp = NULL;
			BuildModelResource(NullModel);
		}
	}
	else if (lstrcmpiW(tag, L"Model")==0)
	{
		isEntity = false;
		CComPtr<IDispatch> disp;
		iModelBase::createModelByNode(n, device, NULL, &disp);
		if (disp.p)
		{
			m = iModelBase::fromObject(disp.p);
			//if (m)
			//{
			//	iEntity* e = m->createEntity();
			//	if (e==NULL) return TRUE;
			//	e->setParent(pThis);
			//	e->m_Enabled = true;
			//	e->m_Name = m->m_name;
			//	e->m_WorldTransform.ParseAttribute(hChild);

			//	// TODO:材质必须作为子标签，而且必须指定name属性，用以取代模型中的同名材质
			//	e->m_Material.ParseAttribute(hChild);
			//}
		}
	}
	else return;

	if (m==NULL) return;
	iEntity* e = m->createEntity();
	if (e==NULL) return;
	if (parent) e->setParent(parent);
	else e->setParent(scene);

	e->m_Enabled = !!ParseBoolString(n->GetAttributeValue(L"enabled"), TRUE);
	e->m_hit.isBox = ParseBoolString(n->GetAttributeValue(L"BoxBound", L"BoundBox", L"Box"));
	if (!e->m_hit.isBox)
		e->m_hit.isBox = ParseBoolString(m->m_node.GetAttributeValue(L"BoxBound", L"BoundBox", L"Box"));

	if (!e->m_hit.isBox)
	{
		e->m_hit.isSphere = ParseBoolString(n->GetAttributeValue(L"SphereBound", L"BoundSphere", L"Sphere"));
		if (!e->m_hit.isSphere)
			e->m_hit.isSphere = ParseBoolString(m->m_node.GetAttributeValue(L"SphereBound", L"BoundSphere", L"Sphere"));
	}

	//// TODO:材质必须作为子标签，而且必须指定name属性，用以取代模型中的同名材质
	//e->m_Material.ParseAttribute(hChild);

	// 部分属性只能也在显式实体节点属性里，隐式实体节点包括的变换属性仅属于模型，不属于实体
	if (isEntity)
	{
		e->m_node = *n;
		e->m_Name = n->GetAttributeValue(L"name", L"id");
		e->m_WorldTransform.ParseAttribute((HANDLE)n);
		e->m_Material.ParseAttribute((HANDLE)n);

		e->effect_params.set_params_string(n->GetAttributeValue(L"EffectParameters", L"Parameters"));

		e->onTransform = n->GetAttributeValue(L"onTransform");
		e->onRender = n->GetAttributeValue(L"onRender", L"Render");
		e->onPrepareEffect = n->GetAttributeValue(L"onPrepareEffect", L"PrepareEffect");
		e->onMouseEnter = n->GetAttributeValue(L"onMouseEnter", L"onMouseOver", L"MouseEnter", L"MouseOver");
		e->onMouseLeave = n->GetAttributeValue(L"onMouseLeave", L"onMouseOut", L"MouseLeave", L"MouseOut");
		e->onClick = n->GetAttributeValue(L"onClick", L"Click");
		e->onDblClick = n->GetAttributeValue(L"onDblClick", L"DblClick");

		// 处理子节点，可能包括材质、子模型、子实体
		for (CMarkupNode child=n->GetChild(); child.IsValid(); child=child.GetSibling())
		{
			LPCOLESTR ctag = child.GetName();
			if (lstrcmpiW(ctag, L"Entity")==0 || lstrcmpiW(ctag, L"Model")==0)
			{
				createEntityByNode(&child, NULL, e);
			}
			else if (lstrcmpiW(ctag, L"Material")==0)
			{
				e->m_Material.ParseAttribute((HANDLE)&child);
				//// 仅当有名称属性时才创建一个映射的材质
				//if (child.GetAttributeValue(L"name", L"id") && m->m_mapped_materials.Add(iMaterial()))
				//{
				//	iMaterial& mat = m->m_mapped_materials[m->m_mapped_materials.GetSize()-1];
				//	mat.ParseAttribute((HANDLE)&child);
				//}
			}
			else if (lstrcmpiW(ctag, L"Transform")==0)
			{
				e->m_WorldTransform.ParseAttribute((HANDLE)&child);
			}
		}
	}
	//else
	//{
	//	// 如果采用隐式实体节点，则实体名称与模型名称相同。隐式实体不能创建变换、材质以及子实体
	//	e->m_Name = m->m_name;
	//}
}

BOOL iEntity::_enumEntity( iEntity* pStart, FINDENTITYPROC proc, LPVOID data, bool meFirst/*=true*/ )
{
	if (meFirst && !proc(pStart, data)) return FALSE;
	if (pStart->m_children && !_enumEntity(pStart->m_children, proc, data)) return FALSE;
	if (pStart->m_siblings && !_enumEntity(pStart->m_siblings, proc, data)) return FALSE;
	if (!meFirst && !proc(pStart, data)) return FALSE;
	return TRUE;
}

BOOL iEntity::_enumParent(iEntity* pStart, FINDENTITYPROC proc, LPVOID data, bool meFirst/*=true*/)
{
	if (meFirst && !proc(pStart, data)) return FALSE;
	if (pStart->m_parent && !_enumParent(pStart->m_parent, proc, data)) return FALSE;
	if (!meFirst && !proc(pStart, data)) return FALSE;
	return TRUE;
}

void iEntity::enumAllEntities( FINDENTITYPROC proc, LPVOID data, bool meFirst/*=true*/ )
{
	if (proc==NULL) return;
	_enumEntity(this, proc, data, meFirst);
}

void iEntity::enumAllParents(FINDENTITYPROC proc, LPVOID data, bool meFirst/* =true */)
{
	if (proc==NULL) return;
	_enumParent(this, proc, data, meFirst);
}

void iEntity::updateIntersectInfo(iEntity*& eDst/*, iEntity* parent*//*=NULL*/)
{
	if (m_Enabled)
	{
		EnterRuntimeContext<iEntity> ctx(m_scene->m_device, this);

		//AutoStateDevice device(m_scene->m_device->m_device);
		//device.SetTransform(D3DTS_WORLD, (CONST D3DXMATRIX*)updateWorldMatrix());

		//iModelBase* m = NULL;
		ID3DXBaseMesh* pMesh = NULL;
		D3DXMATRIX* pWorld = NULL;
		iModelBase* m = (m_anim ? m_anim->model : m_model);
		if (m)
		{
			EnterRuntimeContext<iModelBase> ctx1(m_scene->m_device, m);

			if (m_anim)
			{
				updateFrameMatrices(m_anim->model->m_frameRoot, (CONST D3DXMATRIX*)updateWorldMatrix());
				iXModel::MeshContainerEx* cont = (iXModel::MeshContainerEx*)m_anim->model->m_frameRoot->pMeshContainer;
				pMesh = cont->pSkinInfo ? cont->exSkinMesh : cont->MeshData.pMesh;
				pWorld = &m_anim->model->m_frameRoot->exCombinedTransformationMatrix;
			}
			else
			{
				pMesh = m_model->m_mesh.p;
				m_model->updateMatrices((CONST D3DXMATRIX*)updateWorldMatrix());
				pWorld = &m_model->m_world;
			}

			if (pMesh)
			{
				D3DXVECTOR3 vPos, vDir;
				vPos = m_scene->point_to_ray(m_scene->m_ptMouse, &vDir, pWorld);
				::D3DXIntersect(pMesh, &vPos, &vDir, &m_hit.hit, &m_hit.face, &m_hit.u, &m_hit.v, &m_hit.dist, NULL, NULL);
				if (!m_hit.hit)
				{
					if (m_hit.isBox && ::D3DXBoxBoundProbe(&m->m_meshInfo.boundMin, &m->m_meshInfo.boundMax, &vPos, &vDir))
					{
						m_hit.hit = 2;
					}
					else if (m_hit.isSphere && ::D3DXSphereBoundProbe(&m->m_meshInfo.sphereCenter, m->m_meshInfo.sphereRadius, &vPos, &vDir))
					{
						m_hit.hit = 3;
					}
				}

				if (m_hit.hit==TRUE && (eDst==NULL || m_hit.dist <= eDst->m_hit.dist))
				{
					eDst = this;
				}
			}
		}

		if (m_children) m_children->updateIntersectInfo(eDst/*, this*/);
	}

	if (m_siblings) m_siblings->updateIntersectInfo(eDst/*, parent*/);
}

bool iEntity::_fireMouseEnter(iEntity* eStop)
{
	if (m_parent && m_parent!=eStop)
	{
		if (!m_parent->_fireMouseEnter(eStop))
			return false;
	}

	CComVariant vRet;
	m_scene->m_device->fire_event(&onMouseEnter, GetDispatch(), &vRet);
	if (vRet.vt==VT_BOOL && vRet.boolVal==VARIANT_FALSE) return false;

	// do model fire
	iModelBase* m = (m_anim ? m_anim->model : m_model);
	if (m)
	{
		vRet.Clear();
		m_scene->m_device->fire_event(&m->onMouseEnter, m->getDispatch(), &vRet);
		if (vRet.vt==VT_BOOL && vRet.boolVal==VARIANT_FALSE) return false;
	}
	return true;
}

bool iEntity::_fireMouseLeave(iEntity* eStop)
{
	CComVariant vRet;
	m_scene->m_device->fire_event(&onMouseLeave, GetDispatch(), &vRet);
	if (vRet.vt==VT_BOOL && vRet.boolVal==VARIANT_FALSE) return false;

	// do model fire
	iModelBase* m = (m_anim ? m_anim->model : m_model);
	if (m)
	{
		vRet.Clear();
		m_scene->m_device->fire_event(&m->onMouseLeave, m->getDispatch(), &vRet);
		if (vRet.vt==VT_BOOL && vRet.boolVal==VARIANT_FALSE) return false;
	}

	if (m_parent && m_parent!=eStop)
	{
		if (!m_parent->_fireMouseLeave(eStop))
			return false;
	}
	return true;
}

bool iEntity::fireMouseEnter( iEntity* eOld/*=NULL*/ )
{
	iEntity* parent = getCoParent(this, eOld);
	return _fireMouseEnter(parent);
}

bool iEntity::fireMouseLeave( iEntity* eNew/*=NULL*/ )
{
	iEntity* parent = getCoParent(this, eNew);
	return _fireMouseLeave(parent);
}

bool iEntity::fireClick()
{
	{
		EnterRuntimeContext<iEntity> ctx(m_scene->m_device, this);

		CComVariant vRet;
		m_scene->m_device->fire_event(&onClick, GetDispatch(), &vRet);
		if (vRet.vt==VT_BOOL && vRet.boolVal==VARIANT_FALSE) return false;

		// do model click
		iModelBase* m = (m_anim ? m_anim->model : m_model);
		if (m)
		{
			EnterRuntimeContext<iModelBase> ctx1(m_scene->m_device, m);

			vRet.Clear();
			m_scene->m_device->fire_event(&m->onClick, m->getDispatch(), &vRet);
			if (vRet.vt==VT_BOOL && vRet.boolVal==VARIANT_FALSE) return false;
		}
	}

	// bubble
	if (m_parent) return m_parent->fireClick();

	return true;
}

bool iEntity::fireDblClick()
{
	{
		EnterRuntimeContext<iEntity> ctx(m_scene->m_device, this);

		CComVariant vRet;
		m_scene->m_device->fire_event(&onDblClick, GetDispatch(), &vRet);
		if (vRet.vt==VT_BOOL && vRet.boolVal==VARIANT_FALSE) return false;

		// do model click
		iModelBase* m = (m_anim ? m_anim->model : m_model);
		if (m)
		{
			EnterRuntimeContext<iModelBase> ctx1(m_scene->m_device, m);

			vRet.Clear();
			m_scene->m_device->fire_event(&m->onDblClick, m->getDispatch(), &vRet);
			if (vRet.vt==VT_BOOL && vRet.boolVal==VARIANT_FALSE) return false;
		}
	}

	// bubble
	if (m_parent) return m_parent->fireDblClick();

	return true;
}

iEntity* iEntity::getCoParent( iEntity* e1, iEntity* e2 )
{
	if (e1==NULL || e2==NULL) return NULL;

	class MouseSelector2
	{
	public:
		iEntity* eRef;
		bool bSucc;
		static BOOL Proc(iEntity* entity, LPVOID data)
		{
			MouseSelector2* pThis = (MouseSelector2*)data;
			if (pThis->eRef == entity)
			{
				pThis->bSucc = true;
				return FALSE;
			}
			return TRUE;
		}
	};
	class MouseSelector
	{
	public:
		iEntity* other, *eResult;
		static BOOL Proc(iEntity* entity, LPVOID data)
		{
			MouseSelector* pThis = (MouseSelector*)data;
			MouseSelector2 selector2;
			selector2.eRef = entity;
			selector2.bSucc = false;
			pThis->other->enumAllParents(&MouseSelector2::Proc, &selector2, true);
			if (selector2.bSucc)
			{
				pThis->eResult = entity;
				return FALSE;
			}
			return TRUE;
		}
	};
	MouseSelector selector;
	selector.other = e2;
	selector.eResult = NULL;
	e1->enumAllParents(&MouseSelector::Proc, &selector, true);
	return selector.eResult;
}
//////////////////////////////////////////////////////////////////////////
iModel::iModel( iDevice* device/*=NULL*/ ) : iModelBase(device)
{

}

iModel::~iModel()
{

}

void iModel::createMesh()
{
	ATLASSERT(m_mesh.p == NULL);
	if (m_Points.GetSize()<=0 || m_TriangleIndices.GetSize()<=0) return;

	CComResult hr = D3DXCreateMeshFVF((DWORD)(m_TriangleIndices.GetSize()/3),
								(DWORD)m_Points.GetSize(),
								D3DXMESH_MANAGED /*| D3DXMESH_WRITEONLY*/ | D3DXMESH_32BIT,
								m_defFVF,
								m_device->m_device, &m_mesh);
	if (SUCCEEDED(hr))
	{
		_updateMeshInfo();

		CComPtr<IDirect3DIndexBuffer9> ib;
		CComPtr<IDirect3DVertexBuffer9> vb;
		m_mesh->GetIndexBuffer(&ib);
		m_mesh->GetVertexBuffer(&vb);
		ModelVertex* v = NULL;
		if (SUCCEEDED(vb->Lock(0, 0, (void**)&v, 0)))
		{
			ZeroMemory(v, sizeof(ModelVertex) * m_Points.GetSize());
			for (int i=0; i<m_Points.GetSize(); i++)
			{
				v[i].position = m_Points[i];
				if (m_Normals.GetSize()>i)
					v[i].normal = m_Normals[i];
				if (m_TextureCoordinates.GetSize()>i)
				{
					v[i].u = m_TextureCoordinates[i].x;
					v[i].v = m_TextureCoordinates[i].y;
				}
			}

			vb->Unlock();

			if (m_Normals.GetSize()<m_Points.GetSize())
				hr = D3DXComputeNormals(m_mesh, NULL);
		}

		DWORD* iib = NULL;
		if (SUCCEEDED(ib->Lock(0, 0, (void**)&iib, 0)))
		{
			ZeroMemory(iib, sizeof(DWORD) * m_TriangleIndices.GetSize());
			for (int i=0; i<m_TriangleIndices.GetSize(); i++)
				iib[i] = (DWORD)m_TriangleIndices[i];
			ib->Unlock();
		}

		//// 默认创建一个空材质
		//m_matNum = 1;
		//m_materials = new iMaterial[m_matNum];
	}
}

//////////////////////////////////////////////////////////////////////////
iQuadModel::iQuadModel( iDevice* device/*=NULL*/ ) : iModelBase(device)
{
	Width = Height = 2.0f;
}

iQuadModel::~iQuadModel()
{

}

#if 0
	#define MapTexelOffset 0.5f
#else
	#define MapTexelOffset 0
#endif // 1

void iQuadModel::createMesh()
{
	ATLASSERT(m_mesh.p == NULL);

	CComResult hr = D3DXCreateMeshFVF(2, 4,
		D3DXMESH_MANAGED /*| D3DXMESH_WRITEONLY*/ | D3DXMESH_32BIT,
		iModel::m_defFVF,
		m_device->m_device, &m_mesh);
	if (SUCCEEDED(hr))
	{
		_updateMeshInfo();

		CComPtr<IDirect3DIndexBuffer9> ib;
		CComPtr<IDirect3DVertexBuffer9> vb;
		m_mesh->GetIndexBuffer(&ib);
		m_mesh->GetVertexBuffer(&vb);
		iModel::ModelVertex* v = NULL;
		if (SUCCEEDED(vb->Lock(0, 0, (void**)&v, 0)))
		{
			ZeroMemory(v, sizeof(iModel::ModelVertex) * 4);
			v[0].position = D3DXVECTOR3(-Width/2 - MapTexelOffset, Height/2 - MapTexelOffset, 0);
			v[0].u = 0; v[0].v = 0;

			v[1].position = D3DXVECTOR3(Width/2 - MapTexelOffset, Height/2 - MapTexelOffset, 0);
			v[1].u = 1.0f; v[1].v = 0;

			v[2].position = D3DXVECTOR3(-Width/2 - MapTexelOffset, -Height/2 - MapTexelOffset, 0);
			v[2].u = 0; v[2].v = 1.0f;

			v[3].position = D3DXVECTOR3(Width/2 - MapTexelOffset, -Height/2 - MapTexelOffset, 0);
			v[3].u = 1.0f; v[3].v = 1.0f;

			vb->Unlock();

			hr = D3DXComputeNormals(m_mesh, NULL);
		}

		DWORD* iib = NULL;
		if (SUCCEEDED(ib->Lock(0, 0, (void**)&iib, 0)))
		{
			ZeroMemory(iib, sizeof(DWORD) * 6);
			iib[0] = 0; iib[1] = 1; iib[2] = 2;
			iib[3] = 2; iib[4] = 1; iib[5] = 3;
			ib->Unlock();
		}
	}
}

//////////////////////////////////////////////////////////////////////////
iBoxModel::iBoxModel( iDevice* device/*=NULL*/ ) : iModelBase(device)
{
	Width = Height = Depth = 100.0f;
}

iBoxModel::~iBoxModel()
{

}

void iBoxModel::createMesh()
{
	ATLASSERT(m_mesh.p == NULL);
	CComPtr<ID3DXBuffer> adjacency;
	HRESULT hr = D3DXCreateBox(m_device->m_device, Width, Height, Depth, &m_mesh, &adjacency);
	if (SUCCEEDED(hr))
		_updateMeshInfo();
}

//////////////////////////////////////////////////////////////////////////
iCylinderModel::iCylinderModel( iDevice* device/*=NULL*/ ) : iModelBase(device)
{
	Radius1 = 20.0f;
	Radius2 = 10.0f;
	Length = 50.0f;
	Slices = 5;
	Stacks = 5;
}

iCylinderModel::~iCylinderModel()
{

}

void iCylinderModel::createMesh()
{
	ATLASSERT(m_mesh.p == NULL);
	CComPtr<ID3DXBuffer> adjacency;
	HRESULT hr = D3DXCreateCylinder(m_device->m_device, Radius1, Radius2, Length, Slices, Stacks, &m_mesh, &adjacency);
	if (SUCCEEDED(hr))
		_updateMeshInfo();
}

//////////////////////////////////////////////////////////////////////////
iPolygonModel::iPolygonModel( iDevice* device/*=NULL*/ ) : iModelBase(device)
{
	Length = 50.f;
	Sides = 5;
}

iPolygonModel::~iPolygonModel()
{

}

void iPolygonModel::createMesh()
{
	ATLASSERT(m_mesh.p == NULL);
	CComPtr<ID3DXBuffer> adjacency;
	HRESULT hr = D3DXCreatePolygon(m_device->m_device, Length, Sides, &m_mesh, &adjacency);
	if (SUCCEEDED(hr))
		_updateMeshInfo();
}
//////////////////////////////////////////////////////////////////////////
iSphereModel::iSphereModel( iDevice* device/*=NULL*/ ) : iModelBase(device)
{
	Radius = 50.f;
	Slices = 5;
	Stacks = 5;
}

iSphereModel::~iSphereModel()
{

}

void iSphereModel::createMesh()
{
	ATLASSERT(m_mesh.p == NULL);
	CComPtr<ID3DXBuffer> adjacency;
	HRESULT hr = D3DXCreateSphere(m_device->m_device, Radius, Slices, Stacks, &m_mesh, &adjacency);
	if (SUCCEEDED(hr))
		_updateMeshInfo();
}
//////////////////////////////////////////////////////////////////////////
iTorusModel::iTorusModel( iDevice* device/*=NULL*/ ) : iModelBase(device)
{
	InnerRadius = 30.f;
	OuterRadius = 50.f;
	Sides = 5;
	Rings = 5;
}

iTorusModel::~iTorusModel()
{

}

void iTorusModel::createMesh()
{
	ATLASSERT(m_mesh.p == NULL);
	CComPtr<ID3DXBuffer> adjacency;
	HRESULT hr = D3DXCreateTorus(m_device->m_device, InnerRadius, OuterRadius, Sides, Rings, &m_mesh, &adjacency);
	if (SUCCEEDED(hr))
		_updateMeshInfo();
}
//////////////////////////////////////////////////////////////////////////
iTextModel::iTextModel( iDevice* device/*=NULL*/ ) : iModelBase(device)
{
	Text = L"雨寒工作室\nYuhan Studio\n(www.huyuhan.net)";
	Deviation = 0;
	Extrusion = 0.1f;
}

iTextModel::~iTextModel()
{

}

void iTextModel::createMesh()
{
	ATLASSERT(m_mesh.p == NULL);
	CComPtr<ID3DXBuffer> adjacency;
	CClientDC dc(m_device->m_hwnd);
	HFONT hfont = ::CreateFontIndirectW(&Font);
	HFONT holdfont = dc.SelectFont(hfont);
	HRESULT hr = D3DXCreateTextW(m_device->m_device, dc.m_hDC, Text, Deviation, Extrusion, &m_mesh, &adjacency, NULL);
	dc.SelectFont(holdfont);
	::DeleteObject(hfont);

	if (SUCCEEDED(hr))
	{
		// 重新调整，让原点为文本的中心
		if (Center)
		{
			DWORD numVertices = m_mesh->GetNumVertices();
			DWORD bytesPerVertex = m_mesh->GetNumBytesPerVertex();
			CComPtr<IDirect3DVertexBuffer9> vb;
			m_mesh->GetVertexBuffer(&vb);
			LPBYTE p = NULL;
			if (vb.p && SUCCEEDED(vb->Lock(0, 0, (void**)&p, D3DLOCK_DISCARD)))
			{
				D3DXVECTOR3 boundMin, boundMax, boundCenter;
				D3DXComputeBoundingBox((CONST D3DXVECTOR3 *)p, numVertices, bytesPerVertex, &boundMin, &boundMax);
				boundCenter = (boundMin + boundMax) / 2;
				for (DWORD i=0; i<numVertices; i++)
				{
					D3DXVECTOR3* pos = (D3DXVECTOR3*)&p[i * bytesPerVertex];
					*pos -= boundCenter;
				}
				vb->Unlock();
			}
		}

		_updateMeshInfo();
	}
}
//////////////////////////////////////////////////////////////////////////
iTeapotModel::iTeapotModel( iDevice* device/*=NULL*/ ) : iModelBase(device)
{

}

iTeapotModel::~iTeapotModel()
{

}

void iTeapotModel::createMesh()
{
	ATLASSERT(m_mesh.p == NULL);
	CComPtr<ID3DXBuffer> adjacency;
	HRESULT hr = D3DXCreateTeapot(m_device->m_device, &m_mesh, &adjacency);
	if (SUCCEEDED(hr))
		_updateMeshInfo();
}

//////////////////////////////////////////////////////////////////////////

iEffectParams::~iEffectParams()
{
	clear();
}

void iEffectParams::clear()
{
	for (int i=0; i<params.GetSize(); i++)
	{
		EffectParamValue* p = params.GetValueAt(i);
		if (p) delete p;
	}
	params.RemoveAll();
	_cached = false;
}

void iEffectParams::set_params_string( LPCOLESTR szParams )
{
	if (szParams)
	{
		clear();
		params_string = szParams;
	}
}

void iEffectParams::_cache(iEffect* e)
{
	if (!_cached)
	{
		_cached = true;

		if (params_string.Length()>0)
		{
			CStrArray strs;
			if (SplitStringToArray(params_string, strs, L";"))
			{
				for (int i=0; i<strs.GetSize(); i++)
				{
					CStrArray pairs;
					if (SplitStringToArray(strs[i], pairs, L":="/*, FALSE*/) && pairs.GetSize()==2)
					{
						long idx;
						D3DXHANDLE h = e->get_param_handle(pairs[0].Trim(), &idx);
						if (h)
						{
							iEffect::EffectParam& ep = e->m_params_cache[idx];
							if (ep.desc.Type!=D3DXPT_BOOL && ep.desc.Type!=D3DXPT_INT && ep.desc.Type!=D3DXPT_FLOAT)
								continue;

							EffectParamValue* pValue = new EffectParamValue;
							pValue->set_size(&ep.desc);
							if (ep.desc.Elements==0)
							{
								if (ep.desc.Type==D3DXPT_BOOL)
									*pValue->pbVal = ParseBoolString(pairs[1]);
								else if (ep.desc.Type==D3DXPT_INT)
									TryLoad_long_FromString(pairs[1], (long&)*pValue->piVal);
								// if (ep.desc.Type==D3DXPT_FLOAT)
								else if (ep.desc.Rows>1) // matrix 不处理MATRIX
									continue;
								else if (ep.desc.Columns>1)
								{
									Vector4D* v4 = (Vector4D*)pValue->pvVal;
									v4->InitFromString(pairs[1]);
								}
								else
									TryLoad_float_FromString(pairs[1], *pValue->pfVal);
							}
							else
							{
								if (ep.desc.Rows==1 && ep.desc.Columns==1)
								{
									CStrArray vals;
									UINT num=ep.desc.Elements; 
									if (SplitStringToArray(pairs[1], vals, L", \t\r\n") && (UINT)vals.GetSize()>=num)
									{
										for (UINT j=0; j<num; j++)
										{
											if (ep.desc.Type==D3DXPT_BOOL)
												pValue->pbVal[j] = ParseBoolString(vals[j]);
											else if (ep.desc.Type==D3DXPT_INT)
												TryLoad_long_FromString(vals[j], (long&)pValue->piVal[j]);
											else // if (ep.desc.Type==D3DXPT_FLOAT)
												TryLoad_float_FromString(vals[j], pValue->pfVal[j]);
										}
									}
								}
								else if (ep.desc.Rows==1) // vector4
								{
									CStrArray vals;
									if (SplitStringToArray(pairs[1], vals, L", \t\r\n") && (UINT)vals.GetSize()>=ep.desc.Elements*ep.desc.Columns)
									{
										UINT k = 0;
										for (UINT j=0; j<ep.desc.Elements; j++)
										{
											TryLoad_float_FromString(vals[k++], pValue->pvVal[j].x);
											TryLoad_float_FromString(vals[k++], pValue->pvVal[j].y);
											if (ep.desc.Columns>=3) TryLoad_float_FromString(vals[k++], pValue->pvVal[j].z);
											if (ep.desc.Columns>=4) TryLoad_float_FromString(vals[k++], pValue->pvVal[j].w);
										}
									}
								}
							}

							params.Add(h, pValue);
						}
					}
				}
			}
		}
	}
}

EffectParamValue* iEffectParams::get_param_value( iEffect* e, D3DXHANDLE h, bool bAutoCreate/*=false*/ )
{
	if (e==NULL || h==NULL /*|| params_string.Length()==0*/) return NULL;
	if (_effect!=e)
	{
		clear();
		_effect = e;
	}

	_cache(e);

	int idx = params.FindKey(h);
	if (idx>=0)
		return params.GetValueAt(idx);

	if (bAutoCreate)
	{
		HRESULT hr;
		iEffect::EffectParam* ep = e->get_param_desc(h);
		if (ep)
		{
			D3DXPARAMETER_DESC& desc = ep->desc;
			EffectParamValue* pValue = new EffectParamValue;
			pValue->set_size(&desc);

			//_effect->m_effect->GetValue(hParam, pValue->pBuffer, pValue->nBytes);
			switch (desc.Type)
			{
			case D3DXPT_BOOL:
				if (desc.Elements>0) hr = e->m_effect->GetBoolArray(h, pValue->pbVal, pValue->nElements);
				else hr = e->m_effect->GetBool(h, pValue->pbVal);
				break;
			case D3DXPT_INT:
				if (desc.Elements>0) hr = e->m_effect->GetIntArray(h, pValue->piVal, pValue->nElements);
				else hr = e->m_effect->GetInt(h, pValue->piVal);
				break;
			case D3DXPT_FLOAT:
				if (desc.Rows>1)//4 && desc.Columns==4)
				{
					//continue;
					if (desc.Elements>0) hr = e->m_effect->GetMatrixArray(h, pValue->pmVal, pValue->nElements);
					else hr = e->m_effect->GetMatrix(h, pValue->pmVal);
				}
				else if (/*desc.Rows==1 &&*/ desc.Columns>1)
				{
					if (desc.Elements>0) hr = e->m_effect->GetVectorArray(h, pValue->pvVal, pValue->nElements);
					else hr = e->m_effect->GetVector(h, pValue->pvVal);
				}
				else //if (desc.Rows==1 && desc.Columns==1) // rows == columns == 1
				{
					if (desc.Elements>0) hr = e->m_effect->GetFloatArray(h, pValue->pfVal, pValue->nElements);
					else hr = e->m_effect->GetFloat(h, pValue->pfVal);
				}
				break;
			}

			params.Add(h, pValue);
			e->clearParamBlock();
			return pValue;
		}
	}

	return NULL;
}

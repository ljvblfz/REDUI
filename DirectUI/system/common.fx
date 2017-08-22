
// device common
shared float g_fTime : TIME;
shared float g_fDelta : TIMEDELTA;

// scene
shared float4	g_mMirrorPlane : MirrorPlane = float4(0, 0, 0, 1.0f);
shared float4x4 g_mMirrorWorld : MirrorWorld;
shared float3	g_vMirrorColor : MirrorColor = float3(.5f, .5f, .5f);
shared float4x4 g_mView : View;
shared float4x4 g_mProjection: Projection;

//--------------------------------------------------------------------------------------
// 光照变量和摄像机位置
//--------------------------------------------------------------------------------------
shared float3 g_vLightColor : LightColor = float3( 1.0f, 1.0f, 1.0f );        // Light color
shared float3 g_vLightPosition : LightPosition = float3( 0, 10.0f, -50.0f ); //float3( 50.0f, 10.0f, 0.0f );   // Light position
shared float3 g_vCameraPosition : CameraPosition;

// entity/model
shared float4x4 g_mWorld : World;          // World matrix
shared float4x4 g_mWorldViewProjection : WorldViewProjection; // World * View * Projection matrix

// 纹理参数
//--------------------------------------------------------------------------------------
// 材质对应的纹理，可为空
//--------------------------------------------------------------------------------------
texture  g_MeshTexture : Texture;   // Color texture for mesh

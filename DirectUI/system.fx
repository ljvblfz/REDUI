#include "redirectui"

// 灰度因子[0,1]。1 表示不产生灰度，0 表示完全变灰
float gray_factor = 1.0;

texture g_tClip;
sampler ClipSampler = sampler_state
{
    Texture = (g_tClip);    
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
};

// 透明颜色，如果颜色透明，仅当该颜色的A值大于0时才过滤，默认不过滤。GDI 纹理需要填充该透明色。
//float4 transparent_color = {0, 0, 0, 0};

float4 GreyClip(float4 color: COLOR0, float2 TexCoord : TEXCOORD0) : COLOR0
{
	// 设置灰度
	float gray = color.r * 0.3 + color.g * 0.59 + color.b * 0.11;
	float4 result;
	result.r = (color.r - gray) * gray_factor + gray;
	result.g = (color.g - gray) * gray_factor + gray;
	result.b = (color.b - gray) * gray_factor + gray;
	result.a = color.a; // * (1.0f - clipcolor.a);
	return result;
}

float4 ColorFillPS( float2 TexCoord : TEXCOORD0, float4 vColor: COLOR0 ) : COLOR0
{
	// 裁剪，A 值小于0.5的像素保留，否则裁剪。注意 clip 函数不能放到其它函数中调用，否则无效
	clip(0.5f - tex2D(ClipSampler, TexCoord).a);

	float4 color = GreyClip(vColor, TexCoord);
	return color;
}

technique ColorFill
{
    pass P0
    {
        CullMode = NONE;
		//ZFunc = Always;
		ZEnable = False;
        
        PixelShader = compile ps_2_0 ColorFillPS();
		VertexShader = null;
        //BlendOpAlpha = max;
		//BLENDOPALPHA = MAX;
        //ColorOp[0] = BlendTextureAlpha;
        //ColorArg1[0] = Diffuse;
        //ColorArg2[0] = Current;
        //AlphaOp[0] = SelectArg1;
        //AlphaArg1[0] = Diffuse;
    }
}

// DisabledLayer
//technique DisabledLayer
//{
//    pass P0
//    {
//        CullMode = NONE;
//		ZFunc = Always;
//        
//        PixelShader = null; //compile ps_2_0 ColorPS();
//		VertexShader = null;
//        //BlendOpAlpha = max;
//		//BLENDOPALPHA = MAX;
//		TextureFactor = 0xa0cccccc;
//        ColorOp[0] = SelectArg1;
//        ColorArg1[0] = TFactor;
//        AlphaOp[0] = SelectArg1;
//        AlphaArg1[0] = TFactor;
//    }
//}

// Image Layer
texture g_tImage;

sampler ImageSampler = sampler_state
{
    Texture = (g_tImage);
    //MinFilter = None;
    //MagFilter = None;
};

float4 ImagePS( float2 TexCoord : TEXCOORD0 ) : COLOR0
{
	// 裁剪
	clip(0.5f - tex2D(ClipSampler, TexCoord).a);

	float4 color = tex2D( ImageSampler, TexCoord );

	color = GreyClip(color, TexCoord);
	return color;
}


technique ImageLayer
{
    pass P0
    {
        CullMode = NONE;
		//ZFunc = Always;
		ZEnable = False;

		PixelShader = compile ps_2_0 ImagePS();          
		VertexShader = null;
        //BlendOpAlpha = max;
		//BLENDOPALPHA = MAX;
        //ColorOp[0] = BlendTextureAlpha;
        //ColorArg1[0] = Texture;
        //ColorArg2[0] = Current;
        //AlphaOp[0] = SelectArg1;
        //AlphaArg1[0] = Texture;

		//Sampler[0] = (ImageSampler);
    }
}

//technique GdiLayer
//{
//    pass P0
//    {
//        CullMode = NONE;
//		ZFunc = Always;
//
//		PixelShader = compile ps_2_0 ImagePS(true);          
//		VertexShader = null;
//        //BlendOpAlpha = max;
//		//BLENDOPALPHA = MAX;
//        ColorOp[0] = BlendTextureAlpha;
//        ColorArg1[0] = Texture;
//        ColorArg2[0] = Current;
//        AlphaOp[0] = SelectArg1;
//        AlphaArg1[0] = Texture;
//
//		Sampler[0] = (ImageSampler);
//    }
//}

//--------------------------------------------------------------------------------------
// 每个模型中的子集（对于一个材质）都应用下面的效果和变量，如果有自定义变量，必须设置指定的语义名称
// 下述代码参考自 MeshFromObj
// 材质变量
//--------------------------------------------------------------------------------------
float3 vAmbient : Ambient = float3( 0.2f, 0.2f, 0.2f );   // Material's ambient color
float3 vDiffuse : Diffuse = float3( 0.8f, 0.8f, 0.8f );   // Material's diffuse color
float3 vSpecular : Specular = float3( 1.0f, 1.0f, 1.0f );  // Material's specular color
float  fAlpha : Opacity = 1.0f;
int    nShininess : Power = 32;

// 鼠标跟随的灯光
float3 vMouseColor : MouseColor = float3( 0, 0, 0 );        // 0 == not use
float3 vMousePosition : MousePosition; // = float3( 0, 10.0f, -50.0f ); //float3( 50.0f, 10.0f, 0.0f );   // Light position
//float3 vMouseDir : MouseDir;



//--------------------------------------------------------------------------------------
// Texture samplers
//--------------------------------------------------------------------------------------
sampler MeshTextureSampler = 
sampler_state
{
    Texture = <g_MeshTexture>;    
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
	//MAXD3DTEXF_ANISOTROOPY = 16;
};


struct VS_INPUT
{
	float4 pos : POSITION;
	float2 uv : TEXCOORD0;
	float3 normal : NORMAL;
};

struct VS_OUTPUT
{
	float4 pos : POSITION;
	float4 diffuse : COLOR0;
	float2 uv : TEXCOORD0;
	float3 normal : TEXCOORD1;
	float3 view : TEXCOORD2;
	float3 light : TEXCOORD3;
	//float3 mouse : TEXCOORD4;
	float height : TEXCOORD4;
};

VS_OUTPUT vs_default( VS_INPUT input )
{
	VS_OUTPUT o = (VS_OUTPUT)0;

	float4 posWorld = mul(input.pos, g_mWorld);
	o.pos = mul(input.pos, g_mWorldViewProjection);
	o.uv = input.uv;
	float3 world = mul(input.pos, g_mWorld).xyz;
	o.normal = mul(input.normal, (float3x3)g_mWorld);
	o.view = g_vCameraPosition - world;
	o.light = normalize(world - g_vLightPosition);

	float3 vLight = normalize( vMousePosition - world );
	o.diffuse.rgb = vMouseColor * ( vAmbient + vDiffuse * saturate( dot(vLight, o.normal) ) );

	o.height = posWorld.x * g_mMirrorPlane.x + posWorld.y * g_mMirrorPlane.y + posWorld.z * g_mMirrorPlane.z + g_mMirrorPlane.w;

	return o;
}


float4 ps_default( VS_OUTPUT input ) : COLOR0
{
	float4 color;
	clip(input.height);

	float3 L = normalize(-input.light.xyz);
	float3 N = normalize(input.normal.xyz);
	float3 V = normalize(input.view.xyz);
	float3 R = normalize(reflect(-L, N));

	// 环境色
	color.rgb = g_vLightColor * vAmbient;
	// 增加漫反射色
	float3 diffuseSum = input.diffuse;
	diffuseSum += g_vLightColor * vDiffuse * saturate( dot(N, L) );
	//if (any(vMouseColor.rgb))
	//{
	//	float3 Lm = normalize(-input.mouse);
	//	float3 Rm = normalize(reflect(-Lm, N));
	//	diffuseSum += vMouseColor * vDiffuse * saturate( dot(N, Lm) );
	//}
	color.rgb += diffuseSum;
	// 增加纹理色
	float3 tex = tex2D( MeshTextureSampler, input.uv).rgb;
	if (any(tex))
		color.rgb *= tex;
	// 增加镜面反射色
	if (any(vSpecular))
	{
		float fPhong = saturate( dot(R, V) );
		color.rgb += vSpecular * pow(fPhong, nShininess);
		//float fPhongMouse = saturate( dot(Rm, V) );
		//color.rgb += vSpecular * pow(fPhongMouse, nShininess);
	}

	color.a = fAlpha;
	
	return color;
}

VS_OUTPUT vs_mirror( VS_INPUT input )
{
	VS_OUTPUT o = (VS_OUTPUT)0;

	float4 posWorld = mul(input.pos, g_mWorld);
	o.pos = mul(input.pos, g_mWorldViewProjection);
	o.uv = input.uv;
	float3 world = mul(input.pos, g_mWorld).xyz;
	o.normal = mul(input.normal, (float3x3)g_mWorld);
	o.view = g_vCameraPosition - world;
	o.light = normalize(world - mul(g_vLightPosition, g_mMirrorWorld));

	//float3 mouse = normalize(world - vMousePosition);
	float3 vLight = normalize( mul(vMousePosition, g_mMirrorWorld) - world );
	o.diffuse.rgb = vMouseColor * ( vAmbient + vDiffuse * saturate( dot(vLight, o.normal) ) ); // * g_vMirrorColor;

	o.height = posWorld.x * g_mMirrorPlane.x + posWorld.y * g_mMirrorPlane.y + posWorld.z * g_mMirrorPlane.z + g_mMirrorPlane.w;
	o.height = -o.height;

	return o;
}


float4 ps_mirror( VS_OUTPUT input ) : COLOR0
{
	clip(input.height);
	//clip(1.0f - input.height);

	float3 L = normalize(-input.light.xyz);
	float3 N = normalize(input.normal.xyz);
	float3 V = normalize(input.view.xyz);
	float3 R = normalize(reflect(-L, N));

	// 环境色
	float4 color = float4(g_vLightColor.rgb * vAmbient.rgb, fAlpha
#if SHADER_VERSION >= 3
	 * saturate(1.2f - input.height * 0.55f)
#endif
	 );
	//color.rgb = g_vLightColor * vAmbient;
	// 增加漫反射色
	color.rgb += input.diffuse + g_vLightColor * vDiffuse * saturate( dot(N, L) );
	//float3 diffuseSum = input.diffuse;
	//diffuseSum += g_vLightColor * vDiffuse * saturate( dot(N, L) );
	//color.rgb += diffuseSum;
	// 增加纹理色
	float3 tex = tex2D( MeshTextureSampler, input.uv).rgb;
	if (any(tex))
		color.rgb *= tex;
	// 增加镜面反射色
	if (any(vSpecular))
	{
		float fPhong = saturate( dot(R, V) );
		color.rgb += .5 * vSpecular * pow(fPhong, nShininess);
	}

	color.rgb *= .3f;
	color.rgb += .7 * g_vMirrorColor;

	//color.a = fAlpha; //0.6f;
	
	return color;
}


//--------------------------------------------------------------------------------------
// Techniques
//--------------------------------------------------------------------------------------

technique SystemDefault
{
    pass P0
    {
		CullMode = none; //CCW;
#if SHADER_VERSION >= 3
        VertexShader = compile vs_3_0 vs_default();
        PixelShader = compile ps_3_0 ps_default();
#else
        VertexShader = compile vs_2_0 vs_default();
        PixelShader = compile ps_2_0 ps_default();
#endif
		ZEnable = True;
		ZWriteEnable = True;
		ZFunc = LessEqual;
    }
}

technique SystemMirror
{
    pass P0
    {
		CullMode = none; //CW;
#if SHADER_VERSION >= 3
        VertexShader = compile vs_3_0 vs_mirror();
        PixelShader = compile ps_3_0 ps_mirror();
#else
        VertexShader = compile vs_2_0 vs_mirror();
        PixelShader = compile ps_2_0 ps_mirror();
#endif
		ZEnable = True;
		ZWriteEnable = True;
		ZFunc = LessEqual;
    }
}

//technique SystemDefaultMirror
//{
//    pass P0
//    {
//		//CullMode = CCW;
//        VertexShader = compile vs_2_0 vs_default();
//        PixelShader = compile ps_2_0 ps_default();
//		ZEnable = True;
//		ZWriteEnable = True;
//		ZFunc = LessEqual;
//    }
//	pass P1 < string name="draw mirror mesh"; >
//	{
//		StencilEnable = True;
//		StencilFunc = Always;
//		StencilRef = 0x1;
//		StencilMask = 0xffffffff;
//		StencilWriteMask = 0xffffffff;
//		StencilZFail = Keep;
//		StencilFail = Keep;
//		StencilPass = Replace;
//	}
//}

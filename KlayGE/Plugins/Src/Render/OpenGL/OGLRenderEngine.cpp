// OGLRenderEngine.cpp
// KlayGE OpenGL渲染引擎类 实现文件
// Ver 2.0.1
// 版权所有(C) 龚敏敏, 2003
// Homepage: http://www.enginedev.com
//
// 2.0.1
// 初次建立 (2003.10.11)
//
// 修改记录
//////////////////////////////////////////////////////////////////////////////////

#include <KlayGE/KlayGE.hpp>
#include <KlayGE/ThrowErr.hpp>
#include <KlayGE/SharePtr.hpp>
#include <KlayGE/Math.hpp>
#include <KlayGE/Memory.hpp>
#include <KlayGE/Engine.hpp>

#include <KlayGE/Light.hpp>
#include <KlayGE/Material.hpp>
#include <KlayGE/Viewport.hpp>
#include <KlayGE/VertexBuffer.hpp>
#include <KlayGE/RenderTarget.hpp>
#include <KlayGE/RenderEffect.hpp>
#include <KlayGE/OpenGL/OGLRenderWindow.hpp>
#include <KlayGE/OpenGL/OGLTexture.hpp>

#include <cassert>
#include <algorithm>

#include <KlayGE/OpenGL/OGLRenderEngine.hpp>

#pragma comment(lib, "OpenGL32.lib")
#pragma comment(lib, "glu32.lib")

namespace KlayGE
{
	// 从RenderEngine::CompareFunction转换到D3DCMPFUNC
	/////////////////////////////////////////////////////////////////////////////////
	GLint Convert(RenderEngine::CompareFunction func)
	{
		GLint ret;
		switch (func)
		{
		case RenderEngine::CF_AlwaysFail:
			ret = GL_NEVER;
			break;

		case RenderEngine::CF_AlwaysPass:
			ret = GL_ALWAYS;
			break;

		case RenderEngine::CF_Less:
			ret = GL_LESS;
			break;

		case RenderEngine::CF_LessEqual:
			ret = GL_LEQUAL;
			break;

		case RenderEngine::CF_Equal:
			ret = GL_EQUAL;
			break;

		case RenderEngine::CF_NotEqual:
			ret = GL_NOTEQUAL;
			break;

		case RenderEngine::CF_GreaterEqual:
			ret = GL_GEQUAL;
			break;

		case RenderEngine::CF_Greater:
			ret = GL_GREATER;
			break;
		};

		return ret;
	}

	// 从RenderEngine::StencilOperation转换到D3DSTENCILOP
	/////////////////////////////////////////////////////////////////////////////////
	GLint Convert(RenderEngine::StencilOperation op)
	{
		GLint ret;
		switch (op)
		{
		case RenderEngine::SOP_Keep:
			ret = GL_KEEP;
			break;

		case RenderEngine::SOP_Zero:
			ret = GL_ZERO;
			break;

		case RenderEngine::SOP_Replace:
			ret = GL_REPLACE;
			break;

		case RenderEngine::SOP_Increment:
			ret = GL_INCR;
			break;

		case RenderEngine::SOP_Decrement:
			ret = GL_DECR;
			break;

		case RenderEngine::SOP_Invert:
			ret = GL_INVERT;
			break;
		};

		return ret;
	}

/*	// 从RenderEngine::TexFiltering转换到D3D的MagFilter标志
	/////////////////////////////////////////////////////////////////////////////////
	U32 MagFilter(const D3DCAPS9& caps, RenderEngine::TexFiltering tf)
	{
		// NOTE: Fall through if device doesn't support requested type
		if ((RenderEngine::TF_Anisotropic == tf) && (caps.TextureFilterCaps & D3DPTFILTERCAPS_MAGFANISOTROPIC))
		{
			return D3DTEXF_ANISOTROPIC;
		}
		else
		{
			tf = RenderEngine::TF_Trilinear;
		}

		if ((RenderEngine::TF_Trilinear == tf) && (caps.TextureFilterCaps & D3DPTFILTERCAPS_MAGFLINEAR))
		{
			return D3DTEXF_LINEAR;
		}
		else
		{
			tf = RenderEngine::TF_Bilinear;
		}

		if ((RenderEngine::TF_Bilinear == tf) && (caps.TextureFilterCaps & D3DPTFILTERCAPS_MAGFLINEAR))
		{
			return D3DTEXF_LINEAR;
		}

		return D3DTEXF_POINT;
	}

	// 从RenderEngine::TexFiltering转换到D3D的MinFilter标志
	/////////////////////////////////////////////////////////////////////////////////
	U32 MinFilter(const D3DCAPS9& caps, RenderEngine::TexFiltering tf)
	{
		// NOTE: Fall through if device doesn't support requested type
		if ((RenderEngine::TF_Anisotropic == tf) && (caps.TextureFilterCaps & D3DPTFILTERCAPS_MINFANISOTROPIC))
		{
			return D3DTEXF_ANISOTROPIC;
		}
		else
		{
			tf = RenderEngine::TF_Trilinear;
		}

		if ((RenderEngine::TF_Trilinear == tf) && (caps.TextureFilterCaps & D3DPTFILTERCAPS_MINFLINEAR))
		{
			return D3DTEXF_LINEAR;
		}
		else
		{
			tf = RenderEngine::TF_Bilinear;
		}

		if ((RenderEngine::TF_Bilinear == tf) && (caps.TextureFilterCaps & D3DPTFILTERCAPS_MINFLINEAR))
		{
			return D3DTEXF_LINEAR;
		}

		return D3DTEXF_POINT;
	}

	// 从RenderEngine::TexFiltering转换到D3D的MipFilter标志
	/////////////////////////////////////////////////////////////////////////////////
	U32 MipFilter(const D3DCAPS9& caps, RenderEngine::TexFiltering tf)
	{
		// NOTE: Fall through if device doesn't support requested type
		if ((RenderEngine::TF_Anisotropic == tf) && (caps.TextureFilterCaps & D3DPTFILTERCAPS_MIPFLINEAR))
		{
			return D3DTEXF_LINEAR;
		}
		else
		{
			tf = RenderEngine::TF_Trilinear;
		}

		if ((RenderEngine::TF_Trilinear == tf) && (caps.TextureFilterCaps & D3DPTFILTERCAPS_MIPFLINEAR))
		{
			return D3DTEXF_LINEAR;
		}
		else
		{
			tf = RenderEngine::TF_Bilinear;
		}

		if ((RenderEngine::TF_Bilinear == tf) && (caps.TextureFilterCaps & D3DPTFILTERCAPS_MIPFLINEAR))
		{
			return D3DTEXF_POINT;
		}

		return D3DTEXF_NONE;
	}*/
}

namespace KlayGE
{
	// 构造函数
	/////////////////////////////////////////////////////////////////////////////////
	OGLRenderEngine::OGLRenderEngine()
		: cullingMode_(RenderEngine::Cull_None),
			worldMat_(Matrix4::Identity()),
			viewMat_(Matrix4::Identity()),
			projMat_(Matrix4::Identity())
	{
	}

	// 析构函数
	/////////////////////////////////////////////////////////////////////////////////
	OGLRenderEngine::~OGLRenderEngine()
	{
	}

	// 返回渲染系统的名字
	/////////////////////////////////////////////////////////////////////////////////
	const WString& OGLRenderEngine::Name() const
	{
		static WString name(L"OpenGL Render System");
		return name;
	}

	// 开始渲染
	/////////////////////////////////////////////////////////////////////////////////
	void OGLRenderEngine::StartRendering()
	{
		bool gotMsg;
		MSG  msg;

		::PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE);

		while (WM_QUIT != msg.message)
		{
			// 如果窗口是激活的，用 PeekMessage()以便我们可以用空闲时间渲染场景
			// 不然, 用 GetMessage() 减少 CPU 占用率
			if ((*RenderEngine::ActiveRenderTarget())->Active())
			{
				gotMsg = ::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) ? true : false;
			}
			else
			{
				gotMsg = ::GetMessage(&msg, NULL, 0, 0) ? true : false;
			}

			if (gotMsg)
			{
				::TranslateMessage(&msg);
				::DispatchMessage(&msg);
			}
			else
			{
				// 在空余时间渲染帧 (没有等待的消息)
				if ((*RenderEngine::ActiveRenderTarget())->Active())
				{
					(*RenderEngine::ActiveRenderTarget())->Update();
				}
			}
		}
	}

	// 设置环境光
	/////////////////////////////////////////////////////////////////////////////////
	void OGLRenderEngine::AmbientLight(const Color& col)
	{
		GLfloat ambient[] = { col.r(), col.g(), col.b(), 1.0f };
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);
	}

	// 设置清除颜色
	/////////////////////////////////////////////////////////////////////////////////
	void OGLRenderEngine::ClearColor(const Color& col)
	{
		glClearColor(col.r(), col.g(), col.b(), col.a());
	}

	// 设置光影类型
	/////////////////////////////////////////////////////////////////////////////////
	void OGLRenderEngine::ShadingType(ShadeOptions so)
	{
		GLenum shadeMode;
		switch (so)
		{
		case SO_Flat:
			shadeMode = GL_FLAT;
			break;

		case SO_Gouraud:
			shadeMode = GL_SMOOTH;
			break;

		case SO_Phong:
			shadeMode = GL_SMOOTH;
			break;
		}

		glShadeModel(shadeMode);
	}

	// 打开/关闭光源
	/////////////////////////////////////////////////////////////////////////////////
	void OGLRenderEngine::EnableLighting(bool enabled)
	{
		if (enabled)
		{
			glEnable(GL_LIGHTING);
		}
		else
		{
			glDisable(GL_LIGHTING);
		}
	}

	// 建立渲染窗口
	/////////////////////////////////////////////////////////////////////////////////
	RenderWindowPtr OGLRenderEngine::CreateRenderWindow(const String& name,
		const RenderWindowSettings& settings)
	{
		RenderWindowPtr win(new OGLRenderWindow(name,
			static_cast<const OGLRenderWindowSettings&>(settings)));

		this->ActiveRenderTarget(this->AddRenderTarget(win));

		this->DepthBufferDepthTest(settings.depthBuffer);
		this->DepthBufferDepthWrite(settings.depthBuffer);

		this->SetMaterial(Material(Color(1, 1, 1, 1)));

		return win;
	}

	// 设置剪裁模式
	/////////////////////////////////////////////////////////////////////////////////
	void OGLRenderEngine::CullingMode(CullMode mode)
	{
		cullingMode_ = mode;

		switch (mode)
		{
		case Cull_None:
			glDisable(GL_CULL_FACE);
			break;

		case Cull_Clockwise:
			glEnable(GL_CULL_FACE);
			glFrontFace(GL_CCW);
			break;

		case Cull_AntiClockwise:
			glEnable(GL_CULL_FACE);
			glFrontFace(GL_CW);
			break;
		}
	}

	// 设置光源
	/////////////////////////////////////////////////////////////////////////////////
	void OGLRenderEngine::SetLight(U32 index, const Light& light)
	{
		GLint lightIndex(GL_LIGHT0 + index);

		switch (light.lightType)
		{
		case Light::LT_Spot:
			glLightf(lightIndex, GL_SPOT_CUTOFF, light.spotOuter);
			break;

		default:
			glLightf(lightIndex, GL_SPOT_CUTOFF, 180.0f);
			break;
		}

		GLfloat ambient[4] = { light.ambient.r(), light.ambient.g(), light.ambient.b(), light.ambient.a() };
		glLightfv(lightIndex, GL_AMBIENT, ambient);

		GLfloat diffuse[4] = { light.diffuse.r(), light.diffuse.g(), light.diffuse.b(), light.diffuse.a() };
		glLightfv(lightIndex, GL_DIFFUSE, diffuse);

		GLfloat specular[4] = { light.specular.r(), light.specular.g(), light.specular.b(), light.specular.a() };
		glLightfv(lightIndex, GL_SPECULAR, specular);

		// Set position / direction
		GLfloat f4vals[4];
		switch (light.lightType)
		{
		case Light::LT_Point:
			f4vals[0] = light.position.x();
			f4vals[1] = light.position.y();
			f4vals[2] = light.position.z();
			f4vals[3] = 1.0f;
			glLightfv(lightIndex, GL_POSITION, f4vals);
			break;

		case Light::LT_Directional:
			f4vals[0] = -light.direction.x(); // GL light directions are in eye coords
			f4vals[1] = -light.direction.y();
			f4vals[2] = -light.direction.z(); // GL light directions are in eye coords
			f4vals[3] = 0.0f; // important!
			// In GL you set direction through position, but the
			//  w value of the vector being 0 indicates which it is
			glLightfv(lightIndex, GL_POSITION, f4vals);
			break;

		case Light::LT_Spot:
			f4vals[0] = light.position.x();
			f4vals[1] = light.position.y();
			f4vals[2] = light.position.z();
			f4vals[3] = 1.0f;
			glLightfv(lightIndex, GL_POSITION, f4vals);

			f4vals[0] = light.direction.x();
			f4vals[1] = light.direction.y();
			f4vals[2] = light.direction.z();
			f4vals[3] = 0.0f; 
			glLightfv(lightIndex, GL_SPOT_DIRECTION, f4vals);
		}

		glLightf(lightIndex, GL_CONSTANT_ATTENUATION, light.attenuationConst);
		glLightf(lightIndex, GL_LINEAR_ATTENUATION, light.attenuationLinear);
		glLightf(lightIndex, GL_QUADRATIC_ATTENUATION, light.attenuationQuad);
	}

	// 打开/关闭某个光源
	/////////////////////////////////////////////////////////////////////////////////
	void OGLRenderEngine::LightEnable(U32 index, bool enabled)
	{
		if (enabled)
		{
			glEnable(GL_LIGHT0 + index);
		}
		else
		{
			glDisable(GL_LIGHT0 + index);
		}
	}

	// 获取世界矩阵
	/////////////////////////////////////////////////////////////////////////////////
	Matrix4 OGLRenderEngine::WorldMatrix() const
	{
		return worldMat_;
	}

	// 设置世界矩阵
	/////////////////////////////////////////////////////////////////////////////////
	void OGLRenderEngine::WorldMatrix(const Matrix4& mat)
	{
		worldMat_ = mat;

		glMatrixMode(GL_MODELVIEW);

		Matrix4 oglWorldMat(worldMat_);
		oglWorldMat(3, 0) = -oglWorldMat(3, 0);

		Matrix4 oglViewMat(viewMat_);
		for (size_t i = 0; i < 4; ++ i)
		{
			oglViewMat(i, 0) = -oglViewMat(i, 0);
			oglViewMat(i, 2) = -oglViewMat(i, 2);
		}

		Matrix4 oglMat(oglWorldMat * oglViewMat);
		glLoadMatrixf(&oglMat(0, 0));
	}

	// 设置多个世界矩阵
	/////////////////////////////////////////////////////////////////////////////////
	void OGLRenderEngine::WorldMatrices(Matrix4* mats, size_t count)
	{
	}

	// 获取观察矩阵
	/////////////////////////////////////////////////////////////////////////////////
	Matrix4 OGLRenderEngine::ViewMatrix()
	{
		return viewMat_;
	}

	// 设置观察矩阵
	/////////////////////////////////////////////////////////////////////////////////
	void OGLRenderEngine::ViewMatrix(const Matrix4& mat)
	{
		viewMat_ = mat;

		glMatrixMode(GL_MODELVIEW);

		Matrix4 oglWorldMat(worldMat_);
		oglWorldMat(3, 0) = -oglWorldMat(3, 0);

		Matrix4 oglViewMat(viewMat_);
		for (size_t i = 0; i < 4; ++ i)
		{
			oglViewMat(i, 0) = -oglViewMat(i, 0);
			oglViewMat(i, 2) = -oglViewMat(i, 2);
		}

		Matrix4 oglMat(oglWorldMat * oglViewMat);
		glLoadMatrixf(&oglMat(0, 0));
	}

	// 获取投射矩阵
	/////////////////////////////////////////////////////////////////////////////////
	Matrix4 OGLRenderEngine::ProjectionMatrix()
	{
		return projMat_;
	}

	// 设置投射矩阵
	/////////////////////////////////////////////////////////////////////////////////
	void OGLRenderEngine::ProjectionMatrix(const Matrix4& mat)
	{
		projMat_ = mat;

		Matrix4 oglMat;
		Engine::MathInstance().LHToRH(oglMat, mat);

		glMatrixMode(GL_PROJECTION);
		glLoadMatrixf(&oglMat(0, 0));
		glMatrixMode(GL_MODELVIEW);
	}

	// 设置材质
	/////////////////////////////////////////////////////////////////////////////////
	void OGLRenderEngine::SetMaterial(const Material& material)
	{
		GLfloat ambient[4] = { material.ambient.r(), material.ambient.g(), material.ambient.b(), material.ambient.a() };
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);

		GLfloat diffuse[4] = { material.diffuse.r(), material.diffuse.g(), material.diffuse.b(), material.diffuse.a() };
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);

		GLfloat specular[4] = { material.specular.r(), material.specular.g(), material.specular.b(), material.specular.a() };
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);

		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, material.shininess);
	}

	// 设置当前渲染目标，该渲染目标必须已经在列表中
	/////////////////////////////////////////////////////////////////////////////////
	void OGLRenderEngine::ActiveRenderTarget(RenderTargetListIterator iter)
	{
		RenderEngine::ActiveRenderTarget(iter);

		this->CullingMode(cullingMode_);

		const Viewport& vp((*iter)->GetViewport());
		glViewport(vp.left, vp.top, vp.width, vp.height);
	}

	// 开始一帧
	/////////////////////////////////////////////////////////////////////////////////
	void OGLRenderEngine::BeginFrame()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	// 渲染
	/////////////////////////////////////////////////////////////////////////////////
	void OGLRenderEngine::Render(const VertexBuffer& vb)
	{
		{
			glEnableClientState(GL_VERTEX_ARRAY);
			glVertexPointer(3, GL_FLOAT, vb.vertexStride, vb.pVertices);
		}

		if (vb.vertexOptions & VertexBuffer::VO_Normals)
		{
			glEnableClientState(GL_NORMAL_ARRAY);
			glNormalPointer(GL_FLOAT, vb.normalStride, vb.pNormals);
		}
		else
		{
			glDisableClientState(GL_NORMAL_ARRAY);
		}

		if (vb.vertexOptions & VertexBuffer::VO_Diffuses)
		{
			glEnableClientState(GL_COLOR_ARRAY);
			glColorPointer(4, GL_UNSIGNED_BYTE, vb.diffuseStride, vb.pDiffuses);
		}
		else
		{
			glDisableClientState(GL_COLOR_ARRAY);
		}

		GLenum mode;
		switch (vb.type)
		{
		case VertexBuffer::BT_PointList:
			mode = GL_POINTS;
			break;

		case VertexBuffer::BT_LineList:
			mode = GL_LINES;
			break;

		case VertexBuffer::BT_LineStrip:
			mode = GL_LINE_STRIP;
			break;

		case VertexBuffer::BT_TriangleList:
			mode = GL_TRIANGLES;
			break;

		case VertexBuffer::BT_TriangleStrip:
			mode = GL_TRIANGLE_STRIP;
			break;

		case VertexBuffer::BT_TriangleFan:
			mode = GL_TRIANGLE_FAN;
			break;
		}

		if (vb.useIndices)
		{
			glDrawElements(mode, vb.numIndices, GL_UNSIGNED_SHORT, vb.pIndices);
		}
		else
		{
			glDrawArrays(mode, 0, vb.numVertices);
		}
	}

	// 结束一帧
	/////////////////////////////////////////////////////////////////////////////////
	void OGLRenderEngine::EndFrame()
	{
	}

	// 打开/关闭深度测试
	/////////////////////////////////////////////////////////////////////////////////
	void OGLRenderEngine::DepthBufferDepthTest(bool enabled)
	{
		if (enabled)
		{
			glClearDepth(1.0f);
			glEnable(GL_DEPTH_TEST);
		}
		else
		{
			glDisable(GL_DEPTH_TEST);
		}
	}

	// 打开/关闭深度缓存
	/////////////////////////////////////////////////////////////////////////////////
	void OGLRenderEngine::DepthBufferDepthWrite(bool enabled)
	{
		glDepthMask(enabled ? GL_TRUE : GL_FALSE);
	}

	// 设置深度比较函数
	/////////////////////////////////////////////////////////////////////////////////
	void OGLRenderEngine::DepthBufferFunction(CompareFunction func)
	{
		glDepthFunc(Convert(func));
	}

	// 设置深度偏移
	/////////////////////////////////////////////////////////////////////////////////
	void OGLRenderEngine::DepthBias(U16 bias)
	{
		glEnable(GL_POLYGON_OFFSET_FILL);
		glEnable(GL_POLYGON_OFFSET_POINT);
		glEnable(GL_POLYGON_OFFSET_LINE);
		// Bias is in {0, 16}, scale the unit addition appropriately
		glPolygonOffset(1.0f, bias);
	}

	// 设置雾化效果
	/////////////////////////////////////////////////////////////////////////////////
	void OGLRenderEngine::Fog(FogMode mode, const Color& color,
		float expDensity, float linearStart, float linearEnd)
	{
		GLint fogMode;
		switch (mode)
		{
		case Fog_Exp:
			fogMode = GL_EXP;
			break;

		case Fog_Exp2:
			fogMode = GL_EXP2;
			break;

		case Fog_Linear:
			fogMode = GL_LINEAR;
			break;

		default:
			// Give up on it
			glDisable(GL_FOG);
			return;
		}

		glEnable(GL_FOG);
		glFogi(GL_FOG_MODE, fogMode);
		GLfloat fogColor[4] = { color.r(), color.g(), color.b(), color.a() };
		glFogfv(GL_FOG_COLOR, fogColor);
		glFogf(GL_FOG_DENSITY, expDensity);
		glFogf(GL_FOG_START, linearStart);
		glFogf(GL_FOG_END, linearEnd);
	}

	// 设置纹理
	/////////////////////////////////////////////////////////////////////////////////
	void OGLRenderEngine::SetTexture(U32 stage, const Texture& texture)
	{
		const OGLTexture& oglTexture(reinterpret_cast<const OGLTexture&>(texture));
		glBindTexture(GL_TEXTURE_2D, oglTexture.GLTexture());
	}

	// 设置纹理坐标集
	/////////////////////////////////////////////////////////////////////////////////
	void OGLRenderEngine::TextureCoordSet(U32 stage, int index)
	{
	}

	// 获取最大纹理阶段数
	/////////////////////////////////////////////////////////////////////////////////
	U32 OGLRenderEngine::MaxTextureStages()
	{
		return 1;
	}

	// 关闭某个纹理阶段
	/////////////////////////////////////////////////////////////////////////////////
	void OGLRenderEngine::DisableTextureStage(U32 stage)
	{
	}

	// 计算纹理坐标
	/////////////////////////////////////////////////////////////////////////////////
	void OGLRenderEngine::TextureCoordCalculation(U32 stage, TexCoordCalcMethod m)
	{
	}

	// 设置纹理寻址模式
	/////////////////////////////////////////////////////////////////////////////////
	void OGLRenderEngine::TextureAddressingMode(U32 stage, TexAddressingMode tam)
	{
	}

	// 设置纹理坐标
	/////////////////////////////////////////////////////////////////////////////////
	void OGLRenderEngine::TextureMatrix(U32 stage, const Matrix4& mat)
	{
	}

	// 设置纹理过滤模式
	/////////////////////////////////////////////////////////////////////////////////
	void OGLRenderEngine::TextureFiltering(U32 stage, TexFiltering texFiltering)
	{
	}

	// 设置纹理异性过滤
	/////////////////////////////////////////////////////////////////////////////////
	void OGLRenderEngine::TextureAnisotropy(U32 stage, U32 maxAnisotropy)
	{
	}

	// 获取最大坐标数
	/////////////////////////////////////////////////////////////////////////////////
	U32 OGLRenderEngine::MaxVertexBlendMatrices()
	{
		return 1;
	}

	// 打开模板缓冲区
	/////////////////////////////////////////////////////////////////////////////////
	void OGLRenderEngine::StencilCheckEnabled(bool enabled)
	{
		if (enabled)
		{
			glEnable(GL_STENCIL_TEST);
		}
		else
		{
			glDisable(GL_STENCIL_TEST);
		}
	}

	// 硬件是否支持模板缓冲区
	/////////////////////////////////////////////////////////////////////////////////
	bool OGLRenderEngine::HasHardwareStencil()
	{
		return false;
	}

	// 设置模板位数
	/////////////////////////////////////////////////////////////////////////////////
	U16 OGLRenderEngine::StencilBufferBitDepth()
	{
		return 8;
	}

	// 设置模板比较函数
	/////////////////////////////////////////////////////////////////////////////////
	void OGLRenderEngine::StencilBufferFunction(CompareFunction func)
	{
	}

	// 设置模板缓冲区参考值
	/////////////////////////////////////////////////////////////////////////////////
	void OGLRenderEngine::StencilBufferReferenceValue(U32 refValue)
	{
	}

	// 设置模板缓冲区掩码
	/////////////////////////////////////////////////////////////////////////////////
	void OGLRenderEngine::StencilBufferMask(U32 mask)
	{
	}

	// 设置模板缓冲区测试失败后的操作
	/////////////////////////////////////////////////////////////////////////////////
	void OGLRenderEngine::StencilBufferFailOperation(StencilOperation op)
	{
	}

	// 设置模板缓冲区深度测试失败后的操作
	/////////////////////////////////////////////////////////////////////////////////
	void OGLRenderEngine::StencilBufferDepthFailOperation(StencilOperation op)
	{
	}

	// 设置模板缓冲区通过后的操作
	/////////////////////////////////////////////////////////////////////////////////
	void OGLRenderEngine::StencilBufferPassOperation(StencilOperation op)
	{
	}
}

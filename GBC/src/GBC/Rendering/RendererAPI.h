#pragma once

#include "GBC/Rendering/VertexArray.h"
#include <glm/glm.hpp>

namespace gbc
{
	enum class RendererPrimitive : uint8_t
	{
		// OpenGL, Vulkan, Direct3D, and Metal support these
		Points,
		Lines,
		LineStrip,
		Triangles,
		TriangleStrip,

		// OpenGL, Vulkan, and Direct3D support these
		LinesAdjacency,
		LineStripAdjacency,
		TrianglesAdjacency,
		TriangleStripAdjacency,

		// OpenGL and Vulkan support these
		LineLoop,
		TriangleFan,
	};

	class RendererAPI
	{
	private:
		virtual void Init() = 0;
		virtual void Shutdown() = 0;
	public:
		virtual void EnableDepthTest() = 0;
		virtual void DisableDepthTest() = 0;
		virtual void EnableBlending() = 0;
		virtual void DisableBlending() = 0;
		virtual void EnableCullFace() = 0;
		virtual void DisableCullFace() = 0;

		virtual void SetViewport(int32_t x, int32_t y, int32_t width, int32_t height) = 0;

		virtual void Clear() = 0;
		virtual void ClearDepthOnly() = 0;
		virtual void SetClearColor(float red, float green, float blue, float alpha) = 0;

		virtual void DrawIndexed(const Ref<VertexArray>& vertexArray, const Ref<IndexBuffer>& indexBuffer, uint32_t offset, uint32_t count, RendererPrimitive primitive) = 0;

		// Renderer capabilities
		virtual int GetMaxTextureSlots() = 0;
		virtual int GetMaxTextureSize() = 0;
		virtual int GetMaxFramebufferWidth() = 0;
		virtual int GetMaxFramebufferHeight() = 0;
		virtual int GetMaxFramebufferColorAttachments() = 0;

		enum class API { Headless, OpenGL, Vulkan, Direct3D, Metal };
		static API GetAPI() noexcept { return api; }
	private:
		friend class Renderer;
		static API api;

		static Scope<RendererAPI> Create();
	};
}

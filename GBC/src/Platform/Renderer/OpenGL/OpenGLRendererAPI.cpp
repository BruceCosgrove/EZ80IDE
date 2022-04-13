#include "gbcpch.h"
#include "OpenGLRendererAPI.h"
#include <glad/glad.h>

namespace gbc
{
	static constexpr void DebugMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
	{
		switch (severity)
		{
			case GL_DEBUG_SEVERITY_HIGH:         GBC_CORE_FATAL(message); return;
			case GL_DEBUG_SEVERITY_MEDIUM:       GBC_CORE_ERROR(message); return;
			case GL_DEBUG_SEVERITY_LOW:          GBC_CORE_WARN (message); return;
			case GL_DEBUG_SEVERITY_NOTIFICATION: GBC_CORE_TRACE(message); return;
		}

		GBC_CORE_ASSERT(false, "Unknown OpenGL severity level.");
	}

	static constexpr GLenum GetOpenGLPrimitive(RendererPrimitive primitive)
	{
		switch (primitive)
		{
			case RendererPrimitive::Points:					return GL_POINTS;
			case RendererPrimitive::Lines:					return GL_LINES;
			case RendererPrimitive::LineStrip:				return GL_LINE_STRIP;
			case RendererPrimitive::Triangles:				return GL_TRIANGLES;
			case RendererPrimitive::TriangleStrip:			return GL_TRIANGLE_STRIP;
			case RendererPrimitive::LinesAdjacency:			return GL_LINES_ADJACENCY;
			case RendererPrimitive::LineStripAdjacency:		return GL_LINE_STRIP_ADJACENCY;
			case RendererPrimitive::TrianglesAdjacency:		return GL_TRIANGLES_ADJACENCY;
			case RendererPrimitive::TriangleStripAdjacency: return GL_TRIANGLE_STRIP_ADJACENCY;
			case RendererPrimitive::LineLoop:				return GL_LINE_LOOP;
			case RendererPrimitive::TriangleFan:			return GL_TRIANGLE_FAN;
		}

		GBC_CORE_ASSERT(false, "Unknown Renderer Primitive!");
		return 0;
	}

	static constexpr GLenum GetOpenGLIndexType(IndexBufferElementType indexType)
	{
		switch (indexType)
		{
			case IndexBufferElementType::UInt32: return GL_UNSIGNED_INT;
			case IndexBufferElementType::UInt16: return GL_UNSIGNED_SHORT;
			case IndexBufferElementType::UInt8:  return GL_UNSIGNED_BYTE;
		}

		GBC_CORE_ASSERT(false, "Unknown Buffer Index Type!");
		return 0;
	}

	void OpenGLRendererAPI::Init()
	{
		GBC_PROFILE_FUNCTION();

		clearBits |= GL_COLOR_BUFFER_BIT;

		// TODO: this should be abstracted (shouldn't go here)
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

#if GBC_CONFIG_DEBUG
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(DebugMessageCallback, nullptr);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);
#endif
	}

	void OpenGLRendererAPI::Shutdown()
	{

	}

	void OpenGLRendererAPI::EnableDepthTest()
	{
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		// TODO: this should include GL_STENCIL_BUFFER_BIT if using the stencil buffer
		clearBits |= GL_DEPTH_BUFFER_BIT;
	}

	void OpenGLRendererAPI::DisableDepthTest()
	{
		glDisable(GL_DEPTH_TEST);
		glDepthFunc(GL_ALWAYS);
		// TODO: this should include GL_STENCIL_BUFFER_BIT if using the stencil buffer
		clearBits &= ~GL_DEPTH_BUFFER_BIT;
	}

	void OpenGLRendererAPI::EnableBlending()
	{
		glEnable(GL_BLEND);
	}

	void OpenGLRendererAPI::DisableBlending()
	{
		glDisable(GL_BLEND);
	}

	void OpenGLRendererAPI::EnableCullFace()
	{
		glEnable(GL_CULL_FACE);
	}

	void OpenGLRendererAPI::DisableCullFace()
	{
		glDisable(GL_CULL_FACE);
	}

	void OpenGLRendererAPI::SetViewport(int32_t x, int32_t y, int32_t width, int32_t height)
	{
		glViewport(x, y, width, height);
	}

	void OpenGLRendererAPI::Clear()
	{
		glClear(clearBits);
	}

	void OpenGLRendererAPI::ClearDepthOnly()
	{
		glClear(GL_DEPTH_BUFFER_BIT);
	}

	void OpenGLRendererAPI::SetClearColor(float red, float green, float blue, float alpha)
	{
		glClearColor(red, green, blue, alpha);
	}

	void OpenGLRendererAPI::DrawIndexed(const Ref<VertexArray>& vertexArray, const Ref<IndexBuffer>& indexBuffer, uint32_t offset, uint32_t count, RendererPrimitive primitive)
	{
		vertexArray->Bind();
		indexBuffer->Bind();
		int indexCount = count != 0 ? count : indexBuffer->GetCount();
		glDrawElements(GetOpenGLPrimitive(primitive), indexCount, GetOpenGLIndexType(indexBuffer->GetType()), (const void*)(offset * static_cast<GLsizeiptr>(indexBuffer->GetType())));
	}

	// Renderer capabilities

	int32_t OpenGLRendererAPI::GetMaxTextureSlots()
	{
		int32_t maxTextureSlots = 0;
		glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &maxTextureSlots);
		return maxTextureSlots;
	}

	int32_t OpenGLRendererAPI::GetMaxTextureSize()
	{
		int32_t maxTextureSize = 0;
		glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);
		return maxTextureSize;
	}

	int32_t OpenGLRendererAPI::GetMaxFramebufferWidth()
	{
		int32_t maxFramebufferWidth = 0;
		glGetIntegerv(GL_MAX_FRAMEBUFFER_WIDTH, &maxFramebufferWidth);
		return maxFramebufferWidth;
	}

	int32_t OpenGLRendererAPI::GetMaxFramebufferHeight()
	{
		int32_t maxFramebufferHeight = 0;
		glGetIntegerv(GL_MAX_FRAMEBUFFER_HEIGHT, &maxFramebufferHeight);
		return maxFramebufferHeight;
	}

	int32_t OpenGLRendererAPI::GetMaxFramebufferColorAttachments()
	{
		int32_t maxFramebufferColorAttachments = 0;
		glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &maxFramebufferColorAttachments);
		return maxFramebufferColorAttachments;
	}
}

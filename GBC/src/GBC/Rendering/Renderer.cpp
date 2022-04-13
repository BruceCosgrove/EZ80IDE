#include "gbcpch.h"
#include "Renderer.h"
#include "GBC/Rendering/Renderers/Renderer2D.h"

namespace gbc
{
	Scope<RendererAPI> Renderer::api = nullptr;

	struct RendererData
	{
		Ref<UniformBuffer> cameraUniformBuffer;
	};
	static RendererData data;

	void Renderer::Init()
	{
		GBC_PROFILE_FUNCTION();

		data.cameraUniformBuffer = UniformBuffer::Create(sizeof(Renderer::CameraBuffer), 0, nullptr, BufferUsage::DynamicDraw);

		api = RendererAPI::Create();
		api->Init();
		Renderer2D::Init();
	}

	void Renderer::Shutdown()
	{
		GBC_PROFILE_FUNCTION();

		Renderer2D::Shutdown();
		api->Shutdown();

		data.cameraUniformBuffer.reset();
	}

	void Renderer::EnableDepthTest()
	{ api->EnableDepthTest(); }

	void Renderer::DisableDepthTest()
	{ api->DisableDepthTest(); }

	void Renderer::EnableBlending()
	{ api->EnableBlending(); }

	void Renderer::DisableBlending()
	{ api->DisableBlending(); }

	void Renderer::EnableCullFace()
	{ api->EnableCullFace(); }

	void Renderer::DisableCullFace()
	{ api->DisableCullFace(); }

	void Renderer::SetViewport(const glm::ivec2& position, const glm::ivec2& size)
	{ api->SetViewport(position.x, position.y, size.x, size.y); }
	void Renderer::SetViewport(int32_t x, int32_t y, int32_t width, int32_t height)
	{ api->SetViewport(x, y, width, height); }

	void Renderer::Clear()
	{ api->Clear(); }

	void Renderer::ClearDepthOnly()
	{ api->ClearDepthOnly(); }

	void Renderer::SetClearColor(const glm::vec4& color)
	{ api->SetClearColor(color.r, color.g, color.b, color.a); }
	void Renderer::SetClearColor(float red, float green, float blue, float alpha)
	{ api->SetClearColor(red, green, blue, alpha); }

	void Renderer::DrawIndexed(const Ref<VertexArray>& vertexArray, const Ref<IndexBuffer>& indexBuffer, uint32_t offset, uint32_t count, RendererPrimitive primitive)
	{ api->DrawIndexed(vertexArray, indexBuffer, offset, count, primitive); }

	void Renderer::UploadCameraBuffer(const CameraBuffer& cameraBuffer)
	{
		data.cameraUniformBuffer->SetData(&cameraBuffer, sizeof(Renderer::CameraBuffer));
	}
}

#include "gbcpch.h"
#include "Renderer2D.h"
#include "GBC/Rendering/Renderer.h"
#include "GBC/Rendering/RendererCapabilities.h"
#include "GBC/Rendering/Shader.h"

namespace gbc
{
	struct QuadVertex
	{
		glm::vec3 position;
		glm::vec4 tintColor;
		glm::vec2 texCoord;
		uint32_t texIndex;
		glm::vec2 tilingFactor;
	};

	struct TextVertex
	{
		glm::vec2 position;
		glm::vec4 tintColor;
		glm::vec2 texCoord;
		uint32_t texIndex;
		Renderer2D::CurveInfo curveInfo;
	};

	struct CircleVertex
	{
		glm::vec3 position;
		glm::vec2 localPosition;
		float thickness;
		glm::vec4 color;
	};

	struct Renderer2DData
	{
		// Quads
		uint32_t quadCount = 0;

		Ref<Shader> quadShader;
		Ref<VertexArray> quadVertexArray;
		Ref<VertexBuffer> quadVertexBuffer;
		Ref<IndexBuffer> quadIndexBuffer;

		QuadVertex* localQuadVertexBufferStart = nullptr;
		QuadVertex* localQuadVertexBufferCurrent = nullptr;

		Ref<Texture2D>* quadTextures = nullptr;
		uint32_t quadTextureCount = 1;

		// Text
		uint32_t characterCount = 0;

		Ref<Shader> textShader;
		Ref<VertexArray> textVertexArray;
		Ref<VertexBuffer> textVertexBuffer;

		TextVertex* localTextVertexBufferStart = nullptr;
		TextVertex* localTextVertexBufferCurrent = nullptr;

		Ref<Texture2D>* textTextures = nullptr;
		uint32_t textTextureCount = 0;

		// Circles
		uint32_t circleCount = 0;

		Ref<Shader> circleShader;
		Ref<VertexArray> circleVertexArray;
		Ref<VertexBuffer> circleVertexBuffer;

		CircleVertex* localCircleVertexBufferStart = nullptr;
		CircleVertex* localCircleVertexBufferCurrent = nullptr;

		// Quad Data
		static constexpr uint32_t maxQuads = 65536;
		static constexpr uint32_t maxQuadVertices = maxQuads * 4;
		static constexpr uint32_t maxQuadIndices = maxQuads * 6;

		static constexpr uint32_t quadVertexCount = 4;
		static constexpr glm::vec4 quadVertexPositions[quadVertexCount]
		{
			{ -0.5f, -0.5f, 0.0f, 1.0f },
			{  0.5f, -0.5f, 0.0f, 1.0f },
			{  0.5f,  0.5f, 0.0f, 1.0f },
			{ -0.5f,  0.5f, 0.0f, 1.0f }
		};

		// Textures
		uint32_t maxTextures = 0;

#if GBC_ENABLE_STATS
		Renderer2D::Statistics statistics;
#endif
	};
	static Renderer2DData data;

	void Renderer2D::Init()
	{
		GBC_PROFILE_FUNCTION();

		// TODO: this shouldn't go here
		Renderer::DisableDepthTest();
		Renderer::EnableCullFace();
		Renderer::EnableBlending();

		// Setup vertex buffers
		data.localQuadVertexBufferStart = new QuadVertex[data.maxQuadVertices];
		data.localQuadVertexBufferCurrent = data.localQuadVertexBufferStart;
		data.quadVertexBuffer = VertexBuffer::Create(data.maxQuadVertices * sizeof(QuadVertex), nullptr, BufferUsage::DynamicDraw);
		data.quadVertexBuffer->SetLayout({
			{ VertexBufferElementType::Float3, "position"     },
			{ VertexBufferElementType::Float4, "tintColor"    },
			{ VertexBufferElementType::Float2, "texCoord"     },
			{ VertexBufferElementType::UInt,   "texIndex"     },
			{ VertexBufferElementType::Float2, "tilingFactor" }
		});

		data.localTextVertexBufferStart = new TextVertex[data.maxQuadVertices];
		data.localTextVertexBufferCurrent = data.localTextVertexBufferStart;
		data.textVertexBuffer = VertexBuffer::Create(data.maxQuadVertices * sizeof(TextVertex), nullptr, BufferUsage::DynamicDraw);
		data.textVertexBuffer->SetLayout({
			{ VertexBufferElementType::Float2, "position"  },
			{ VertexBufferElementType::Float4, "tintColor" },
			{ VertexBufferElementType::Float2, "texCoord"  },
			{ VertexBufferElementType::UInt,   "texIndex"  },
			{ VertexBufferElementType::Float2, "curveInfo" }
		});

		data.localCircleVertexBufferStart = new CircleVertex[data.maxQuadVertices];
		data.localCircleVertexBufferCurrent = data.localCircleVertexBufferStart;
		data.circleVertexBuffer = VertexBuffer::Create(data.maxQuadVertices * sizeof(CircleVertex), nullptr, BufferUsage::DynamicDraw);
		data.circleVertexBuffer->SetLayout({
			{ VertexBufferElementType::Float3, "position"      },
			{ VertexBufferElementType::Float2, "localPosition" },
			{ VertexBufferElementType::Float,  "thickness"     },
			{ VertexBufferElementType::Float4, "color"         }
		});

		// Setup vertex arrays
		data.quadVertexArray = VertexArray::Create();
		data.quadVertexArray->AddVertexBuffer(data.quadVertexBuffer);
		data.textVertexArray = VertexArray::Create();
		data.textVertexArray->AddVertexBuffer(data.textVertexBuffer);
		data.circleVertexArray = VertexArray::Create();
		data.circleVertexArray->AddVertexBuffer(data.circleVertexBuffer);

		// Setup quad index buffer
		uint32_t* indices = new uint32_t[data.maxQuadIndices];
		for (uint32_t index = 0, offset = 0; index < data.maxQuadIndices; index += 6, offset += 4)
		{
			indices[index + 0] = offset + 0;
			indices[index + 1] = offset + 1;
			indices[index + 2] = offset + 2;
			indices[index + 3] = offset + 2;
			indices[index + 4] = offset + 3;
			indices[index + 5] = offset + 0;
		}
		data.quadIndexBuffer = IndexBuffer::Create(data.maxQuadIndices, indices, BufferUsage::StaticDraw, IndexBufferElementType::UInt32);
		delete[] indices;

		// Setup shaders
		data.quadShader = Shader::Create("Resources/Shaders/Renderer2DQuad.glsl");
		data.textShader = Shader::Create("Resources/Shaders/Renderer2DText.glsl");
		data.circleShader = Shader::Create("Resources/Shaders/Renderer2DCircle.glsl");

		// Setup texture slots
		data.maxTextures = static_cast<uint32_t>(RendererCapabilities::GetMaxTextureSlots());
		data.quadTextures = new Ref<Texture2D>[data.maxTextures];
		data.textTextures = new Ref<Texture2D>[data.maxTextures];

		// Setup white texture
		auto localWhiteTexture = LocalTexture2D::Create(1, 1, 4);
		*(uint32_t*)localWhiteTexture->GetData() = 0xFFFFFFFF;
		data.quadTextures[0] = Texture2D::Create(localWhiteTexture);
	}

	void Renderer2D::Shutdown()
	{
		GBC_PROFILE_FUNCTION();

		delete[] data.localQuadVertexBufferStart;
		delete[] data.localTextVertexBufferStart;
		delete[] data.localCircleVertexBufferStart;

		delete[] data.quadTextures;
		delete[] data.textTextures;

		// Make sure to free GPU memory
		data.quadShader.reset();
		data.quadVertexArray.reset();
		data.quadVertexBuffer.reset();
		data.quadIndexBuffer.reset();

		data.textShader.reset();
		data.textVertexArray.reset();
		data.textVertexBuffer.reset();

		data.circleShader.reset();
		data.circleVertexArray.reset();
		data.circleVertexBuffer.reset();
	}

	void Renderer2D::BeginScene(const glm::mat4& viewProjection)
	{
		GBC_PROFILE_FUNCTION();

		Renderer::CameraBuffer cameraBuffer;
		cameraBuffer.viewProjection = viewProjection;
		Renderer::UploadCameraBuffer(cameraBuffer);
	}

	void Renderer2D::EndScene()
	{
		Flush();
	}

	void Renderer2D::Flush()
	{
		GBC_PROFILE_FUNCTION();

		FlushQuads();
		FlushText();
		FlushCircles();
		ResetQuads();
		ResetText();
		ResetCircles();
	}

	void Renderer2D::FlushQuads()
	{
		uint32_t quadVertexBufferSize = static_cast<uint32_t>((data.quadCount * (4 * sizeof(QuadVertex))));
		if (quadVertexBufferSize)
		{
			// Copy quad vertices to GPU
			data.quadVertexBuffer->SetData(data.localQuadVertexBufferStart, quadVertexBufferSize);

			// Bind textures
			for (uint32_t i = 0; i < data.quadTextureCount; i++)
				data.quadTextures[i]->Bind(i);

			// Actually render
			data.quadShader->Bind();
			Renderer::DrawIndexed(data.quadVertexArray, data.quadIndexBuffer, 0, data.quadCount * 6);

#if GBC_ENABLE_STATS
			data.statistics.drawCallCount++;
#endif
		}
	}

	void Renderer2D::FlushText()
	{
		uint32_t textVertexBufferSize = static_cast<uint32_t>((data.characterCount * (4 * sizeof(TextVertex))));
		if (textVertexBufferSize)
		{
			// Copy text vertices to GPU
			data.textVertexBuffer->SetData(data.localTextVertexBufferStart, textVertexBufferSize);

			// Bind textures
			for (uint32_t i = 0; i < data.textTextureCount; i++)
				data.textTextures[i]->Bind(i);

			// Actually render
			data.textShader->Bind();
			// Yes, it is intentional to use the quad index buffer here because all the indices will be the
			// same anyway, so there's no point in creating the same data twice for both the quads and text.
			Renderer::DrawIndexed(data.textVertexArray, data.quadIndexBuffer, 0, data.characterCount * 6);

#if GBC_ENABLE_STATS
			data.statistics.drawCallCount++;
#endif
		}
	}

	void Renderer2D::FlushCircles()
	{
		uint32_t circleVertexBufferSize = static_cast<uint32_t>((data.circleCount * (4 * sizeof(CircleVertex))));
		if (circleVertexBufferSize)
		{
			// Copy circle vertices to GPU
			data.circleVertexBuffer->SetData(data.localCircleVertexBufferStart, circleVertexBufferSize);

			// Actually render
			data.circleShader->Bind();
			// Yes, it is intentional to use the quad index buffer here because all the indices will be the
			// same anyway, so there's no point in creating the same data twice for both the quads and circles.
			Renderer::DrawIndexed(data.circleVertexArray, data.quadIndexBuffer, 0, data.circleCount * 6);

#if GBC_ENABLE_STATS
			data.statistics.drawCallCount++;
#endif
		}
	}

	void Renderer2D::ResetQuads()
	{
		data.quadCount = 0;
		data.localQuadVertexBufferCurrent = data.localQuadVertexBufferStart;

		// Remove the references once the textures has been rendered
		for (uint32_t i = 1; i < data.quadTextureCount; i++)
			data.quadTextures[i] = nullptr;
		data.quadTextureCount = 1;
	}

	void Renderer2D::ResetText()
	{
		data.characterCount = 0;
		data.localTextVertexBufferCurrent = data.localTextVertexBufferStart;

		// Remove the references once the textures has been rendered
		for (uint32_t i = 0; i < data.textTextureCount; i++)
			data.textTextures[i] = nullptr;
		data.textTextureCount = 0;
	}

	void Renderer2D::ResetCircles()
	{
		data.circleCount = 0;
		data.localCircleVertexBufferCurrent = data.localCircleVertexBufferStart;
	}

	uint32_t Renderer2D::GetQuadTexIndex(const Ref<Texture2D>& texture)
	{
		// If texture is null or it has a null local texture, then use white texture
		if (!texture || (!texture->IsFramebufferTexture() && (!texture->GetLocalTexture() || !*texture->GetLocalTexture())))
			return 0;

		for (uint32_t i = 1; i < data.quadTextureCount; i++)
			if (*data.quadTextures[i]->GetLocalTexture() == *texture->GetLocalTexture())
				return i;

		return data.quadTextureCount;
	}

	uint32_t Renderer2D::GetTextTexIndex(const Ref<Texture2D>& texture)
	{
		// If texture is null or it has a null local texture, then use white texture
		if (!texture || !texture->GetLocalTexture() || !*texture->GetLocalTexture())
			return -1;

		for (uint32_t i = 0; i < data.textTextureCount; i++)
			if (*data.textTextures[i]->GetLocalTexture() == *texture->GetLocalTexture())
				return i;

		return data.textTextureCount;
	}

	void Renderer2D::EnsureQuadBatch(uint32_t quadCount, uint32_t texIndex)
	{
		if (data.quadCount + quadCount >= data.maxQuads || texIndex >= data.maxTextures)
		{
			FlushQuads();
			ResetQuads();
		}
	}

	void Renderer2D::EnsureTextBatch(uint32_t characterCount, uint32_t texIndex)
	{
		if (data.characterCount + characterCount >= data.maxQuads || texIndex >= data.maxTextures)
		{
			FlushText();
			ResetText();
		}
	}

	void Renderer2D::EnsureCircleBatch()
	{
		if (data.circleCount >= data.maxQuads)
		{
			FlushCircles();
			ResetCircles();
		}
	}

	void Renderer2D::DrawQuad(const glm::mat4& transform, const glm::vec4& color, const Ref<Texture2D>& texture, const glm::vec2& tilingFactor, const std::array<glm::vec2, 4>& texCoords)
	{
		// Handle texture
		uint32_t texIndex = GetQuadTexIndex(texture);
		EnsureQuadBatch(1, texIndex);
		if (texIndex >= data.quadTextureCount)
		{
			texIndex = data.quadTextureCount;
			data.quadTextures[texIndex] = texture;
			data.quadTextureCount++;
#if GBC_ENABLE_STATS
			data.statistics.textureCount++;
#endif
		}

		// Handle vertices
		if (RendererAPI::GetAPI() == RendererAPI::API::OpenGL)
		{
			for (uint32_t i = 0; i < data.quadVertexCount; i++, data.localQuadVertexBufferCurrent++)
			{
				data.localQuadVertexBufferCurrent->position = transform * data.quadVertexPositions[i];
				data.localQuadVertexBufferCurrent->tintColor = color;
				glm::vec2 texCoord = texCoords[i];
				texCoord.y = 1.0f - texCoord.y;
				data.localQuadVertexBufferCurrent->texCoord = texCoord;
				data.localQuadVertexBufferCurrent->texIndex = texIndex;
				data.localQuadVertexBufferCurrent->tilingFactor = tilingFactor;
			}
		}
		else
		{
			for (uint32_t i = 0; i < data.quadVertexCount; i++, data.localQuadVertexBufferCurrent++)
			{
				data.localQuadVertexBufferCurrent->position = transform * data.quadVertexPositions[i];
				data.localQuadVertexBufferCurrent->tintColor = color;
				data.localQuadVertexBufferCurrent->texCoord = texCoords[i];
				data.localQuadVertexBufferCurrent->texIndex = texIndex;
				data.localQuadVertexBufferCurrent->tilingFactor = tilingFactor;
			}
		}

		// Update counts
		data.quadCount++;
#if GBC_ENABLE_STATS
		data.statistics.quadCount++;
#endif
	}

	void Renderer2D::DrawString(std::string_view string, const Ref<Font>& font, const CurveInfo& curveInfo, const glm::mat4& transform, const glm::vec4& color)
	{
		if (string.empty() || !font)
			return;

		// Handle texture
		Ref<Texture2D> texture = font->GetTexture();
		uint32_t texIndex = GetTextTexIndex(texture);
		if (texIndex == -1)
		{
			// Attempt to force load the font texture
			for (int32_t codepoint : string)
				if (font->GetGlyph(codepoint))
					break;
			texture = font->GetTexture();
			texIndex = GetTextTexIndex(texture);

			// For whatever reason, the font texture was unable to load, so don't render anything.
			if (texIndex == -1)
				return;
		}

		uint32_t characterCount = static_cast<uint32_t>(string.size());
		EnsureTextBatch(characterCount, texIndex);
		if (texIndex >= data.textTextureCount)
		{
			texIndex = data.textTextureCount;
			data.textTextures[texIndex] = texture;
			data.textTextureCount++;
#if GBC_ENABLE_STATS
			data.statistics.textureCount++;
#endif
		}

		// Make sure all the glyphs are loaded before getting the texture size.
		// Otherwise, texture coordinates will be wrong on the first frame.
		for (int32_t codepoint : string)
			(void)font->GetGlyph(codepoint);

		// TODO: these are good for unscaled arial...
		float scale = font->GetScale();
		CurveInfo actualCurveInfo
		{
			curveInfo.width * scale,
			curveInfo.fade * scale
		};

		glm::vec2 textureSize = font->GetTextureSize();
		glm::vec2 cursor{ 0.0f };
		int32_t lastCodepoint = 0;

		for (int32_t codepoint : string)
		{
			const Glyph* glyph = font->GetGlyph(codepoint);
			if (!glyph) continue;

			float advance = glyph->GetAdvance();
			auto& bearing = glyph->GetBearing();
			auto& texRect = glyph->GetTextureRect();

			int32_t kerning = 0;
			if (lastCodepoint)
				kerning = font->GetKerning(lastCodepoint, codepoint);

			glm::vec2 posLeftTop{ cursor.x + bearing.x + kerning, cursor.y - texRect.w + bearing.y };
			glm::vec2 posRightBottom = posLeftTop + glm::vec2(texRect.z, texRect.w);
			glm::vec2 vertexPositions[4]
			{
				posLeftTop,
				glm::vec2(posRightBottom.x, posLeftTop.y),
				posRightBottom,
				glm::vec2(posLeftTop.x, posRightBottom.y)
			};

			glm::vec2 texLeftTop = glm::vec2(texRect.x, texRect.y) / textureSize;
			glm::vec2 texRightBottom = texLeftTop + glm::vec2(texRect.z, texRect.w) / textureSize;
			glm::vec2 texCoords[4]
			{
				texLeftTop,
				glm::vec2(texRightBottom.x, texLeftTop.y),
				texRightBottom,
				glm::vec2(texLeftTop.x, texRightBottom.y)
			};

			if (RendererAPI::GetAPI() == RendererAPI::API::OpenGL)
			{
				std::swap(texCoords[0], texCoords[3]);
				std::swap(texCoords[1], texCoords[2]);
			}

			// Handle vertices
			for (uint32_t i = 0; i < data.quadVertexCount; i++, data.localTextVertexBufferCurrent++)
			{
				data.localTextVertexBufferCurrent->position = transform * glm::vec4(vertexPositions[i], 0.0f, 1.0f);
				data.localTextVertexBufferCurrent->tintColor = color;
				data.localTextVertexBufferCurrent->texCoord = texCoords[i];
				data.localTextVertexBufferCurrent->texIndex = texIndex;
				data.localTextVertexBufferCurrent->curveInfo = curveInfo;
			}

			cursor.x += advance;
			lastCodepoint = codepoint;
		}

		// Update counts
		data.characterCount += characterCount;
#if GBC_ENABLE_STATS
		data.statistics.quadCount += characterCount;
#endif
	}

	void Renderer2D::DrawCircle(const glm::mat4& transform, float thickness, const glm::vec4& color)
	{
		// The shader can, even if thickness is exactly zero, render pixels at the very edge of the circle.
		if (thickness == 0.0f)
			return;

		EnsureCircleBatch();

		// Handle vertices
		// Again, using the quad vertices here is intentional
		for (uint32_t i = 0; i < data.quadVertexCount; i++, data.localCircleVertexBufferCurrent++)
		{
			data.localCircleVertexBufferCurrent->position = transform * data.quadVertexPositions[i];
			data.localCircleVertexBufferCurrent->localPosition = data.quadVertexPositions[i] * 2.0f;
			data.localCircleVertexBufferCurrent->thickness = thickness;
			data.localCircleVertexBufferCurrent->color = color;
		}

		// Update counts
		data.circleCount++;
#if GBC_ENABLE_STATS
		data.statistics.circleCount++;
#endif
	}

	glm::mat4 Renderer2D::GetTransform(const glm::vec3& translation)
	{
		return glm::translate(glm::mat4(1.0f), translation);
	}

	glm::mat4 Renderer2D::GetTransform(const glm::vec3& translation, float rotation)
	{
		return glm::translate(glm::mat4(1.0f), translation) *
			   glm::rotate(glm::mat4(1.0f), rotation, glm::vec3(0.0f, 0.0f, 1.0f));
	}

	glm::mat4 Renderer2D::GetTransform(const glm::vec3& translation, const glm::vec2& scale)
	{
		return glm::translate(glm::mat4(1.0f), translation) *
			   glm::scale(glm::mat4(1.0f), glm::vec3(scale, 1.0f));
	}

	glm::mat4 Renderer2D::GetTransform(const glm::vec3& translation, float rotation, const glm::vec2& scale)
	{
		return glm::translate(glm::mat4(1.0f), translation) *
			   glm::rotate(glm::mat4(1.0f), rotation, glm::vec3(0.0f, 0.0f, 1.0f)) *
			   glm::scale(glm::mat4(1.0f), glm::vec3(scale, 1.0f));
	}

#if GBC_ENABLE_STATS
	const Renderer2D::Statistics& Renderer2D::GetStatistics()
	{
		return data.statistics;
	}

	void Renderer2D::ResetStatistics()
	{
		std::memset(&data.statistics, 0, sizeof(data.statistics));
	}
#endif
}

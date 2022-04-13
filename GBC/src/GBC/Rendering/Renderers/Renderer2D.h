#pragma once

#include "GBC/Rendering/Camera.h"
#include "GBC/Rendering/Texture.h"
#include "GBC/Rendering/Font/Font.h"

namespace gbc
{
	// This renderer is capable of rendering the following:
	//	quads with different translations, rotations, scales, colors, textures, and tiling factors
	//	circles with different translations, scales, thicknesses, and colors
	//	text with different fonts, translations, rotations, scales, and colors
	class Renderer2D
	{
	public:
		static void BeginScene(const glm::mat4& viewProjection);
		static void EndScene();
		static void Flush();
	public:
		static inline void DrawQuad(const glm::vec3& translation, const glm::vec4& color = glm::vec4(1.0f), const Ref<Texture2D>& texture = nullptr, const glm::vec2& tilingFactor = glm::vec2(1.0f), const std::array<glm::vec2, 4>& texCoords = defaultTexCoords()) { DrawQuad(GetTransform(translation), color, texture, tilingFactor, texCoords); }
		static inline void DrawQuad(const glm::vec3& translation, float rotation, const glm::vec4& color = glm::vec4(1.0f), const Ref<Texture2D>& texture = nullptr, const glm::vec2& tilingFactor = glm::vec2(1.0f), const std::array<glm::vec2, 4>& texCoords = defaultTexCoords()) { DrawQuad(GetTransform(translation, rotation), color, texture, tilingFactor, texCoords); }
		static inline void DrawQuad(const glm::vec3& translation, const glm::vec2& scale, const glm::vec4& color = glm::vec4(1.0f), const Ref<Texture2D>& texture = nullptr, const glm::vec2& tilingFactor = glm::vec2(1.0f), const std::array<glm::vec2, 4>& texCoords = defaultTexCoords()) { DrawQuad(GetTransform(translation, scale), color, texture, tilingFactor, texCoords); }
		static inline void DrawQuad(const glm::vec3& translation, float rotation, const glm::vec2& scale, const glm::vec4& color = glm::vec4(1.0f), const Ref<Texture2D>& texture = nullptr, const glm::vec2& tilingFactor = glm::vec2(1.0f), const std::array<glm::vec2, 4>& texCoords = defaultTexCoords()) { DrawQuad(GetTransform(translation, rotation, scale), color, texture, tilingFactor, texCoords); }

		static inline void DrawQuad(const glm::vec2& translation, const glm::vec4& color = glm::vec4(1.0f), const Ref<Texture2D>& texture = nullptr, const glm::vec2& tilingFactor = glm::vec2(1.0f), const std::array<glm::vec2, 4>& texCoords = defaultTexCoords()) { DrawQuad(glm::vec3(translation, 0.0f), color, texture, tilingFactor, texCoords); }
		static inline void DrawQuad(const glm::vec2& translation, float rotation, const glm::vec4& color = glm::vec4(1.0f), const Ref<Texture2D>& texture = nullptr, const glm::vec2& tilingFactor = glm::vec2(1.0f), const std::array<glm::vec2, 4>& texCoords = defaultTexCoords()) { DrawQuad(glm::vec3(translation, 0.0f), rotation, color, texture, tilingFactor, texCoords); }
		static inline void DrawQuad(const glm::vec2& translation, const glm::vec2& scale, const glm::vec4& color = glm::vec4(1.0f), const Ref<Texture2D>& texture = nullptr, const glm::vec2& tilingFactor = glm::vec2(1.0f), const std::array<glm::vec2, 4>& texCoords = defaultTexCoords()) { DrawQuad(glm::vec3(translation, 0.0f), scale, color, texture, tilingFactor, texCoords); }
		static inline void DrawQuad(const glm::vec2& translation, float rotation, const glm::vec2& scale, const glm::vec4& color = glm::vec4(1.0f), const Ref<Texture2D>& texture = nullptr, const glm::vec2& tilingFactor = glm::vec2(1.0f), const std::array<glm::vec2, 4>& texCoords = defaultTexCoords()) { DrawQuad(glm::vec3(translation, 0.0f), rotation, scale, color, texture, tilingFactor, texCoords); }

		// Every DrawQuad feeds to here
		static void DrawQuad(const glm::mat4& transform, const glm::vec4& color = glm::vec4(1.0f), const Ref<Texture2D>& texture = nullptr, const glm::vec2& tilingFactor = glm::vec2(1.0f), const std::array<glm::vec2, 4>& texCoords = defaultTexCoords());
	public:
		struct CurveInfo
		{
			float width;
			float fade;
		};

		static inline void DrawString(std::string_view string, const Ref<Font>& font, const CurveInfo& curveInfo, const glm::vec3& translation, const glm::vec4& color = glm::vec4(1.0f)) { DrawString(string, font, curveInfo, GetTransform(translation, glm::vec2(1.0f / font->GetSize())), color); }
		static inline void DrawString(std::string_view string, const Ref<Font>& font, const CurveInfo& curveInfo, const glm::vec3& translation, float rotation, const glm::vec4& color = glm::vec4(1.0f)) { DrawString(string, font, curveInfo, GetTransform(translation, rotation, glm::vec2(1.0f / font->GetSize())), color); }
		static inline void DrawString(std::string_view string, const Ref<Font>& font, const CurveInfo& curveInfo, const glm::vec3& translation, const glm::vec2& scale, const glm::vec4& color = glm::vec4(1.0f)) { DrawString(string, font, curveInfo, GetTransform(translation, scale / font->GetSize()), color); }
		static inline void DrawString(std::string_view string, const Ref<Font>& font, const CurveInfo& curveInfo, const glm::vec3& translation, float rotation, const glm::vec2& scale, const glm::vec4& color = glm::vec4(1.0f)) { DrawString(string, font, curveInfo, GetTransform(translation, rotation, scale / font->GetSize()), color); }

		static inline void DrawString(std::string_view string, const Ref<Font>& font, const CurveInfo& curveInfo, const glm::vec2& translation, const glm::vec4& color = glm::vec4(1.0f)) { DrawString(string, font, curveInfo, glm::vec3(translation, 0.0f), color); }
		static inline void DrawString(std::string_view string, const Ref<Font>& font, const CurveInfo& curveInfo, const glm::vec2& translation, float rotation, const glm::vec4& color = glm::vec4(1.0f)) { DrawString(string, font, curveInfo, glm::vec3(translation, 0.0f), rotation, color); }
		static inline void DrawString(std::string_view string, const Ref<Font>& font, const CurveInfo& curveInfo, const glm::vec2& translation, const glm::vec2& scale, const glm::vec4& color = glm::vec4(1.0f)) { DrawString(string, font, curveInfo, glm::vec3(translation, 0.0f), scale, color); }
		static inline void DrawString(std::string_view string, const Ref<Font>& font, const CurveInfo& curveInfo, const glm::vec2& translation, float rotation, const glm::vec2& scale, const glm::vec4& color = glm::vec4(1.0f)) { DrawString(string, font, curveInfo, glm::vec3(translation, 0.0f), rotation, scale, color); }

		// Every DrawString feeds to here
		static void DrawString(std::string_view string, const Ref<Font>& font, const CurveInfo& curveInfo, const glm::mat4& transform, const glm::vec4& color = glm::vec4(1.0f));
	public:
		static inline void DrawCircle(const glm::vec3& translation, float thickness, const glm::vec4& color = glm::vec4(1.0f)) { DrawCircle(GetTransform(translation), thickness, color); }
		static inline void DrawCircle(const glm::vec3& translation, const glm::vec2& scale, float thickness, const glm::vec4& color = glm::vec4(1.0f)) { DrawCircle(GetTransform(translation, scale), thickness, color); }

		static inline void DrawCircle(const glm::vec2& translation, float thickness, const glm::vec4& color = glm::vec4(1.0f)) { DrawCircle(glm::vec3(translation, 0.0f), thickness, color); }
		static inline void DrawCircle(const glm::vec2& translation, const glm::vec2& scale, float thickness, const glm::vec4& color = glm::vec4(1.0f)) { DrawCircle(glm::vec3(translation, 0.0f), scale, thickness, color); }

		// Every DrawCircle feeds to here
		static void DrawCircle(const glm::mat4& transform, float thickness, const glm::vec4& color = glm::vec4(1.0f));
	private:
		static glm::mat4 GetTransform(const glm::vec3& translation);
		static glm::mat4 GetTransform(const glm::vec3& translation, float rotation);
		static glm::mat4 GetTransform(const glm::vec3& translation, const glm::vec2& scale);
		static glm::mat4 GetTransform(const glm::vec3& translation, float rotation, const glm::vec2& scale);
	private:
		static constexpr std::array<glm::vec2, 4> defaultTexCoords()
		{
			return
			{
				glm::vec2(0.0f, 1.0f),
				glm::vec2(1.0f, 1.0f),
				glm::vec2(1.0f, 0.0f),
				glm::vec2(0.0f, 0.0f)
			};
		}
	private:
		static void FlushQuads();
		static void FlushText();
		static void FlushCircles();
		static void ResetQuads();
		static void ResetText();
		static void ResetCircles();
	private:
		static uint32_t GetQuadTexIndex(const Ref<Texture2D>& texture);
		static uint32_t GetTextTexIndex(const Ref<Texture2D>& texture);
		static void EnsureQuadBatch(uint32_t quadCount, uint32_t texIndex);
		static void EnsureTextBatch(uint32_t characterCount, uint32_t texIndex);
		static void EnsureCircleBatch();
	private:
		friend class Renderer;
		static void Init();
		static void Shutdown();
#if GBC_ENABLE_STATS
	public:
		struct Statistics
		{
		public:
			constexpr uint32_t GetDrawCallCount() const noexcept { return drawCallCount; }
			constexpr uint32_t GetQuadCount() const noexcept { return quadCount; }
			constexpr uint32_t GetCircleCount() const noexcept { return circleCount; }
			constexpr uint32_t GetTextureCount() const noexcept { return textureCount; }
			constexpr uint32_t GetVertexCount() const noexcept { return (quadCount + circleCount) * 4; }
			constexpr uint32_t GetIndexCount() const noexcept { return (quadCount + circleCount) * 6; }
		private:
			friend class Renderer2D;
			uint32_t drawCallCount = 0;
			uint32_t quadCount = 0;
			uint32_t circleCount = 0;
			uint32_t textureCount = 0;
		};

		static const Statistics& GetStatistics();
		static void ResetStatistics();
#endif
	};
}

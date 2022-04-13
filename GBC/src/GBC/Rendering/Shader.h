#pragma once

#include "GBC/Core/Core.h"
#include <glm/glm.hpp>
#include <cstdint>
#include <filesystem>
#include <string>

namespace gbc
{
	class Shader
	{
	public:
		virtual ~Shader() = default;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void SetFloat  (const std::string& name, float              value) = 0;
		virtual void SetFloat2 (const std::string& name, const glm::vec2&   value) = 0;
		virtual void SetFloat3 (const std::string& name, const glm::vec3&   value) = 0;
		virtual void SetFloat4 (const std::string& name, const glm::vec4&   value) = 0;
		virtual void SetInt    (const std::string& name, int32_t            value) = 0;
		virtual void SetInt2   (const std::string& name, const glm::ivec2&  value) = 0;
		virtual void SetInt3   (const std::string& name, const glm::ivec3&  value) = 0;
		virtual void SetInt4   (const std::string& name, const glm::ivec4&  value) = 0;
		virtual void SetUInt   (const std::string& name, uint32_t           value) = 0;
		virtual void SetUInt2  (const std::string& name, const glm::uvec2&  value) = 0;
		virtual void SetUInt3  (const std::string& name, const glm::uvec3&  value) = 0;
		virtual void SetUInt4  (const std::string& name, const glm::uvec4&  value) = 0;
		virtual void SetBool   (const std::string& name, bool               value) = 0;
		virtual void SetBool2  (const std::string& name, const glm::bvec2&  value) = 0;
		virtual void SetBool3  (const std::string& name, const glm::bvec3&  value) = 0;
		virtual void SetBool4  (const std::string& name, const glm::bvec4&  value) = 0;
		virtual void SetMat2   (const std::string& name, const glm::mat2&   value) = 0;
		virtual void SetMat2x3 (const std::string& name, const glm::mat2x3& value) = 0;
		virtual void SetMat2x4 (const std::string& name, const glm::mat2x4& value) = 0;
		virtual void SetMat3x2 (const std::string& name, const glm::mat3x2& value) = 0;
		virtual void SetMat3   (const std::string& name, const glm::mat3&   value) = 0;
		virtual void SetMat3x4 (const std::string& name, const glm::mat3x4& value) = 0;
		virtual void SetMat4x2 (const std::string& name, const glm::mat4x2& value) = 0;
		virtual void SetMat4x3 (const std::string& name, const glm::mat4x3& value) = 0;
		virtual void SetMat4   (const std::string& name, const glm::mat4&   value) = 0;
		virtual void SetInts   (const std::string& name, const int32_t*     values, int32_t count) = 0;
		virtual void SetUInts  (const std::string& name, const uint32_t*    values, int32_t count) = 0;

		static Ref<Shader> Create(const std::filesystem::path& filepath);
	};
}

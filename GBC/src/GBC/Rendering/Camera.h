#pragma once

#include <glm/glm.hpp>

namespace gbc
{
	class Camera
	{
	public:
		constexpr Camera() = default;
		constexpr Camera(const glm::mat4& projection) : projection(projection) {}
		virtual ~Camera() = default;

		virtual void OnViewportResize(int32_t width, int32_t height) {}

		constexpr const glm::mat4& GetProjection() const noexcept { return projection; }
	protected:
		glm::mat4 projection{ 1.0f };
	};
}

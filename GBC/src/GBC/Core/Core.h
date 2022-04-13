#pragma once

#include "GBC/Core/Debugbreak.h"

#if GBC_CONFIG_DEBUG
	#define GBC_ENABLE_ASSERTS 1
	#define GBC_ENABLE_LOGGING 1
	#define GBC_ENABLE_IMGUI 1
	#define GBC_ENABLE_STATS 1
	#define GBC_ENABLE_PROFILE_RUNTIME 1
	#define GBC_ENABLE_PROFILE 0
#elif GBC_CONFIG_RELEASE
	#define GBC_ENABLE_ASSERTS 0
	#define GBC_ENABLE_LOGGING 1
	#define GBC_ENABLE_IMGUI 1
	#define GBC_ENABLE_STATS 1
	#define GBC_ENABLE_PROFILE_RUNTIME 0
	#define GBC_ENABLE_PROFILE 0
#elif GBC_CONFIG_DIST
	#define GBC_ENABLE_ASSERTS 0
	#define GBC_ENABLE_LOGGING 0
	#define GBC_ENABLE_IMGUI 1
	#define GBC_ENABLE_STATS 0
	#define GBC_ENABLE_PROFILE_RUNTIME 0
	#define GBC_ENABLE_PROFILE 0
#endif

#define GBC_EXPAND_MACRO(x) x
#define GBC_STRINGIFY_MACRO_IMPL(x) #x
#define GBC_STRINGIFY_MACRO(x) GBC_EXPAND_MACRO(GBC_STRINGIFY_MACRO_IMPL(x))

#define GBC_BIND_FUNC(func) [this](auto&&... args) -> decltype(auto) { return this->func(std::forward<decltype(args)>(args)...); }

#include <memory>
#include <cstdint>

namespace gbc
{
	// Ref and Scope
	template<typename T>
	using Ref = std::shared_ptr<T>;
	template<typename T, typename... Args>
	constexpr Ref<T> CreateRef(Args&&... args)
	{ return std::make_shared<T>(std::forward<Args>(args)...); }
	template<typename T>
	constexpr Ref<T> CreateRef(T* t)
	{ return Ref<T>(t); }

	template<typename T>
	using Scope = std::unique_ptr<T>;
	template<typename T, typename... Args>
	constexpr Scope<T> CreateScope(Args&&... args)
	{ return std::make_unique<T>(std::forward<Args>(args)...); }
	template<typename T>
	constexpr Scope<T> CreateScope(T* t)
	{ return Scope<T>(t); }

	// RendererID
	using RendererID = uint32_t;
}

#include "GBC/Core/Assert.h"
#include "GBC/Core/Logger.h"

#pragma once
#pragma warning(disable : 4996)

#include "PlatformDetection.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#ifdef E_DEBUG
	#if defined(E_PLATFORM_WINDOWS)
		#define E_DEBUGBREAK() __debugbreak()
	#elif defined(E_PLATFORM_LINUX)
		#include <signal.h>
		#define E_DEBUGBREAK() raise(SIGTRAP)
	#else
		#error "Platform doesn't support debugbreak yet!"
	#endif
	#define E_ENABLE_ASSERTS
#else
	#define E_DEBUGBREAK()
#endif

#ifdef E_PLATFORM_WINDOWS
    #ifndef NOMINMAX
    # define NOMINMAX
    #endif
    #include <Windows.h>
#endif

namespace Engine {

    //--------------------- Scope = unique pointer --------------------
    template<typename T>
    using Scope = std::unique_ptr<T>;
    template<typename T, typename ... Args>
    constexpr Scope<T> CreateScope(Args&& ... args){

        return std::make_unique<T>(std::forward<Args>(args)...);
    }

    //--------------------- Ref = shared pointer -----------------------
    template<typename T>
    using Ref = std::shared_ptr<T>;
    template<typename T, typename ... Args>
    constexpr Ref<T> CreateRef(Args&& ... args){

        return std::make_shared<T>(std::forward<Args>(args)...);
    }
}

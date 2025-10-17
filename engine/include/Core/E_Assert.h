#pragma once

#include "Types.h"
#include "Log.h"
#include <filesystem>

#ifdef E_ENABLE_ASSERTS

	// Alteratively we could use the same "default" message for both "WITH_MSG" and "NO_MSG" and
	// provide support for custom formatting by concatenating the formatting string instead of having the format inside the default message
	#define E_INTERNAL_ASSERT_IMPL(type, check, msg, ...) { if(!(check)) { E##type##ERROR(msg, __VA_ARGS__); E_DEBUGBREAK(); } }
	#define E_INTERNAL_ASSERT_WITH_MSG(type, check, ...) E_INTERNAL_ASSERT_IMPL(type, check, "Assertion failed: {0}", __VA_ARGS__)
	#define E_INTERNAL_ASSERT_NO_MSG(type, check) E_INTERNAL_ASSERT_IMPL(type, check, "Assertion '{0}' failed at {1}:{2}", E_STRINGIFY_MACRO(check), std::filesystem::path(__FILE__).filename().string(), __LINE__)

	#define E_INTERNAL_ASSERT_GET_MACRO_NAME(arg1, arg2, macro, ...) macro
	#define E_INTERNAL_ASSERT_GET_MACRO(...) E_EXPAND_MACRO( E_INTERNAL_ASSERT_GET_MACRO_NAME(__VA_ARGS__, E_INTERNAL_ASSERT_WITH_MSG, E_INTERNAL_ASSERT_NO_MSG) )

	// Currently accepts at least the condition and one additional parameter (the message) being optional
	#define E_ASSERT(...) E_EXPAND_MACRO( E_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_, __VA_ARGS__) )
	#define E_CORE_ASSERT(...) E_EXPAND_MACRO( E_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_CORE_, __VA_ARGS__) )
#else
	#define E_ASSERT(...)
	#define E_CORE_ASSERT(...)
#endif
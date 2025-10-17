#pragma once

#include "Types.h"

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/string_cast.hpp"

// This ignores all warnings raised inside External headers
#pragma warning(push, 0)
#include <spdlog/fmt/bundled/format.h>
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
#pragma warning(pop)

namespace Engine {

    class  Log{
    public:
        static void Init();
        
        static Ref<spdlog::logger>& GetCoreLogger() { return s_CoreLogger;}
        static Ref<spdlog::logger>& GetClientLogger() { return s_ClientLogger;}
    private:
        static Ref<spdlog::logger> s_CoreLogger;
        static Ref<spdlog::logger> s_ClientLogger;
    };
}

template<typename OStream, glm::length_t L, typename T, glm::qualifier Q>
inline OStream& operator<<(OStream& os, const glm::vec<L, T, Q>& vector)
{
	return os << glm::to_string(vector);
}

template<typename OStream, glm::length_t C, glm::length_t R, typename T, glm::qualifier Q>
inline OStream& operator<<(OStream& os, const glm::mat<C, R, T, Q>& matrix)
{
	return os << glm::to_string(matrix);
}

template<typename OStream, typename T, glm::qualifier Q>
inline OStream& operator<<(OStream& os, glm::qua<T, Q> quaternio)
{
	return os << glm::to_string(quaternio);
}

inline std::ostream& operator<<(std::ostream& os, const glm::vec2& v) {
    return os << '(' << v.x << ", " << v.y << ')';
}
inline std::ostream& operator<<(std::ostream& os, const glm::vec3& v) {
    return os << '(' << v.x << ", " << v.y << ", " << v.z << ')';
}
inline std::ostream& operator<<(std::ostream& os, const glm::vec4& v) {
    return os << '(' << v.x << ", " << v.y << ", " << v.z << ", " << v.w << ')';
}

template<> struct fmt::formatter<glm::vec3> {
    constexpr auto parse(fmt::format_parse_context& ctx) { return ctx.begin(); }
    template <class Ctx>
    auto format(const glm::vec3& v, Ctx& ctx) const {
        return fmt::format_to(ctx.out(), "({}, {}, {})", v.x, v.y, v.z);
    }
};

template<glm::length_t L, typename T, glm::qualifier Q>
struct fmt::formatter<glm::vec<L,T,Q>> {
    constexpr auto parse(fmt::format_parse_context& ctx) { return ctx.begin(); }
    template <typename Ctx>
    auto format(const glm::vec<L,T,Q>& v, Ctx& ctx) const {
        auto out = ctx.out();
        *out++ = '(';
        for (glm::length_t i = 0; i < L; ++i) {
            out = fmt::format_to(out, "{}", v[i]);
            if (i + 1 < L) out = fmt::format_to(out, ", ");
        }
        *out++ = ')';
        return out;
    }
};

// Core log macros
#define E_CORE_TRACE(...)    ::Engine::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define E_CORE_INFO(...)     ::Engine::Log::GetCoreLogger()->info(__VA_ARGS__)
#define E_CORE_WARN(...)     ::Engine::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define E_CORE_ERROR(...)    ::Engine::Log::GetCoreLogger()->error(__VA_ARGS__)
#define E_CORE_CRITICAL(...) ::Engine::Log::GetCoreLogger()->critical(__VA_ARGS__)

// Client log macros
#define E_TRACE(...)         ::Engine::Log::GetClientLogger()->trace(__VA_ARGS__)
#define E_INFO(...)          ::Engine::Log::GetClientLogger()->info(__VA_ARGS__)
#define E_WARN(...)          ::Engine::Log::GetClientLogger()->warn(__VA_ARGS__)
#define E_ERROR(...)         ::Engine::Log::GetClientLogger()->error(__VA_ARGS__)
#define E_CRITICAL(...)      ::Engine::Log::GetClientLogger()->critical(__VA_ARGS__)

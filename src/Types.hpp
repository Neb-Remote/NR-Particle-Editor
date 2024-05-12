#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Angle.hpp>
#include <SFML/System/Vector2.hpp>
#include <cstdint>
#include <type_traits>
#include <spdlog/fmt/fmt.h>
#include <format>

using i8 = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;

using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

using f32 = float;
using f64 = double;

template <typename T>
struct fmt::formatter<sf::Vector2<T>> : fmt::formatter<std::string> {
    auto format(sf::Vector2<T> my, format_context& ctx) const -> decltype(ctx.out())
    {
        return fmt::format_to(ctx.out(), "[{}, {}]", my.x, my.y);
    }
};

template <typename T>
struct std::formatter<sf::Vector2<T>> : std::formatter<std::string> {
    auto format(sf::Vector2<T> v, format_context& ctx) const
    {
        return formatter<string>::format(std::format("[{}, {}]", v.x, v.y), ctx);
    }
};

template <typename T>
struct fmt::formatter<sf::Rect<T>> : fmt::formatter<std::string> {
    auto format(sf::Rect<T> my, format_context& ctx) const -> decltype(ctx.out())
    {
        return fmt::format_to(ctx.out(), "[{}, {}, {}, {}]", my.left, my.top, my.width, my.height);
    }
};

template <typename T>
struct std::formatter<sf::Rect<T>> : std::formatter<std::string> {
    auto format(sf::Rect<T> r, format_context& ctx) const
    {
        return formatter<string>::format(std::format("[{}, {}, {}, {}]", r.left, r.top, r.width, r.height), ctx);
    }
};

template <>
struct fmt::formatter<sf::Angle> : fmt::formatter<std::string> {
    auto format(sf::Angle my, format_context& ctx) const -> decltype(ctx.out())
    {
        return fmt::format_to(ctx.out(), "[{} deg | {} rad]", my.asDegrees(), my.asRadians());
    }
};

template <>
struct std::formatter<sf::Angle> : std::formatter<std::string> {
    auto format(sf::Angle r, format_context& ctx) const
    {
        return formatter<string>::format(std::format("[{} deg | {} rad]", r.asDegrees(), r.asRadians()), ctx);
    }
};

template <typename T>
    requires std::is_integral_v<T>
struct std::hash<sf::Vector2<T>> {
    std::size_t operator()(const sf::Vector2<T>& s) const noexcept { return static_cast<size_t>(s.x ^ (s.y << 1)); }
};
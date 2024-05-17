#pragma once
#include "Types.hpp"

#include <SFML/Graphics/Color.hpp>
#include <algorithm>
#include <array>

#define UNUSED(var) (void)(var)

[[nodiscard]] inline sf::Color LerpColour(const sf::Color& start, const sf::Color& end, f32 t)
{
    // Ensure t is clamped between 0 and 1
    t = std::clamp(t, 0.f, 1.f);

    // Perform linear interpolation for each color component
    const auto red = static_cast<u8>(start.r + t * (end.r - start.r));
    const auto green = static_cast<u8>(start.g + t * (end.g - start.g));
    const auto blue = static_cast<u8>(start.b + t * (end.b - start.b));
    const auto alpha = static_cast<u8>(start.a + t * (end.a - start.a));

    // Return the interpolated color
    return sf::Color(red, green, blue, alpha);
}

[[nodiscard]] inline sf::Color ArrayToColour(const std::array<float, 3>& colorArray)
{
    // Ensure the values are clamped between 0.0f and 1.0f
    f32 red = std::clamp(colorArray[0], 0.0f, 1.0f);
    f32 green = std::clamp(colorArray[1], 0.0f, 1.0f);
    f32 blue = std::clamp(colorArray[2], 0.0f, 1.0f);

    // Convert the f32 values (0.0 - 1.0) to sf::Uint8 (0 - 255)
    auto r = static_cast<u8>(red * 255);
    auto g = static_cast<u8>(green * 255);
    auto b = static_cast<u8>(blue * 255);

    // Return the sf::Color instance
    return sf::Color(r, g, b);
}

[[nodiscard]] inline std::array<float, 3> ColourToArray(const sf::Color& color)
{
    // Convert the sf::Uint8 values (0 - 255) to float values (0.0 - 1.0)
    const auto red = color.r / 255.0f;
    const auto green = color.g / 255.0f;
    const auto blue = color.b / 255.0f;

    // Return the std::array<float, 3> instance
    return { red, green, blue };
}
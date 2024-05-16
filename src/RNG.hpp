#pragma once

#include <random>
#include <type_traits>

class RNG {
public:
    template <typename T>
        requires std::is_floating_point_v<T>
    [[nodiscard]] static T RealWithinRange(T start, T end);

    template <typename T>
        requires std::is_integral_v<T>
    [[nodiscard]] static T IntWithinRange(T start, T end);

private:
    static std::random_device m_randomDevice;
    static std::default_random_engine m_randomEngine;
};

template <typename T>
    requires std::is_floating_point_v<T>
inline T RNG::RealWithinRange(T start, T end)
{
    T result { 0 };
    std::uniform_real_distribution<T> dist { start, end };
    result = dist(m_randomEngine);
    return result;
}

template <typename T>
    requires std::is_integral_v<T>
inline T RNG::IntWithinRange(T start, T end)
{
    T result { 0 };
    std::uniform_int_distribution<T> dist { start, end };
    result = dist(m_randomEngine);
    return result;
}

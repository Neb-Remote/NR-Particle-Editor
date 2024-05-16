#include "RNG.hpp"

std::random_device RNG::m_randomDevice;
std::default_random_engine RNG::m_randomEngine { RNG::m_randomDevice() };

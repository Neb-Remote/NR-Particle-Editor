#pragma once

#include "Types.hpp"

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Angle.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/System/Vector2.hpp>
#include <string>
#include <tl/expected.hpp>

struct ParticleProperties {
    f32 alpahStart { 0.f };
    f32 alphaEnd { 0.f };

    f32 scaleStart { 0.f };
    f32 scaleEnd { 0.f };
    f32 minimumScaleMultiplier { 1.f }; // Value between 1 and minimumScaleMultiplier is generated randomly
                                        // then multiplied by the start and end scale
    sf::Color colourStart;
    sf::Color colourEnd;

    f32 speedStart { 0.f };
    f32 speedEnd { 0.f };
    f32 minimumSpeedMultiplier { 1.f }; // Value between 1 and minimumScaleMultiplier is generated randomly
                                        // then multiplied by the start and end scale

    sf::Vector2f acceleration; // Prevents use of speed (speed takes precedent)
    f32 maximumSpeed { 0.f }; // Maximum speed on accelerating particles only

    sf::Angle startRotationMin { sf::degrees(0.f) };
    sf::Angle startRotationMax { sf::degrees(15.f) };

    sf::Time lifeTimeMin { sf::seconds(0.5f) };
    sf::Time lifeTimeMax { sf::seconds(0.5f) };
};

struct EmitterProperties {
    sf::Time spawnFrequency;
    sf::Time lifeTime;
    u32 maxParticles;
    bool addAtBack { false }; // Should particles be added at the back of the draw list
};

struct PropertiesFileData {
    ParticleProperties particleProperties;
    EmitterProperties emitterProperties;
    sf::Texture* texture { nullptr };
};

[[nodiscard]] tl::expected<PropertiesFileData, std::string> LoadFromJSON(std::string_view filePath) noexcept;
// void SaveToJSON();
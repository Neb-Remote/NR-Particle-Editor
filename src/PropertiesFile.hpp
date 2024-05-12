#pragma once

#include "Types.hpp"

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Angle.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/System/Vector2.hpp>

struct ParticleProperties {
    f32 alpahStart{ 0.f };
    f32 alphaEnd{ 0.f };

    f32 scaleStart{ 0.f };
    f32 scaleEnd{ 0.f };
    f32 scaleMultiplier{ 1.f };

    sf::Color colourStart;
    sf::Color colourEnd;

    f32 speedStart{ 0.f };
    f32 speedEnd{ 0.f };

    sf::Vector2f acceleration; // Prevents use of speed (speed takes precedent)
    f32 maximumSpeed{ 0.f }; // Maximum speed on accelerating particles only

    sf::Angle startRotationMin{ sf::degrees(0.f) };
    sf::Angle startRotationMax{ sf::degrees(15.f) };

    sf::Time lifeTimeMin{ sf::seconds(0.5f) };
    sf::Time lifeTimeMax{ sf::seconds(0.5f) };

    sf::Texture* texture{ nullptr };
};

struct EmitterProperties {};

struct PropertiesFileData {
    ParticleProperties particleProperties;
    EmitterProperties emitterProperties;
};

void loadFromJSON(std::string_view filePath);
void saveToJSON();
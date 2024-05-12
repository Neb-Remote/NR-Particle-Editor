#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Angle.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/System/Vector2.hpp>

struct ParticleProperties {
    float alpahStart { 0.f };
    float alphaEnd { 0.f };

    float scaleStart { 0.f };
    float scaleEnd { 0.f };
    float scaleMultiplier { 1.f };

    sf::Color colourStart;
    sf::Color colourEnd;

    float speedStart { 0.f };
    float speedEnd { 0.f };

    sf::Vector2f acceleration; // Prevents use of speed (speed takes precedent)
    float maximumSpeed { 0.f }; // Maximum speed on accelerating particles only

    sf::Angle startRotationMin { sf::degrees(0.f) };
    sf::Angle startRotationMax { sf::degrees(15.f) };

    sf::Time lifeTimeMin { sf::seconds(0.5f) };
    sf::Time lifeTimeMax { sf::seconds(0.5f) };

    sf::Texture* texture { nullptr };
};

struct EmitterProperties { };

struct PropertiesFileData {
    ParticleProperties particleProperties;
    EmitterProperties emitterProperties;
};

void loadFromJSON(std::string_view filePath);
void saveToJSON();
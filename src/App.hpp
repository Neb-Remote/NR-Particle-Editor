#pragma once

#include "ParticleEmitter.hpp"
#include "PropertiesFile.hpp"

#include <SFML/Graphics/RenderWindow.hpp>

class App {
public:
    App();
    ~App();

    void run();

private:
    sf::RenderWindow m_renderWindow;
    PropertiesFileData m_propertiesFileData;
    ParticleEmitter m_emitter;
};
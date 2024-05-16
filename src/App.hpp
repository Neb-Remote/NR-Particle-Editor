#pragma once

#include "ParticleEmitter.hpp"
#include "PropertiesFile.hpp"

#include <SFML/Graphics/RenderWindow.hpp>
#include <string>
#include <memory>

class App {
public:
    App();
    ~App();

    void run();

private:
    void displayMenu();

    sf::RenderWindow m_renderWindow;
    PropertiesFileData m_propertiesFileData;
    std::unique_ptr<ParticleEmitter> m_emitter;
    std::string m_loadedFilePath;
    std::string m_loadErrorString;
};
#pragma once

#include "ParticleEmitter.hpp"
#include "PropertiesFile.hpp"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Vector2.hpp>
#include <memory>
#include <string>

class App {
public:
    App();
    ~App();

    void run();

private:
    enum class EditorTab { ParticleProperties, EmitterProperties };

    void displayMenu(sf::Time dt);
    void displayPropertiesEditor();
    void displayParticleProperties();
    void displayEmitterProperties();
    sf::Vector2i calculateMenuSize();
    void resetEmitter();

    sf::RenderWindow m_renderWindow;
    PropertiesFileData m_propertiesFileData;
    std::unique_ptr<ParticleEmitter> m_emitter;
    std::string m_loadedFilePath;
    std::string m_loadErrorString;
    EditorTab m_editorTab { EditorTab::ParticleProperties };
};
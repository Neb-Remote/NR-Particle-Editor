#pragma once

#include <SFML/Graphics/RenderWindow.hpp>

class App {
public:
    App();
    ~App();

    void run();

private:
    sf::RenderWindow m_renderWindow;
};

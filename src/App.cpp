#include "App.hpp"

#include "InputHandler.hpp"

#include <SFML/System/String.hpp>
#include <SFML/Window/Event.hpp>

constexpr auto WINDOW_TITLE { "NR Particle Editor" };

App::App()
{
    const auto desktopMode = sf::VideoMode::getDesktopMode();
    m_renderWindow.create(desktopMode, WINDOW_TITLE, sf::Style::Default);
    m_renderWindow.setFramerateLimit(60);
}

App::~App() { }

void App::run()
{
    sf::Clock loopClock;

    while (m_renderWindow.isOpen()) {
        const auto dt = loopClock.restart();
        InputHandler::Update();

        while (auto event = m_renderWindow.pollEvent()) {
            if (event.is<sf::Event::Closed>())
                m_renderWindow.close();

            InputHandler::HandleEvent(event, m_renderWindow);
        }

        m_renderWindow.clear();
        m_renderWindow.display();
    }
}
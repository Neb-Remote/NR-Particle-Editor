#include "App.hpp"

#include "InputHandler.hpp"
#include <spdlog/spdlog.h>

#include <SFML/System/String.hpp>
#include <SFML/Window/Event.hpp>
#include <imgui-SFML.h>

constexpr auto WINDOW_TITLE{ "NR Particle Editor" };

App::App()
    : m_emitter(&m_propertiesFileData)
{
    const auto desktopMode = sf::VideoMode::getDesktopMode();
    m_renderWindow.create(desktopMode, WINDOW_TITLE, sf::Style::Default);
    m_renderWindow.setFramerateLimit(60);

    if (!ImGui::SFML::Init(m_renderWindow))
        throw std::runtime_error("Unable to init ImGui::SFML!");

#ifdef NR_PARTICLE_EDITOR_DEBUG
    spdlog::set_level(spdlog::level::debug);
#else
    spdlog::set_level(spdlog::level::info);
#endif
}

App::~App() { ImGui::SFML::Shutdown(m_renderWindow); }

void App::run()
{
    sf::Clock loopClock;

    while (m_renderWindow.isOpen()) {
        const auto dt = loopClock.restart();
        InputHandler::Update();

        while (auto event = m_renderWindow.pollEvent()) {
            if (event.is<sf::Event::Closed>()) {
                m_renderWindow.close();
            } else if (auto resize = event.getIf<sf::Event::Resized>()) {
                const sf::FloatRect visibleArea(sf::Vector2f(0.f, 0.f), sf::Vector2f(resize->size));
                m_renderWindow.setView(sf::View(visibleArea));
            }

            InputHandler::HandleEvent(event, m_renderWindow);
            ImGui::SFML::ProcessEvent(m_renderWindow, event);
        }

        ImGui::SFML::Update(m_renderWindow, dt);

        m_renderWindow.clear();

        m_renderWindow.draw(m_emitter);

        ImGui::SFML::Render();
        m_renderWindow.display();
    }
}
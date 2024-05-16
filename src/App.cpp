#include "App.hpp"

#include "InputHandler.hpp"
#include <spdlog/spdlog.h>

#include <ImGuiFileDialog.h>
#include <SFML/System/String.hpp>
#include <SFML/Window/Event.hpp>
#include <imgui-SFML.h>
#include <imgui.h>

constexpr auto WINDOW_TITLE { "NR Particle Editor" };

App::App()
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

    if (!sf::VertexBuffer::isAvailable())
        throw std::runtime_error("Vertex buffers unavailable");
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
            } else if (const auto resize = event.getIf<sf::Event::Resized>()) {
                const sf::FloatRect visibleArea(sf::Vector2f(0.f, 0.f), sf::Vector2f(resize->size));
                m_renderWindow.setView(sf::View(visibleArea));
            }

            InputHandler::HandleEvent(event, m_renderWindow);
            ImGui::SFML::ProcessEvent(m_renderWindow, event);
        }

        ImGui::SFML::Update(m_renderWindow, dt);
        displayMenu();
        if (m_emitter)
            m_emitter->update(dt);

        m_renderWindow.clear();

        if (m_emitter)
            m_renderWindow.draw(*m_emitter);

        ImGui::SFML::Render(m_renderWindow);
        m_renderWindow.display();
    }
}

void App::displayMenu()
{
    ImGui::Begin("Emitter Editor");
    if (ImGui::Button("Open Existing Emitter JSON")) {
        IGFD::FileDialogConfig config;
        config.path = ".";
        ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose File", ".json", config);
    }

    if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey")) {
        if (ImGuiFileDialog::Instance()->IsOk()) {
            m_loadedFilePath = ImGuiFileDialog::Instance()->GetFilePathName();
            if (const auto loadResult = LoadFromJSON(m_loadedFilePath)) {
                m_propertiesFileData = loadResult.value();
                // Reset the emitter
                m_emitter.reset();
                m_emitter = std::make_unique<ParticleEmitter>(&m_propertiesFileData);
                m_emitter->setPosition(sf::Vector2f(m_renderWindow.getSize()) * 0.5f);
                m_emitter->play();
                m_emitter->setLooping(true);

            } else {
                m_loadErrorString = loadResult.error();
            }
        }
        ImGuiFileDialog::Instance()->Close();
    }

    if (!m_loadErrorString.empty())
        ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), "Failed to load JSON file: %s", m_loadErrorString.data());
    if (m_emitter)
        ImGui::Text("Alive Particle Count %d", m_emitter->getAliveParticleCount());
    else
        ImGui::Text("Alive Particle Count 0");

    ImGui::End();
}
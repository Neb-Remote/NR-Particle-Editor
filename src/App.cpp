#include "App.hpp"
#include "InputHandler.hpp"
#include "Types.hpp"
#include "Utils.hpp"

#include <ImGuiFileDialog.h>
#include <SFML/System/String.hpp>
#include <SFML/Window/Event.hpp>
#include <imgui-SFML.h>
#include <imgui.h>
#include <spdlog/spdlog.h>

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

    // TODO: Use this to find colour flags, then make colour picker smaller ImGuiColorEditFlags_DefaultOptions_
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
        displayMenu(dt);

        if (m_emitter) {
            // If the emitter is no longer playing it means looping has been disabled
            // due to an edit to the emitter properties, so reset the emitter
            if (!m_emitter->isPlaying()) {
                resetEmitter();
            }
            m_emitter->update(dt);
        }

        m_renderWindow.clear();

        if (m_emitter)
            m_renderWindow.draw(*m_emitter);

        ImGui::SFML::Render(m_renderWindow);
        m_renderWindow.display();
    }
}

void App::displayMenu(sf::Time dt)
{
    ImGui::Begin(
        "Emitter Editor", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);
    ImGui::SetWindowSize(calculateMenuSize());
    ImGui::SetWindowPos({ 0, 0 });

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
                m_loadErrorString.clear();
                resetEmitter();
            } else {
                m_loadErrorString = loadResult.error();
            }
        }
        ImGuiFileDialog::Instance()->Close();
    }

    ImGui::Text("Frame Rate %f", 1.f / dt.asSeconds());
    ImGui::Text("Milliseconds Per Frame %d", dt.asMilliseconds());
    if (!m_loadErrorString.empty())
        ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), "Failed to load JSON file: %s", m_loadErrorString.data());
    if (m_emitter)
        ImGui::Text("Alive Particle Count %d", m_emitter->getAliveParticleCount());
    else
        ImGui::Text("Alive Particle Count 0");

    if (m_emitter) {
        displayPropertiesEditor();
    }
    ImGui::End();
}

void App::displayPropertiesEditor()
{

    if (ImGui::BeginTabBar("Effect Properties")) {
        if (ImGui::BeginTabItem("Particle Properties")) {
            m_editorTab = EditorTab::ParticleProperties;
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Emitter Properties")) {
            m_editorTab = EditorTab::EmitterProperties;
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }

    switch (m_editorTab) {
    case EditorTab::ParticleProperties:
        displayParticleProperties();
        break;
    case EditorTab::EmitterProperties:
        displayEmitterProperties();
        break;
    default:
        assert(false);
    }
}

void App::displayParticleProperties()
{
    // We disable looping after any edits to ensure the particle system will stop playing
    // since when it stops playing there is a force reset mechanism performed by the app
    // which will recreate the emitter with the updated properties

    /* Alpha */
    if (ImGui::SliderFloat("Alpha Start", &m_propertiesFileData.particleProperties.alphaStart, 0.f, 1.f)) {
        m_emitter->setLooping(false);
    }

    if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
        ImGui::SetTooltip("Particle start alpha value");

    if (ImGui::SliderFloat("Alpha End", &m_propertiesFileData.particleProperties.alphaEnd, 0.f, 1.f))
        m_emitter->setLooping(false);

    if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
        ImGui::SetTooltip("Particle end alpha value");

    /* Scale */

    if (ImGui::InputFloat("Scale Start", &m_propertiesFileData.particleProperties.scaleStart))
        m_emitter->setLooping(false);

    if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
        ImGui::SetTooltip("Particle start scale (applies uniformly to x & y axis)");

    if (ImGui::InputFloat("Scale End", &m_propertiesFileData.particleProperties.scaleEnd))
        m_emitter->setLooping(false);

    if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
        ImGui::SetTooltip("Particle end scale (applies uniformly to x & y axis)");

    if (ImGui::InputFloat("Minimum Scale Multiplier", &m_propertiesFileData.particleProperties.minimumScaleMultiplier))
        m_emitter->setLooping(false);

    if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
        ImGui::SetTooltip(
            "Value between minimum scale multiplier and 1 is randomly selected, then applied to start and "
            "end scale of a particle");

    /* Colour */
    auto colourArray = ColourToArray(m_propertiesFileData.particleProperties.colourStart);
    if (ImGui::ColorPicker3("Start Colour", colourArray.data())) {
        m_emitter->setLooping(false);
        m_propertiesFileData.particleProperties.colourStart = ArrayToColour(colourArray);
    }

    if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
        ImGui::SetTooltip("Particle start colour");

    colourArray = ColourToArray(m_propertiesFileData.particleProperties.colourEnd);
    if (ImGui::ColorPicker3("End Colour", colourArray.data())) {
        m_emitter->setLooping(false);
        m_propertiesFileData.particleProperties.colourEnd = ArrayToColour(colourArray);
    }
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
        ImGui::SetTooltip("Particle end colour");
}

void App::displayEmitterProperties() { }

sf::Vector2i App::calculateMenuSize()
{
    sf::Vector2i size;
    size.x = static_cast<i32>(static_cast<f32>(m_renderWindow.getSize().x) * 0.20f);
    size.y = m_renderWindow.getSize().y;
    return size;
}

void App::resetEmitter()
{
    m_emitter.reset();
    m_emitter = std::make_unique<ParticleEmitter>(&m_propertiesFileData);
    m_emitter->setPosition(sf::Vector2f(m_renderWindow.getSize()) * 0.5f);
    m_emitter->play();
    m_emitter->setLooping(true);
}

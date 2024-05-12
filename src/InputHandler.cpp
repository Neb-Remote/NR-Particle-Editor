#include "InputHandler.hpp"

#include <SFML/Window/Event.hpp>
#include <imgui.h>

sf::Vector2f InputHandler::m_mousePositionWorld;
sf::Vector2i InputHandler::m_mousePositionScreen;
sf::Vector2i InputHandler::m_mouseScreenDelta;

std::set<sf::Keyboard::Scan> InputHandler::m_keyboardPressed;
std::set<sf::Keyboard::Scan> InputHandler::m_keyboardReleased;
std::set<sf::Keyboard::Scan> InputHandler::m_keyboardHeld;

std::set<sf::Mouse::Button> InputHandler::m_mouseButtonPressed;
std::set<sf::Mouse::Button> InputHandler::m_mouseButtonReleased;
std::set<sf::Mouse::Button> InputHandler::m_mouseButtonHeld;

void InputHandler::Update()
{
    m_mouseScreenDelta = {};
    m_keyboardPressed.clear();
    m_keyboardReleased.clear();
    m_mouseButtonPressed.clear();
    m_mouseButtonReleased.clear();
}

void InputHandler::HandleEvent(sf::Event& event, sf::RenderWindow& window)
{
    if (event.getIf<sf::Event::FocusLost>()) {
        m_keyboardHeld.clear();
        m_mouseButtonHeld.clear();
    }

    HandleKeyboardAndMouse(event, window);
}

auto InputHandler::KeyPressed(sf::Keyboard::Scan key) -> bool
{
    const auto present = m_keyboardPressed.find(key);
    return present != m_keyboardPressed.end();
}

auto InputHandler::KeyHeld(sf::Keyboard::Scan key) -> bool
{
    const auto present = m_keyboardHeld.find(key);
    return present != m_keyboardHeld.end();
}

auto InputHandler::KeyReleased(sf::Keyboard::Scan key) -> bool
{
    const auto present = m_keyboardReleased.find(key);
    return present != m_keyboardReleased.end();
}

auto InputHandler::MousePressed(sf::Mouse::Button button) -> bool
{
    const auto present = m_mouseButtonPressed.find(button);
    return present != m_mouseButtonPressed.end();
}

auto InputHandler::MouseReleased(sf::Mouse::Button button) -> bool
{
    const auto present = m_mouseButtonReleased.find(button);
    return present != m_mouseButtonReleased.end();
}

auto InputHandler::MouseHeld(sf::Mouse::Button button) -> bool
{
    const auto present = m_mouseButtonHeld.find(button);
    return present != m_mouseButtonHeld.end();
}
auto InputHandler::GetMouseWorldPosition() -> sf::Vector2f { return m_mousePositionWorld; }

auto InputHandler::GetMouseScreenPosition() -> sf::Vector2i { return m_mousePositionScreen; }

auto InputHandler::GetMouseScreenDelta() -> sf::Vector2i { return m_mouseScreenDelta; }

void InputHandler::HandleKeyboardAndMouse(const sf::Event& event, sf::RenderWindow& window)
{
    if (const auto mouseMove = event.getIf<sf::Event::MouseMoved>()) {
        const auto before = m_mousePositionScreen;
        m_mousePositionScreen = sf::Vector2i { mouseMove->position.x, mouseMove->position.y };
        m_mouseScreenDelta = m_mousePositionScreen - before;
        m_mousePositionWorld = window.mapPixelToCoords(m_mousePositionScreen);
    }

    if (const auto mouseButtonPressed = event.getIf<sf::Event::MouseButtonPressed>()) {
        if (!ImGui::GetIO().WantCaptureMouse) {
            m_mouseButtonPressed.emplace(mouseButtonPressed->button);
            m_mouseButtonHeld.emplace(mouseButtonPressed->button);
        }
    }

    if (const auto mouseButtonReleased = event.getIf<sf::Event::MouseButtonReleased>()) {
        if (!ImGui::GetIO().WantCaptureMouse) {
            auto removeIt = m_mouseButtonHeld.find(mouseButtonReleased->button);
            if (removeIt != m_mouseButtonHeld.end())
                m_mouseButtonHeld.erase(removeIt);

            m_mouseButtonReleased.emplace(mouseButtonReleased->button);
        }
    }

    // Keyboard pressed
    if (const auto keyPressed = event.getIf<sf::Event::KeyPressed>()) {
        m_keyboardPressed.emplace(keyPressed->scancode);
        m_keyboardHeld.emplace(keyPressed->scancode);
    }

    // Keyboard released
    if (const auto keyReleased = event.getIf<sf::Event::KeyReleased>()) {
        m_keyboardReleased.emplace(keyReleased->scancode);
        auto removeIt = m_keyboardHeld.find(keyReleased->scancode);
        if (removeIt != m_keyboardHeld.end())
            m_keyboardHeld.erase(removeIt);
    }
}

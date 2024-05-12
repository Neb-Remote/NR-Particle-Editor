#pragma once

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
#include <set>

class InputHandler {
public:
    static void Update();
    static void HandleEvent(sf::Event& event, sf::RenderWindow& window);

    [[nodiscard]] static auto KeyPressed(sf::Keyboard::Scan key) -> bool;
    [[nodiscard]] static auto KeyHeld(sf::Keyboard::Scan key) -> bool;
    [[nodiscard]] static auto KeyReleased(sf::Keyboard::Scan key) -> bool;

    [[nodiscard]] static auto MousePressed(sf::Mouse::Button button) -> bool;
    [[nodiscard]] static auto MouseReleased(sf::Mouse::Button button) -> bool;
    [[nodiscard]] static auto MouseHeld(sf::Mouse::Button button) -> bool;

    [[nodiscard]] static auto GetMouseWorldPosition() -> sf::Vector2f;
    [[nodiscard]] static auto GetMouseScreenPosition() -> sf::Vector2i;
    [[nodiscard]] static auto GetMouseScreenDelta() -> sf::Vector2i;

private:
    static void HandleKeyboardAndMouse(const sf::Event& event, sf::RenderWindow& window);

    static sf::Vector2f m_mousePositionWorld;
    static sf::Vector2i m_mousePositionScreen;
    static sf::Vector2i m_mouseScreenDelta;

    static std::set<sf::Keyboard::Scan> m_keyboardPressed;
    static std::set<sf::Keyboard::Scan> m_keyboardReleased;
    static std::set<sf::Keyboard::Scan> m_keyboardHeld;

    static std::set<sf::Mouse::Button> m_mouseButtonPressed;
    static std::set<sf::Mouse::Button> m_mouseButtonReleased;
    static std::set<sf::Mouse::Button> m_mouseButtonHeld;
};
} // nce namespace

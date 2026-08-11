#include "Window.h"
#include <SFML/Window/Keyboard.hpp>

Window::Window(unsigned int width, unsigned int height, const std::string& title)
    : window_(sf::VideoMode(width, height), title)
{
}

bool Window::IsOpen() const
{
    return window_.isOpen();
}

void Window::PollEvents()
{
    sf::Event event;
    while (window_.pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
            window_.close();
    }
}

sf::RenderWindow& Window::GetRaw()
{
    return window_;
}

bool Window::IsKeyPressed(sf::Keyboard::Key key) const
{
    return sf::Keyboard::isKeyPressed(key);
}

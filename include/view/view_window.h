#ifndef VIEW_WINDOW_H
#define VIEW_WINDOW_H

#include <SFML/Graphics.hpp>
#include <string>
#include <functional>

namespace genetic
{

namespace view
{

class ViewWindow
{
    private:
        sf::RenderWindow window_;
        std::function<void(const sf::Event&)> input_callback_;
        std::function<void(float dt)> step_callback_;
        std::function<void(sf::RenderTarget&)> display_callback_;

        void input();
        void step(float dt);
        void display();

    public:
        ViewWindow(
            const std::string& problem,
            sf::Vector2u dimensions,
            std::function<void(const sf::Event&)> input,
            std::function<void(float dt)> step,
            std::function<void(sf::RenderTarget&)> display
        );
        void run();
};

}

}

namespace genetic
{

namespace view
{

ViewWindow::ViewWindow(
    const std::string& problem,
    sf::Vector2u dimensions,
    std::function<void(const sf::Event&)> input,
    std::function<void(float dt)> step,
    std::function<void(sf::RenderTarget&)> display
)   : window_(sf::VideoMode(dimensions), problem + " SFML")
    , input_callback_(std::move(input))
    , step_callback_(std::move(step_callback_))
    , display_callback_(std::move(display))
{}

void ViewWindow::run()
{
    sf::Clock clock;

    clock.start();
    while (window_.isOpen())
    {
        input();
        step(clock.restart().asSeconds());
        display();
    }
}

void ViewWindow::input()
{
    while (const std::optional event = window_.pollEvent())
    {
        if (event->is<sf::Event::Closed>())
        {
            window_.close();
        }
        else
        {
            input_callback_(event);
        }
    }
}

void ViewWindow::step(float dt)
{
    step_callback_(dt);
}

void ViewWindow::display()
{
    display_callback_(window_);
}

}

}
#endif
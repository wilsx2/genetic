#include "graphic_view.h"

namespace genetic
{

template <typename T>
GraphicView<T>::GraphicView(
    const std::string& problem,
    sf::Vector2u dimensions
)   : problem_(problem)
    , dimensions_(dimensions)
{}

template <typename T>
void GraphicView<T>::run()
{
    sf::RenderWindow window (sf::VideoMode(dimensions_), problem_+" SFML");

    clock_.stop();
    clock_.start();
    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }
        }

        render(window);
    }
    clock_.reset();
}

}
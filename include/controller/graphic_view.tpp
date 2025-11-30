#include "graphic_view.h"

namespace genetic
{

template <typename T>
GraphicView<T>::GraphicView(
    const std::string& problem,
    sf::Vector2u dimensions,
    RenderCallback render
)   : problem_(problem)
    , dimensions_(dimensions)
    , render_callback_(render)
{}

template <typename T>
void GraphicView<T>::run(const std::vector<Member<T>>& members, ViewType view_type)
{
    sf::RenderWindow window (sf::VideoMode(dimensions_), problem_+" SFML");

    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }
        }

        render_callback_(window, members, view_type);
    }
}

template <typename T>
ViewCallback<T> GraphicView<T>::callback()
{
    return [this](const std::vector<Member<T>> m, ViewType v){ this->run(m, v); };
}

}
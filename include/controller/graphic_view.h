#ifndef GRAPHIC_VIEW_H
#define GRAPHIC_VIEW_H

#include "controller.h"
#include "core/member.h"
#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>
#include <string>
#include <functional>

namespace genetic
{

template <typename T>
class GraphicView : public View<T>
{
    protected:
        sf::Clock clock_;
    private:
        std::string problem_;
        sf::Vector2u dimensions_;
        virtual void render(sf::RenderWindow&) = 0;
        void run();

    public:
        GraphicView(
            const std::string& problem,
            sf::Vector2u dimensions
        );
};

}

#include "graphic_view.tpp"
#endif
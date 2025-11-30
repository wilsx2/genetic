#ifndef VIEW_WINDOW_H
#define VIEW_WINDOW_H

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
class GraphicView
{
    private:
        using RenderCallback = std::function<void(sf::RenderTarget&, const std::vector<Member<T>>&, ViewType)>;
        std::string problem_;
        sf::Vector2u dimensions_;
        RenderCallback render_callback_;

    public:
        GraphicView(
            const std::string& problem,
            sf::Vector2u dimensions,
            RenderCallback render
        );
        void run(const std::vector<Member<T>>& members, ViewType view_type);
        ViewCallback<T> callback();
};

}

#include "graphic_view.tpp"
#endif
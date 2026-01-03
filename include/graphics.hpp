#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>

namespace graphics
{

void drawLine(sf::RenderWindow& target, sf::Vector2f a, sf::Vector2f b, float width, sf::Color color)
{
    static sf::Vector2f normal (1.f, 0.f);
    static sf::Angle half_pi (sf::degrees(90));
    sf::VertexArray va (sf::PrimitiveType::TriangleStrip);

    sf::Angle angle = (a - b).angle();

    sf::Vertex v1;
    v1.position = a + (normal*width/2.f).rotatedBy(angle - half_pi);
    v1.color = color;

    sf::Vertex v2;
    v2.position = a + (normal*width/2.f).rotatedBy(angle + half_pi);
    v2.color = color;

    sf::Vertex v3;
    v3.position = b + (normal*width/2.f).rotatedBy(angle - half_pi);
    v3.color = color;

    sf::Vertex v4;
    v4.position = b + (normal*width/2.f).rotatedBy(angle + half_pi);
    v4.color = color;

    va.append(std::move(v1));
    va.append(std::move(v2));
    va.append(std::move(v3));
    va.append(std::move(v4));

    target.draw(va);
}

}
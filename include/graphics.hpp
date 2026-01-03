#include <SFML/Graphics.hpp>
#include <vector>

namespace graphics
{

void drawLines(sf::RenderWindow& target, const std::vector<sf::Vector2f> points, float width, sf::Color color)
{
    static sf::Vector2f normal (1.f, 0.f);
    static sf::Angle half_pi (sf::radians(1.5707964));
    if (points.size() <= 1)
        return;

    sf::VertexArray va (sf::PrimitiveType::TriangleStrip);

    for(int i = 0; i < points.size(); ++i)
    {        
        sf::Angle angle;
        if (i == 0)
        {
            angle = points[i].angleTo(points[i + 1]);
        }
        else if(i > 0 && i < points.size() - 1)
        {
            angle = (points[i-1].angleTo(points[i]) + points[i].angleTo(points[i+1]))/2;
        }
        else if (i == points.size())
        {
            angle = points[i-1].angleTo(points[i]);
        }

        sf::Vertex v1;
        v1.position = points[i] + normal.rotatedBy(half_pi)*width;
        v1.color = color;

        sf::Vertex v2;
        v1.position = points[i] - normal.rotatedBy(half_pi)*width;
        v1.color = color;

        va.append(std::move(v1));
        va.append(std::move(v2));
    }

    target.draw(va);
}

void drawLine(sf::RenderWindow& target, sf::Vector2f a, sf::Vector2f b, float width, sf::Color color)
{
    std::vector<sf::Vector2f> points {a, b};
    drawLines(target, points, width, color);
}

}
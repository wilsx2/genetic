#include "genetic.h"
#include "monalisa.h"
#include <SFML/Graphics.hpp>
#include <array>
#include <numbers>

//TODO: Constrain points within the image
namespace img_approx
{

struct Triangle
{
    sf::Vector2f p1;
    sf::Vector2f p2;
    sf::Vector2f p3;
    sf::Color color;
};
constexpr unsigned int NUM_TRIANGLES = 64;
using Approximation = std::array<Triangle, NUM_TRIANGLES>;

const sf::Image monalisa {monalisa_jpg, monalisa_jpg_len};

sf::Image renderApproximation(const Approximation& approx)
{
    sf::RenderTexture texture (monalisa.getSize());
    sf::VertexArray va (sf::PrimitiveType::Triangles);
    for (const auto& triangle : approx)
    {
        sf::Vertex v1, v2, v3;
        v1.color = triangle.color;
        v2.color = triangle.color;
        v3.color = triangle.color;
        v1.position = triangle.p1;
        v2.position = triangle.p2;
        v3.position = triangle.p3;
        va.append(std::move(v1));
        va.append(std::move(v2));
        va.append(std::move(v3));
    }
    texture.draw(va);
    texture.display();

    return texture.getTexture().copyToImage();
} 

class ImageApproximationScenario : public genetic::BinaryEncodedScenario<Approximation>
{
    private:
    using BinApprox = BinT;
    
    inline static const std::string name = "monalisa";

    public: 
    ImageApproximationScenario()
    : genetic::BinaryEncodedScenario<Approximation>(name)
    { }
    const std::string& getName()
    {
        return name;
    }
    
    float evaluateFitness(const Approximation& approx)
    {
        static const std::size_t NUM_PIXELS = monalisa.getSize().x * monalisa.getSize().y * 4;
        
        float sum = 0.f;
        sf::Image render = std::move(renderApproximation(approx));
        for (int i = 0; i < NUM_PIXELS; ++i)
        {
            float target_pixel_value = static_cast<float>(monalisa.getPixelsPtr()[i]);
            float approximated_pixel_value = static_cast<float>(render.getPixelsPtr()[i]);
            float difference = target_pixel_value - approximated_pixel_value;
            float squared_difference = difference * difference;
            sum += squared_difference;
        }
        return -sum;
    }
};

}
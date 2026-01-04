#include "genetic.h"
#include "monalisa.h"
#include <SFML/Graphics.hpp>
#include <array>
#include <numbers>

//TODO: Constrain points within the image
namespace img
{

struct Triangle
{
    sf::Vector2f p1;
    sf::Vector2f p2;
    sf::Vector2f p3;
    sf::Color color;
};
constexpr unsigned int NUM_TRIANGLES = 16;
using Approximation = std::array<Triangle, NUM_TRIANGLES>;
using BinApprox = genetic::BinaryEncoding<Approximation>;

const sf::Image monalisa {monalisa_jpg, monalisa_jpg_len};



sf::Texture renderApproximation(const Approximation& approx)
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

    return texture.getTexture();
} 

class Scenario : public genetic::BinaryEncodedScenario<Approximation>
{
    private:
    using BinApprox = BinT;
    
    inline static const std::string name = "monalisa";

    public: 
    Scenario()
    : genetic::BinaryEncodedScenario<Approximation>(name)
    { }
    const std::string& getName()
    {
        return name;
    }
    
    float evaluateFitness(const BinApprox& approx)
    {
        static const std::size_t NUM_PIXELS = monalisa.getSize().x * monalisa.getSize().y * 4;
        
        float sum = 0.f;
        sf::Image render = renderApproximation(approx.get()).copyToImage();
        for (int i = 0; i < NUM_PIXELS; ++i)
        {
            if (i % 4 == 0) continue; //Ignore opacity

            float target_pixel_value = static_cast<float>(monalisa.getPixelsPtr()[i]);
            float approximated_pixel_value = static_cast<float>(render.getPixelsPtr()[i]);
            float difference = target_pixel_value - approximated_pixel_value;
            float squared_difference = difference * difference;
            sum += squared_difference;
        }
        return -sum;
    }
};

class View : public genetic::GraphicView<BinApprox>
{
    private:
    static constexpr float ANIMATION_DURATION = 10.f;

    void render(sf::RenderWindow& target)
    {
        // Determine current member
        float percent_completed = clock_.getElapsedTime().asSeconds() / ANIMATION_DURATION;
        if (percent_completed > 1.f)
            percent_completed = 1.f;

        int i = static_cast<int>((members_.size()-1) * percent_completed);
        Approximation approx = members_[i].value.get();
        sf::Texture tex = renderApproximation(approx);
        // Render
        target.clear();
        target.draw(sf::Sprite(tex));
        target.display();
    }

    public:
    View(): genetic::GraphicView<BinApprox>(
        "Image Approximation", monalisa.getSize()
    ) {}
};

}

int main()
{
    auto cli = genetic::Controller<img::BinApprox>
    (
        genetic::GeneticAlgorithm<img::BinApprox>(
            std::make_unique<img::Scenario>(),
            genetic::selection::rankBased<img::BinApprox>,
            10,
            .1f
        ),
        std::make_unique<img::View>()
    );
    cli.run();
    return 0;
}

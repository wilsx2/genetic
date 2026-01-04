#include "genetic.h"
#include "monalisa.h"
#include <SFML/Graphics.hpp>
#include <array>
#include <limits>
#include <cmath>

//TODO: Constrain points within the image
namespace img
{

struct Triangle
{
    uint16_t x1,x2,x3,y1,y2,y3;
    sf::Color color;

    Triangle() = default;
    Triangle(util::RNG rng)
    : x1(rng.integer(0, std::numeric_limits<uint16_t>::max()))
    , x2(rng.integer(0, std::numeric_limits<uint16_t>::max()))
    , x3(rng.integer(0, std::numeric_limits<uint16_t>::max()))
    , y1(rng.integer(0, std::numeric_limits<uint16_t>::max()))
    , y2(rng.integer(0, std::numeric_limits<uint16_t>::max()))
    , y3(rng.integer(0, std::numeric_limits<uint16_t>::max()))
    , color(rng.integer(0, std::numeric_limits<uint32_t>::max()))
    { }
};
constexpr unsigned int NUM_TRIANGLES = 64;
using Approximation = std::array<Triangle, NUM_TRIANGLES>;

const sf::Image monalisa {monalisa_jpg, monalisa_jpg_len};

sf::Vector2f vectorToImageVector(uint16_t x, uint16_t y)
{
    static const float max_value = static_cast<float>(std::numeric_limits<uint16_t>::max());
    static const float img_width = monalisa.getSize().x;
    static const float img_height = monalisa.getSize().y;
    return {
        (static_cast<float>(x) / max_value) * img_width,
        (static_cast<float>(y) / max_value) * img_height
    };
}

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
        v1.position = vectorToImageVector(triangle.x1, triangle.y1);
        v2.position = vectorToImageVector(triangle.x2, triangle.y2);
        v3.position = vectorToImageVector(triangle.x3, triangle.y3);
        va.append(std::move(v1));
        va.append(std::move(v2));
        va.append(std::move(v3));
    }
    texture.clear();
    texture.draw(va);
    texture.display();

    return texture.getTexture();
} 

class Scenario : public genetic::Scenario<Approximation>
{
    private:
    inline static const std::string name = "monalisa";
    genetic::Serializer<Approximation> serializer_;

    public: 
    Scenario()
    : serializer_(name)
    { }
    const std::string& getName()
    {
        return name;
    }
    const genetic::Serializer<Approximation>& getSerializer()
    {
        return serializer_;
    }
    
    float evaluateFitness(const Approximation& approx)
    {
        static const std::size_t NUM_PIXELS = monalisa.getSize().x * monalisa.getSize().y * 4;
        
        float sum = 0.f;
        sf::Image render = renderApproximation(approx).copyToImage();
        for (int i = 0; i < NUM_PIXELS; ++i)
        {
            if (i+1 % 4 == 0) continue; //Ignore opacity

            float target_pixel_value = static_cast<float>(monalisa.getPixelsPtr()[i]);
            float approximated_pixel_value = static_cast<float>(render.getPixelsPtr()[i]);
            float difference = target_pixel_value - approximated_pixel_value;
            float squared_difference = difference * difference;
            sum += squared_difference;
        }
        return -sum;
    }

    Approximation birth(util::RNG& rng)
    {
        Approximation approx;

        for(auto& triangle : approx)
        {
            triangle = Triangle(rng);
        }
        
        return std::move(approx);
    }

    Approximation crossover(const Approximation& a, const Approximation& b, util::RNG& rng)
    {
        Approximation c;

        std::size_t crossover_point = rng.index(NUM_TRIANGLES);
        for (int i = 0; i < NUM_TRIANGLES; ++i)
            c[i] = (i < crossover_point ? a : b)[i];
        
        return std::move(c);
    }

    void mutate(Approximation& approx, util::RNG& rng)
    {
        for(auto& triangle : approx)
        {
            auto roll = rng.real(0.f, 1.f);

            if(roll < .1f)
            {
                triangle = Triangle(rng);
            }
        }
    }
};

class View : public genetic::GraphicView<Approximation>
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
        Approximation approx = members_[i].value;
        sf::Texture tex = renderApproximation(approx);
        // Render
        target.clear();
        target.draw(sf::Sprite(tex));
        target.display();
    }

    public:
    View(): genetic::GraphicView<Approximation>(
        "Image Approximation", monalisa.getSize()
    ) {}
};

}

int main()
{
    auto cli = genetic::Controller<img::Approximation>
    (
        genetic::GeneticAlgorithm<img::Approximation>(
            std::make_unique<img::Scenario>(),
            genetic::selection::rankBased<img::Approximation>,
            10,
            .1f
        ),
        std::make_unique<img::View>()
    );
    cli.run();
    return 0;
}

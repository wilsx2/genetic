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

class Renderer {
    private:
    sf::Image img_;
    sf::RenderTexture texture_;

    sf::Vector2f pointToVector(uint16_t x, uint16_t y)
    {
        static const float max_value = static_cast<float>(std::numeric_limits<uint16_t>::max());
        return {
            (static_cast<float>(x) / max_value) * img_.getSize().x,
            (static_cast<float>(y) / max_value) * img_.getSize().y
        };
    }

    public:
    Renderer(sf::Image img)
    : img_(img)
    , texture_(img.getSize())
    { }
    Renderer(sf::Image img, float scale)
    {
        // Resize image in a render texture
        sf::Texture img_tex (img);
        sf::Sprite sprite (img_tex);
        sprite.setScale({scale, scale});
        sf::RenderTexture temp ({
            static_cast<unsigned int>(img.getSize().x * scale),
            static_cast<unsigned int>(img.getSize().y * scale)
        });
        temp.clear();
        temp.draw(sprite);
        temp.display();

        // Save to image
        img_ = temp.getTexture().copyToImage();
        (void)texture_.resize(img_.getSize());
    }
    const sf::Image& getImage()
    {
        return img_;
    }
    const sf::Texture& renderApproximation(const Approximation& approx)
    {
        sf::VertexArray va (sf::PrimitiveType::Triangles);
        for (const auto& triangle : approx)
        {
            sf::Vertex v1, v2, v3;
            v1.color = triangle.color;
            v2.color = triangle.color;
            v3.color = triangle.color;
            v1.position = pointToVector(triangle.x1, triangle.y1);
            v2.position = pointToVector(triangle.x2, triangle.y2);
            v3.position = pointToVector(triangle.x3, triangle.y3);
            va.append(std::move(v1));
            va.append(std::move(v2));
            va.append(std::move(v3));
        }
        texture_.clear();
        texture_.draw(va);
        texture_.display();

        return texture_.getTexture();
    }
};

class Scenario : public genetic::Scenario<Approximation>
{
    private:
    static constexpr int PIXEL_STRIDE = 3;
    static constexpr float SCALING_FACTOR = .25f;
    inline static const std::string name = "approx";
    Renderer renderer_;
    genetic::Serializer<Approximation> serializer_;

    uint8_t perturbInt16(uint8_t c, int magnitude, util::RNG& rng)
    {
        static constexpr int MIN = 0;
        static constexpr int MAX = static_cast<int>(std::numeric_limits<uint16_t>::max());
        return static_cast<uint16_t>(std::clamp(static_cast<int>(c + rng.integer(-magnitude/2, magnitude)), MIN, MAX));
    }
    uint8_t perturbInt8(uint8_t c, int magnitude, util::RNG& rng)
    {
        static constexpr int MIN = 0;
        static constexpr int MAX = static_cast<int>(std::numeric_limits<uint8_t>::max());
        return static_cast<uint8_t>(std::clamp(static_cast<int>(c + rng.integer(-magnitude/2, magnitude)), MIN, MAX));
    }

    public: 
    Scenario(sf::Image image)
    : renderer_(image, SCALING_FACTOR)
    , serializer_(name)
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
        static const std::size_t NUM_PIXELS = renderer_.getImage().getSize().x * renderer_.getImage().getSize().y * 4;
        
        int sum = 0;
        sf::Image render = renderer_.renderApproximation(approx).copyToImage();
        for (int i = 0; i < NUM_PIXELS; i += 4 * PIXEL_STRIDE)
        {
            for (int c = 0; c < 3; ++c) // R, G, & B only
            {
                int target_pixel_value = static_cast<int>(monalisa.getPixelsPtr()[i + c]);
                int approximated_pixel_value = static_cast<int>(render.getPixelsPtr()[i + c]);
                int difference = target_pixel_value - approximated_pixel_value;
                int squared_difference = difference * difference;
                sum += squared_difference;
            }
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
            else if (roll < .4f)
            {
                int property = rng.integer(1, 10);
                switch (property)
                {
                    case 1: triangle.x1 = perturbInt16(triangle.x1, 3000, rng); break;
                    case 2: triangle.x2 = perturbInt16(triangle.x2, 3000, rng); break;
                    case 3: triangle.x3 = perturbInt16(triangle.x3, 3000, rng); break;
                    case 4: triangle.y1 = perturbInt16(triangle.y1, 3000, rng); break;
                    case 5: triangle.y2 = perturbInt16(triangle.y2, 3000, rng); break;
                    case 6: triangle.y3 = perturbInt16(triangle.y3, 3000, rng); break;
                    case 7: triangle.color.r = perturbInt8(triangle.color.r, 10, rng); break;
                    case 8: triangle.color.g = perturbInt8(triangle.color.g, 10, rng); break;
                    case 9: triangle.color.b = perturbInt8(triangle.color.b, 10, rng); break;
                    case 10: triangle.color.a = perturbInt8(triangle.color.a, 10, rng); break;
                }
            }
        }
    }
};

class View : public genetic::GraphicView<Approximation>
{
    private:
    static constexpr float ANIMATION_DURATION = 10.f;
    Renderer renderer_;

    void render(sf::RenderWindow& target)
    {
        // Determine current member
        float percent_completed = clock_.getElapsedTime().asSeconds() / ANIMATION_DURATION;
        if (percent_completed > 1.f)
            percent_completed = 1.f;

        int i = static_cast<int>((members_.size()-1) * percent_completed);
        Approximation approx = members_[i].value;
        sf::Texture tex = renderer_.renderApproximation(approx);
        // Render
        target.clear();
        target.draw(sf::Sprite(tex));
        target.display();
    }

    public:
    View()
    : renderer_(monalisa)
    , genetic::GraphicView<Approximation>(
        "Image Approximation", monalisa.getSize()
    ) {}
};

}

int main()
{
    auto cli = genetic::Controller<img::Approximation>
    (
        genetic::GeneticAlgorithm<img::Approximation>(
            std::make_unique<img::Scenario>(img::monalisa),
            genetic::selection::rankBased<img::Approximation>,
            10,
            .1f
        ),
        std::make_unique<img::View>()
    );
    cli.run();
    return 0;
}

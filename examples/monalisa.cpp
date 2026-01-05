#include "genetic.h"
#include "monalisa.h"
#include <SFML/Graphics.hpp>
#include <array>
#include <limits>
#include <cmath>

//TODO: Constrain points within the image
namespace img
{

struct Rectangle
{
    uint16_t x1,x2,y1,y2;
    sf::Color color;

    Rectangle() = default;
    Rectangle(util::RNG& rng)
    : x1(rng.integer(0, std::numeric_limits<uint16_t>::max()))
    , x2(rng.integer(0, std::numeric_limits<uint16_t>::max()))
    , y1(rng.integer(0, std::numeric_limits<uint16_t>::max()))
    , y2(rng.integer(0, std::numeric_limits<uint16_t>::max()))
    , color({
        static_cast<uint8_t>(rng.integer(0, std::numeric_limits<uint8_t>::max())),
        static_cast<uint8_t>(rng.integer(0, std::numeric_limits<uint8_t>::max())),
        static_cast<uint8_t>(rng.integer(0, std::numeric_limits<uint8_t>::max())),
        static_cast<uint8_t>(rng.integer(64, std::numeric_limits<uint8_t>::max()))
    })
    { }
};
constexpr unsigned int NUM_RECTS = 16;
using Approximation = std::array<Rectangle, NUM_RECTS>;

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
        for (const auto& rect : approx)
        {
            sf::Vertex v1, v2, v3, v4, v5, v6;
            v1.color = rect.color;
            v2.color = rect.color;
            v3.color = rect.color;
            v4.color = rect.color;
            v5.color = rect.color;
            v6.color = rect.color;
            v1.position = pointToVector(rect.x1, rect.y1);
            v2.position = pointToVector(rect.x2, rect.y1);
            v3.position = pointToVector(rect.x1, rect.y2);
            v4.position = pointToVector(rect.x2, rect.y1);
            v5.position = pointToVector(rect.x2, rect.y2);
            v6.position = pointToVector(rect.x1, rect.y2);
            va.append(std::move(v1));
            va.append(std::move(v2));
            va.append(std::move(v3));
            va.append(std::move(v4));
            va.append(std::move(v5));
            va.append(std::move(v6));
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

    uint16_t perturbInt16(uint16_t c, util::RNG& rng)
    {
        static constexpr int MAGNITUDE = 12000;
        static constexpr int MIN = 0;
        static constexpr int MAX = static_cast<int>(std::numeric_limits<uint16_t>::max());
        return static_cast<uint16_t>(std::clamp(static_cast<int>(c + rng.integer(-MAGNITUDE/2, MAGNITUDE/2)), MIN, MAX));
    }
    uint8_t perturbInt8(uint8_t c, util::RNG& rng)
    {
        static constexpr int MAGNITUDE = 40;
        static constexpr int MIN = 0;
        static constexpr int MAX = static_cast<int>(std::numeric_limits<uint8_t>::max());
        return static_cast<uint8_t>(std::clamp(static_cast<int>(c + rng.integer(-MAGNITUDE/2, MAGNITUDE/2)), MIN, MAX));
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
                int target_pixel_value = static_cast<int>(renderer_.getImage().getPixelsPtr()[i + c]);
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

        for(auto& rect : approx)
        {
            rect = Rectangle(rng);
        }
        
        return std::move(approx);
    }

    Approximation crossover(const Approximation& a, const Approximation& b, util::RNG& rng)
    {
        Approximation c;

        std::size_t crossover_point = rng.index(NUM_RECTS);
        for (int i = 0; i < NUM_RECTS; ++i)
            c[i] = (i < crossover_point ? a : b)[i];
        
        return std::move(c);
    }

    void mutate(Approximation& approx, util::RNG& rng)
    {
        for(auto& rect : approx)
        {
            auto roll = rng.real(0.f, 1.f);

            if(roll < .02f)
            {
                rect = Rectangle(rng);
            }
            else if (roll < .15f)
            {
                int property = rng.integer(1, 8);
                switch (property)
                {
                    case 1: rect.x1 = perturbInt16(rect.x1, rng); break;
                    case 2: rect.x2 = perturbInt16(rect.x2, rng); break;
                    case 3: rect.y1 = perturbInt16(rect.y1, rng); break;
                    case 4: rect.y2 = perturbInt16(rect.y2, rng); break;
                    case 5: rect.color.r = perturbInt8(rect.color.r, rng); break;
                    case 6: rect.color.g = perturbInt8(rect.color.g, rng); break;
                    case 7: rect.color.b = perturbInt8(rect.color.b, rng); break;
                    case 8: rect.color.a = perturbInt8(rect.color.a, rng); break;
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
            genetic::selection::tournament<img::Approximation, 5>,
            200, // Population Size
            .05f // Elitism Rate
        ),
        std::make_unique<img::View>()
    );
    cli.run();
    return 0;
}

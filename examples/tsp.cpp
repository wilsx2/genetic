#include "genetic.h"
#include "graphics.hpp"
#include <memory>
#include <array>
#include <algorithm>
#include <utility>

namespace tsp 
{

    constexpr int NUM_CITIES = 50;
    using Path = std::array<int, NUM_CITIES - 1>;

    class Graph
    {
        private:
            std::array<std::pair<float, float>, NUM_CITIES> positions;
            std::array<std::array<float, NUM_CITIES>, NUM_CITIES> adjacency_matrix;
            Graph() = default;

        public:
            static Graph instance;

            void init(int seed)
            {
                util::RNG rng (seed);
                for (std::pair<float, float>& position : positions)
                {
                    position.first  = rng.real(0.f, 1.f);
                    position.second = rng.real(0.f, 1.f);
                }

                for (int i = 0; i < NUM_CITIES - 1; ++i)
                {
                    for (int j = i + 1; j < NUM_CITIES; ++j)
                    {
                        float distance = std::sqrt(
                                std::pow(positions[i].first     - positions[j].first,   2.f)
                                +  std::pow(positions[i].second    - positions[j].second,  2.f)
                        );
                        adjacency_matrix[i][j] = distance;
                        adjacency_matrix[j][i] = distance;
                    }
                }
            }
            float& weight(std::size_t from, std::size_t to)
            {
                return adjacency_matrix[from][to];
            }
            const float& weight(std::size_t from, std::size_t to) const
            {
                return adjacency_matrix[from][to];
            }
            const std::array<std::pair<float, float>, NUM_CITIES>& getPositions()
            {
                return positions;
            }
    };
    Graph Graph::instance;

    class Scenario : public genetic::Scenario<Path>
    {
        private:
        inline static const std::string name = "TSP";
        genetic::Serializer<Path> serializer_; 

        public: 
        Scenario()
        : serializer_(name)
        { }
        const std::string& getName()
        {
            return name;
        }
        const genetic::Serializer<Path>& getSerializer()
        {
            return serializer_;
        }
        float evaluateFitness(const Path& path)
        {
            float total_distance = 0.f;

            std::size_t prev = 0;
            for (int curr : path)
            {
                total_distance += Graph::instance.weight(prev, curr);
                prev = curr;
            }
            total_distance += Graph::instance.weight(prev, 0);

            return -total_distance;
        }

        Path birth(util::RNG& rng)
        {
            Path path;
            for (int i = 0; i < path.size(); ++i)
                path[i] = i + 1;
            std::shuffle(path.begin(), path.end(), rng.generator());
            return std::move(path);
        }

        void mutate(Path& path, util::RNG& rng)
        {
            int i = rng.integer(0, path.size() - 2);
            int j = rng.integer(i + 1, path.size() - 1);
            int temp = path[i];
            path[i] = path[j];
            path[j] = temp;
        }

        Path crossover(const Path& a, const Path& b, util::RNG& rng)
        {
            Path path;
            
            // Ordered crossover
            /// Copy Segment
            int crossover_point_a = rng.integer(0, path.size() - 1);
            int crossover_point_b = rng.integer(crossover_point_a, path.size() - 1);
            for (int i = crossover_point_a; i <= crossover_point_b; ++i)
                path[i] = b[i];

            // Fill in Remaining Genes
            int i = (crossover_point_b + 1) % path.size();
            int j = (crossover_point_b + 1) % path.size();
            while (i != crossover_point_a) {
                bool redundant = false;
                for (int i = crossover_point_a; i <= crossover_point_b; ++i)
                    if (path[i] == a[j])
                        redundant = true;

                if (!redundant)
                {
                    path[i] = a[j];
                    i = (i + 1) % path.size();
                }
                j = (j + 1) % path.size();
            }
            

            return std::move(path);
        }
    };

    class View : public genetic::GraphicView<Path>
    {
        private:
        static constexpr float SCALE = 800;
        static constexpr float ANIMATION_DURATION = 20.f;

        void render(sf::RenderWindow& target)
        {
            // Determine current member
            float percent_completed = clock_.getElapsedTime().asSeconds() / ANIMATION_DURATION;
            if (percent_completed > 1.f)
                percent_completed = 1.f;

            std::cout << percent_completed*100 << "%" << "\n";


            int i = static_cast<int>((members_.size()-1) * percent_completed);
            Path path = members_[i].value;

            // Rendering
            target.clear(sf::Color::Black);

            sf::CircleShape circle (.005f * SCALE, 30);
            circle.setOrigin({.005f * SCALE, .005f * SCALE});
            for(int i = 0; i < path.size(); ++i)
            {
                std::pair<float,float> position = Graph::instance.getPositions()[path[i]];
                circle.setPosition({position.first * SCALE, position.second * SCALE});
                target.draw(circle);

                std::pair<float,float> next_position;
                if (i < path.size() - 1)
                {
                    next_position = Graph::instance.getPositions()[path[i+1]];
                }
                else
                {
                    next_position = Graph::instance.getPositions()[path[0]];
                }                

                graphics::drawLine(
                    target, 
                    {
                        position.first * SCALE,
                        position.second * SCALE
                    }, 
                    {
                        next_position.first * SCALE,
                        next_position.second * SCALE
                    }, 
                    .0025 * SCALE, 
                    sf::Color::White
                );
            }

            target.display();
        }

        public:
        View(): genetic::GraphicView<Path>(
            "Traveling Salesman Problem",
            {
                static_cast<unsigned int>(SCALE),
                static_cast<unsigned int>(SCALE)
            }
        ) {}
    };
}

int main()
{
    tsp::Graph::instance.init(0);

    auto cli = genetic::Controller<tsp::Path>
    (
        genetic::GeneticAlgorithm<tsp::Path>(
            std::make_unique<tsp::Scenario>(),
            genetic::selection::rankBased<tsp::Path>,
            1000,
            .1f
        ),
        std::make_unique<tsp::View>()
    );
    cli.run();
    return 0;
}

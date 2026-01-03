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
                    position.first  = rng.real(0.f, 100.f);
                    position.second = rng.real(0.f, 100.f);
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
            // Path path;

            // int crossover_point = rng.integer(1, a.size() - 1);
            // for (int i = 0; i < crossover_point; ++i)
            // {
            //     path[i] = a[i];
            // }
            // for (int j = crossover_point; j < b.size(); ++j)
            // {
            //     path[j] = b[j];
            // }

            return a;//std::move(path);
        }
    };

    class View : public genetic::GraphicView<Path>
    {
        private:
        void render(sf::RenderWindow& target)
        {
            target.clear(sf::Color::Black);

            Path path = members_.back().value;

            sf::CircleShape circle (3.f, 30);
            circle.setOrigin({3.f,3.f});
            for(int i = 0; i < path.size(); ++i)
            {
                std::pair<float,float> position = Graph::instance.getPositions()[path[i]];
                circle.setPosition({position.first * 5, position.second * 5});
                target.draw(circle);

                if (i < path.size() - 1)
                {
                    std::pair<float,float> next_position = Graph::instance.getPositions()[path[i+1]];

                    graphics::drawLine(
                        target, 
                        {
                            position.first * 5,
                            position.second * 5
                        }, 
                        {
                            next_position.first * 5,
                            next_position.second * 5
                        }, 
                        1, 
                        sf::Color::White
                    );
                }
            }

            target.display();
        }

        public:
        View(): genetic::GraphicView<Path>("Traveling Salesman Problem", {500,500}) {}
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

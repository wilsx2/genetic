#include "genetic.h"
#include <array>
#include <algorithm>
#include <utility>

namespace tsp
{
    constexpr int NUM_CITIES = 100;
    
    class Graph
    {

        private:
            std::array<std::array<float, NUM_CITIES>, NUM_CITIES> adjacency_matrix;
            Graph() = default;

        public:
            static Graph instance;

            void init(int seed)
            {
                util::RNG rng (seed);
                std::array<std::pair<float, float>, NUM_CITIES> positions;
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
    };
    Graph Graph::instance;

    using Path = std::array<int, NUM_CITIES - 1>; // Always starts and ends at 0, ommitted

    float fitness(const Path& path)
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
        path[j] = path[i];
    }

    Path crossover(const Path& a, const Path& b, util::RNG& rng)
    {
        Path path;

        int crossover_point = rng.integer(1, a.size() - 1);
        for (int i = 0; i < crossover_point; ++i)
        {
            path[i] = a[i];
        }
        for (int j = crossover_point; j < b.size(); ++j)
        {
            path[j] = b[j];
        }

        return std::move(path);
    }
}

int main()
{
    tsp::Graph::instance.init(0);

    genetic::GeneticAlgorithm<tsp::Path>::Config conf;
    conf.problem = "tsp";
    conf.fitness = tsp::fitness;
    conf.birth = tsp::birth;
    conf.mutate = tsp::mutate;
    conf.crossover =  tsp::crossover;
    conf.select = genetic::selection::rankBased<tsp::Path>;
    conf.population_size = 1000;
    conf.elitism_rate = .1f;

    auto cli = genetic::Controller<tsp::Path>
    (
        genetic::GeneticAlgorithm<tsp::Path>(conf),
        [](const std::vector<genetic::Member<tsp::Path>>& paths, genetic::ViewType view_type)
        {
            for (float i = 0; i < paths.size(); ++i)
            {
                if (view_type == genetic::ViewType::Generations)
                    std::cout << "Generation " << i << ": ";
                else if (view_type == genetic::ViewType::Population)
                    std::cout << "Rank " << (paths.size() - i) << ": ";
                
                std::cout << "Distance " << paths[i].fitness << "\n";
            }
        }
    );
    cli.run();
    return 0;
}

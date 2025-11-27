#include "genetic.h"
#include <limits>

int main()
{
    genetic::GeneticAlgorithm<int>::Config conf;
    conf.problem = "number";
    conf.fitness = [](const int& n){return -abs(1000 - n); };
    conf.birth = [](util::RNG& rng){return rng.integer(std::numeric_limits<int>::min(), std::numeric_limits<int>::max());};
    conf.mutate = [](int& n, util::RNG& rng){return n += rng.integer(-10000,10000); };
    conf.crossover = [](const int& a, const int& b, util::RNG&){return a/2 + b/2; };

    auto cli = genetic::Controller<int>
    (
        genetic::GeneticAlgorithm<int>(conf),
        [](const std::vector<genetic::Member<int>>& nums, genetic::ViewType view_type)
        {
            for (int i = 0; i < nums.size(); ++i)
            {
                if (view_type == genetic::ViewType::Generations)
                    std::cout << "Generation " << i << ": ";
                else if (view_type == genetic::ViewType::Population)
                    std::cout << "Rank " << (nums.size() - i) << ": ";
                
                std::cout << nums[i].value << " | " << nums[i].fitness << "\n";
            }
        }
    );
    cli.run();
    return 0;
}

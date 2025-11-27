#include "genetic.h"
#include <limits>

int main()
{
    auto cli = genetic::Controller<int>
    (
        genetic::GeneticAlgorithm<int>(
            "number",
            [](const int& n){return -abs(1000 - n); },
            [](util::RNG& rng){return rng.integer(std::numeric_limits<int>::min(), std::numeric_limits<int>::max());},
            [](int& n, util::RNG& rng){return n += rng.integer(-10000,10000); },
            [](const int& a, const int& b, util::RNG&){return a/2 + b/2; },
            genetic::selection::tournament<int, 5>,
            10000,
            .1f
        ),
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

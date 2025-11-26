#include "genetic.h"
#include <limits>

int main()
{
    auto cli = Controller<int>
    (
        GeneticAlgorithm<int>(
            "number",
            [](const int& n){return -abs(1000 - n); },
            [](RNG& rng){return rng.integer(std::numeric_limits<int>::min(), std::numeric_limits<int>::max());},
            [](int& n, RNG& rng){return n += rng.integer(-10000,10000); },
            [](const int& a, const int& b, RNG&){return a/2 + b/2; },
            selection::tournament<int, 5>,
            10000,
            .1f
        ),
        [](const std::vector<Member<int>>& nums, ViewType view_type)
        {
            for (int i = 0; i < nums.size(); ++i)
            {
                if (view_type == ViewType::Generations)
                    std::cout << "Generation " << i << ": ";
                else if (view_type == ViewType::Population)
                    std::cout << "Rank " << (nums.size() - i) << ": ";
                
                std::cout << nums[i].value << " | " << nums[i].fitness << "\n";
            }
        }
    );
    cli.run();
    return 0;
}

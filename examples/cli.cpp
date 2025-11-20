#include "genetic.h"

int main()
{
    auto cli = Controller<int>
    (
        GeneticAlgorithm<int>(
            "number",
            10000,
            [](){return rand();},
            [](int& n){return -abs(1000 - n); },
            [](int& n){return n += (rand()%20000) - 10000; },
            [](int& a, int& b){return a/2 + b/2; },
            .1f,
            selection::tournament<int, 5>
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

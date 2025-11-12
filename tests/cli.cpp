#include "controller.h"

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
        [](const std::vector<Member<int>>& nums)
        {
            for (auto& num : nums)
            {
                std::cout << num.value << " | " << num.fitness << "\n";
            }
        }
    );
    cli.run();
    return 0;
}

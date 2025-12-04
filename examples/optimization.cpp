#include "genetic.h"
#include <memory>
#include <numbers>
#include <limits>

class NumberView : public genetic::View<float>
{
    protected:
    void run() {
        for (float i = 0; i < members_.size(); ++i)
        {
            if (view_type_ == genetic::ViewType::Generations)
                std::cout << "Generation " << i << ": ";
            else if (view_type_ == genetic::ViewType::Population)
                std::cout << "Rank " << (members_.size() - i) << ": ";
            
            std::cout << members_[i].value << " | " << members_[i].fitness << "\n";
        }
    }
};

float rastrigin1D(const float& x)
{
    return x*x - 10*std::cos(2*x*std::numbers::pi);
}

int main()
{
    genetic::GeneticAlgorithm<float>::Config conf;
    conf.problem = "optimization";
    conf.fitness = rastrigin1D;
    conf.birth = [](util::RNG& rng){return rng.real(-2.f, 2.f);};
    conf.mutate = [](float& n, util::RNG& rng){return n = std::min(std::max(n + rng.real(-.1f, .1f), -2.f), 2.f); };
    conf.crossover = [](const float& a, const float& b, util::RNG&){return a/2 + b/2; };
    conf.select = genetic::selection::rankBased<float>;
    conf.population_size = 10;
    conf.elitism_rate = .1f;

    auto cli = genetic::Controller<float>
    (
        genetic::GeneticAlgorithm<float>(conf),
        std::make_unique<NumberView>()
    );
    cli.run();
    return 0;
}

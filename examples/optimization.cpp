#include "genetic.h"
#include <memory>
#include <numbers>
#include <limits>

using namespace genetic;

float rastrigin1D(const float& x)
{
    return x*x - 10*std::cos(2*x*std::numbers::pi);
}

class FunctionOptimizationScenario : public Scenario<float>
{
    private:
    inline static const std::string name = "optimization";
    Serializer<float> serializer_; 

    public: 
    FunctionOptimizationScenario()
    : serializer_(name)
    { }
    const std::string& getName()
    {
        return name;
    }
    const genetic::Serializer<float>& getSerializer()
    {
        return serializer_;
    }
    
    float evaluateFitness(const float& n)
    {
        return rastrigin1D(n);
    }
    float birth(util::RNG& rng)
    {
        return rng.real(-2.f, 2.f);
    }
    float crossover(const float& a, const float& b, util::RNG& rng)
    {
        return a/2 + b/2;
    }
    void mutate(float& n, util::RNG& rng)
    {
        n = std::min(std::max(n + rng.real(-.1f, .1f), -2.f), 2.f);
    }
};

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

int main()
{
    auto cli = genetic::Controller<float>
    (
        genetic::GeneticAlgorithm<float>(
            std::make_unique<FunctionOptimizationScenario>(),
            genetic::selection::rankBased<float>,
            10,
            .1f
        ),
        std::make_unique<NumberView>()
    );
    cli.run();
    return 0;
}

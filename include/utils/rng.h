#ifndef RNG_H
#define RNG_H

#include <stdexcept>
#include <random>

namespace util
{

class RNG
{
    private:
        std::mt19937 gen_;
        
    public:
        RNG(): gen_(std::random_device()()) {}
        RNG(int seed): gen_(seed) {}

        long int integer(long int low, long int high)
        {
            if (low > high)
                throw std::invalid_argument("low must be <= high");

            std::uniform_int_distribution<long int> dist(low, high);
            return dist(gen_);
        }

        std::size_t index(std::size_t size)
        {
            std::uniform_int_distribution<std::size_t> dist(0, size - 1);
            return dist(gen_);
        }

        float real(float low, float high)
        {
            if (low > high)
                throw std::invalid_argument("low must be <= high");

            std::uniform_real_distribution<> dist(low, high);
            return dist(gen_);
        }

        std::mt19937& generator()
        {
            return gen_;
        }
};

}

#endif
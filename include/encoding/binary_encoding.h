#ifndef BINARY_ENCODING_H
#define BINARY_ENCODING_H

#include "utils/rng.h"
#include <bitset>

namespace genetic 
{

template <typename T>
class BinaryEncoding {
    static_assert(std::is_trivially_copyable_v<T> == true);

    private:
        std::bitset<sizeof(T)*8> data_;

    public:
        BinaryEncoding();
        BinaryEncoding(T value);
        T get() const;
        void set(T value);
        const std::bitset<sizeof(T)*8>& data() const;

        static BinaryEncoding birth(util::RNG& rng);
        static BinaryEncoding crossover(const BinaryEncoding& a, const BinaryEncoding& b, util::RNG& rng);
        template <int R> static void mutate(BinaryEncoding& bin, util::RNG& rng);
};

}

#include "binary_encoding.tpp"

#endif
#ifndef BINARY_ENCODING_H
#define BINARY_ENCODING_H

#include <bitset>

template <typename T>
class BinaryEncoding {
    private:
        std::bitset<sizeof(T)*8> data_;

    public:
        BinaryEncoding();
        BinaryEncoding(T value);
        T get() const;
        void set(T value);
        const std::bitset<sizeof(T)*8>& data() const;

        static BinaryEncoding birth();
        static BinaryEncoding crossover(BinaryEncoding& a, BinaryEncoding& b);
        template <int R> static void mutate(BinaryEncoding& bin);
};

#include "binary_encoding.tpp"

#endif
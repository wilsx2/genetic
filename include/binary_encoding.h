#ifndef BINARY_ENCODING_H
#define BINARY_ENCODING_H

#include <bitset>

template <typename T>
class BinaryEncoding {
    std::bitset<sizeof(T)*8> data_;

    BinaryEncoding(T value);
    T get();
    void set(T value);

    static BinaryEncoding birth();
    static BinaryEncoding crossover(BinaryEncoding& a, BinaryEncoding& b);
    static void mutate(BinaryEncoding& bin);
};

#endif
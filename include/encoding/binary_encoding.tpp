#include "binary_encoding.h"
#include <cstdlib>

template <typename T>
BinaryEncoding<T>::BinaryEncoding()
    : data_()
{}

template <typename T>
BinaryEncoding<T>::BinaryEncoding(T value)
    : data_(value)
{}

template <typename T>
T BinaryEncoding<T>::get() const
{
    T result;
    
    // Convert bitset to bytes, write into result.
    for (size_t i = 0; i < sizeof(T); ++i) {
        unsigned char byte = 0;
        for (size_t j = 0; j < 8; ++j) {
            if (data_[i * 8 + j]) {
                byte |= (1 << j);
            }
        }
        reinterpret_cast<unsigned char*>(&result)[i] = byte;
    }
    
    return result;
}

template <typename T>
void BinaryEncoding<T>::set(T value)
{
    data_ = std::bitset<sizeof(T)>(value);
}

template <typename T>
const std::bitset<sizeof(T)*8>& BinaryEncoding<T>::data() const
{
    return data_;
}

template <typename T>
BinaryEncoding<T> BinaryEncoding<T>::birth()
{
    BinaryEncoding<T> baby;
    for (std::size_t i = 0; i < baby.data_.size(); ++i)
        if(rand() % 2) baby.data_.flip(i);
    return baby;
}

template <typename T>
BinaryEncoding<T> BinaryEncoding<T>::crossover(BinaryEncoding<T>& a, BinaryEncoding<T>& b)
{
    BinaryEncoding<T> offspring;

    std::size_t crossover_point = rand() % offspring.data_.size();
    for (int i = 0; i < offspring.data_.size(); ++i)
    {
        offspring.data_.set(i, i < crossover_point ? a.data_[i] : b.data_[i]);
    }

    return offspring;
}

template <typename T>
template <int R>
void BinaryEncoding<T>::mutate(BinaryEncoding& bin)
{
    float mutation_rate = static_cast<float>((rand() % R) % 100) / 100.f;
    for (int i = 0; i < bin.data_.size() * mutation_rate; ++i)
    {
        bin.data_.flip(rand() % bin.data_.size());
    }
}
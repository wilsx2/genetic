#ifndef BIN_SCENARIO_H
#define BIN_SCENARIO_H

#include "scenario.h"
#include "serialization/serializer.h"
#include "encoding/binary_encoding.h"
#include "utils/rng.h"
#include <string>

namespace genetic
{

template <typename T>
class BinaryEncodedScenario : public Scenario<BinaryEncoding<T>>
{
    protected:
    using BinT = BinaryEncoding<T>;

    private: 
    Serializer<BinT> serializer_; 

    public: 
    BinaryEncodedScenario(std::string name)
    : serializer_(name)
    { }

    const Serializer<BinT>& getSerializer();
    BinT birth(util::RNG&);
    BinT crossover(const BinT&, const BinT&, util::RNG&);
    void mutate(BinT&, util::RNG&);
};

}

#include "binary_scenario.tpp"
#endif
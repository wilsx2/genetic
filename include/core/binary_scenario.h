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
    public: 
    T birth(util::RNG&);
    T crossover(const T&, const T&, util::RNG&);
    void mutate(T&, util::RNG&);
};

}

#endif
#ifndef POPULATION_HISTORY_H
#define POPULATION_HISTORY_H

#include "serialization/serializer.h"
#include "member.h"
#include "generation.h"
#include <string>
#include <vector>
#include <cstdint>

template <typename T>
class PopulationHistory {
    friend class Serializer<T>;

    private:
        uint32_t id_;
        std::size_t population_size_;
        std::vector<Generation<T>> generations_;
        std::vector<Member<T>> fittest_history_;

    public:
        PopulationHistory(uint32_t id, std::size_t population_size);
        uint32_t id() const;
        std::string formattedId() const;
        std::size_t populationSize() const;
        std::size_t numGenerations() const;
        const std::vector<Generation<T>>& getGenerations() const;
        const Generation<T>& current() const;
        const std::vector<Member<T>>& fittestHistory() const;
        void pushNext(std::vector<Member<T>>&& next);
        void restart(uint32_t new_id, std::size_t new_size);

        const Member<T>& getFittest() const;
        float getFittestScore() const;
};

#include "population_history.tpp"
#endif
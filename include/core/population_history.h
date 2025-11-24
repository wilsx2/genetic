#ifndef POPULATION_HISTORY_H
#define POPULATION_HISTORY_H

#include "member.h"
#include <vector>

template <typename T>
class PopulationHistory {
    private:
        std::size_t population_size_;
        std::vector<std::vector<Member<T>>> generations_;
        std::vector<Member<T>> fittest_history_;

    public:
        PopulationHistory(std::size_t population_size);
        std::size_t populationSize() const;
        std::size_t numGenerations() const;
        const std::vector<std::vector<Member<T>>>& getGenerations() const;
        const std::vector<Member<T>>& getCurrent() const;
        const std::vector<Member<T>>& getFittestHistory() const;
        void pushNext(std::vector<Member<T>>&& next);
        void clear(std::size_t new_size);

        const Member<T>& getFittest() const;
        const Member<T>& getFittest(std::size_t generation) const;
        const Member<T>& getFittestScore() const;
        const Member<T>& getFittestScore(std::size_t generation) const;
};

#include "population_history.tpp"
#endif
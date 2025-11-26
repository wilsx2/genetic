#ifndef GENERATION_H
#define GENERATION_H

#include "member.h"

template <typename T>
class Generation {
    friend class Serializer<T>;

    private:
        std::vector<Member<T>> members_;
        float total_fitness_; // Relevant to some selection functions
    
    public:
        Generation(std::vector<Member<T>>&& members);
        const Member<T>& operator[](std::size_t index) const;
        const std::vector<Member<T>>& members() const;
        std::size_t size() const;
        const Member<T>& fittest() const;
        float fittestScore() const;
        float totalFitness() const;
};

#include "generation.tpp"
#endif
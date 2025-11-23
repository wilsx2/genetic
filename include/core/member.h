#ifndef MEMBER_H
#define MEMBER_H

#include <compare>

template <typename T>
struct Member
{
    T value;
    float fitness;

    auto operator<=>(const Member& other) const {
        return fitness <=> other.fitness;
    }

    bool operator==(const Member& other) const {
        return fitness == other.fitness;
    }
};

#endif
#ifndef SERIALIZER_H
#define SERIALIZER_H

#include "core/member.h"
#include <vector>
#include <string>
#include <filesystem>
#include <optional>
#include <cstdint>

template <typename T>
struct PopulationData {
    uint32_t identifier;
    std::vector<Member<T>> current_population;
    std::vector<Member<T>> fittest_history;
};

template <typename T>
class Serializer
{
    static_assert(std::is_trivially_copyable_v<T>, "T must be trivially copyable for binary serialization");

    private:
        const std::string save_directory_;
        
        std::string formatFilename(uint32_t id, std::size_t generation, float fitness) const;
        std::optional<std::filesystem::path> findPopulationFile(const std::string& id_prefix) const;
        
    public:
        PopulationSerializer(std::string problem_name);

        bool save(const PopulationData<T>& data, std::size_t generation, float fitness);
        std::optional<PopulationData<T>> load(const std::string& id_prefix);
};

#include "serializer.tpp"
#endif
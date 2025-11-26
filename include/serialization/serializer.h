#ifndef SERIALIZER_H
#define SERIALIZER_H

#include "core/member.h"
#include "core/population_history.h"
#include <vector>
#include <string>
#include <filesystem>
#include <optional>
#include <cstdint>

template <typename T>
class PopulationHistory;

template <typename T>
class Serializer
{
    static_assert(std::is_trivially_copyable_v<T>, "T must be trivially copyable for binary serialization");

    private:
        static constexpr std::size_t ID_STRING_SIZE = sizeof(uint32_t)*2;
        const std::string save_directory_;
        
        std::string formatFilename(uint32_t id, std::size_t generation, float fitness) const;
        std::optional<std::filesystem::path> findPopulationFile(const std::string& id) const;
        std::optional<std::filesystem::path> findPopulationFile(uint32_t id) const;
        
    public:
        Serializer(std::string problem_name);

        bool save(PopulationHistory<T>& pop) const;
        std::optional<PopulationHistory<T>> load(const std::string& id) const;
        std::vector<std::string> getSaves() const;
        bool deleteSave(const std::string& id) const;
        bool deleteAllSaves() const;
};

#include "serializer.tpp"
#endif
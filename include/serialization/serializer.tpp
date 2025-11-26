#include "serializer.h"
#include <filesystem>
#include <format>
#include <iostream>
#include <fstream>

template <typename T>
Serializer<T>::Serializer(std::string problem_name):
    save_directory_("populations/"+problem_name+"/")
{}

template <typename T>
std::string Serializer<T>::formatFilename(uint32_t id, std::size_t generation, float fitness) const
{
    return std::format("{:08x} G{} F{}", id, generation, fitness);
}

template <typename T>
std::optional<std::filesystem::path> Serializer<T>::findPopulationFile(uint32_t id) const
{
    return findPopulationFile(std::format("{:08x}", id));
}

template <typename T>
std::optional<std::filesystem::path> Serializer<T>::findPopulationFile(const std::string& id) const
{
    if (id.size() != ID_STRING_SIZE)
    {
        return std::nullopt;
    }

    if (!std::filesystem::exists(save_directory_) || !std::filesystem::is_directory(save_directory_))
    {
        return std::nullopt;
    }

    for (const auto& entry : std::filesystem::directory_iterator(save_directory_))
    {
        if (!entry.is_regular_file())
            continue;

        const auto filename = entry.path().filename().string();
        if (filename.rfind(id, 0) == 0)
        {
            return entry.path();
        }
    }

    return std::nullopt;
}

template <typename T>
bool Serializer<T>::save(PopulationHistory<T>& pop) const
{
    // TODO: delete created file if write fails
    // Check that there is data to save
    if (pop.numGenerations() == 0) {
        return false;
    } 

    // Ensure there's a place for our save
    std::filesystem::create_directories(save_directory_);

    // Overwrite previous save of this population, if it exists
    std::optional<std::filesystem::path> path = findPopulationFile(pop.id_);
    if (path.has_value())
        std::filesystem::remove(path.value());

    // Begin saving
    std::ofstream output (save_directory_+formatFilename(pop.id_, pop.numGenerations(), pop.getFittestScore()));
    
    if (!output.is_open()) {
        std::cerr << "Failed to create save file\n";
        return false;
    }

    // Tag With Type ID
    std::size_t typeHash = typeid(T).hash_code();
    output.write(reinterpret_cast<char*>(&typeHash), sizeof(std::size_t));
    if (!output.good())
    {
        std::cerr << "Failed to write type id\n";
        return false;
    }

    // Identifier
    output.write(reinterpret_cast<char*>(&pop.id_), sizeof(uint32_t));
    if (!output.good())
    {
        std::cerr << "Failed to write population identifier\n";
        return false;
    }

    // Population Size
    output.write(reinterpret_cast<char*>(&pop.population_size_), sizeof(uint32_t));
    if (!output.good())
    {
        std::cerr << "Failed to write population size\n";
        return false;
    }

    // Number of Generations
    std::size_t num_gens = pop.generations_.size();
    output.write(reinterpret_cast<char*>(&num_gens), sizeof(std::size_t));
    if (!output.good())
    {
        std::cerr << "Failed to write number of generations\n";
        return false;
    }
    
        
    // Generations
    for (int i = 0; i < num_gens; ++i)
    {
        output.write(reinterpret_cast<char*>(pop.generations_[i].members_.data()), sizeof(Member<T>)*pop.population_size_);
        if (!output.good())
        {
            std::cerr << "Failed to write generation " << (i + 1) << "\n";
            return false;
        }
    }

    output.close();
    return true;
}

template <typename T>
std::optional<PopulationHistory<T>> Serializer<T>::load(const std::string& id) const
{
    // Search for file with id provided
    std::optional<std::filesystem::path> path = findPopulationFile(id);
    if (!path.has_value())
    {
        std::cerr << "No save file in the \"" << save_directory_ <<
        "\" directory has the id \"" << id << "\"\n";
        return std::nullopt;
    }

    // Begin loading
    std::ifstream input (path.value().string());
    if (!input.is_open())
    {
        std::cerr << "Failed to open file \"" << path.value().string() << "\"\n";
        return std::nullopt;
    }
    
    // Check Type ID tag
    std::size_t inputTypeHash;
    input.read(reinterpret_cast<char*>(&inputTypeHash), sizeof(std::size_t));
    if (inputTypeHash != typeid(T).hash_code())
    {
        std::cerr << "File \"" << path.value().string()
            << "\" stores a population of a type other than \""
            << typeid(T).name() << "\"\n";
        return std::nullopt;
    }

    // Identifier
    PopulationHistory<T> pop (0, 1);

    input.read(reinterpret_cast<char*>(&pop.id_), sizeof(u_int32_t));
    if (!input.good())
    {
        std::cerr << "Failed to read population identifier";
        return std::nullopt;
    }

    // Population Size
    input.read(reinterpret_cast<char*>(&pop.population_size_), sizeof(uint32_t));
    if (!input.good())
    {
        std::cerr << "Failed to read population size\n";
        return std::nullopt;
    }

    // Number of Generations
    std::size_t num_gens;
    input.read(reinterpret_cast<char*>(&num_gens), sizeof(std::size_t));
    if (!input.good())
    {
        std::cerr << "Failed to read number of generations\n";
        return std::nullopt;
    }
        
    // Generations
    for (int i = 0; i < num_gens; ++i)
    {
        std::vector<Member<T>> next;
        next.resize(pop.population_size_);
        input.read(reinterpret_cast<char*>(next.data()), sizeof(Member<T>)*pop.population_size_);
        if (!input.good())
        {
            std::cerr << "Failed to read generation " << (i + 1) << "\n";
            return std::nullopt;
        }
        pop.pushNext(std::move(next));
    }

    input.close();

    return std::move(pop);
}

template <typename T>
std::vector<std::string> Serializer<T>::getSaves() const
{
    if (!std::filesystem::exists(save_directory_) || !std::filesystem::is_directory(save_directory_))
    {
        return {};
    }

    std::vector<std::string> saves_ids;
    for (const auto& entry : std::filesystem::directory_iterator(save_directory_))
    {
        if (entry.is_regular_file())
            saves_ids.emplace_back(entry.path().filename().string());
    }

    return std::move(saves_ids);
}

template <typename T>
bool Serializer<T>::deleteSave(const std::string& id) const
{
    std::optional<std::filesystem::path> path = findPopulationFile(id);

    
    if (!path.has_value()) 
    {
        std::cerr << "No save file in the \"" << save_directory_ <<
        "\" directory has the id \"" << id << "\"\n";
        return false;
    }
    if (!std::filesystem::remove(path.value()))
    {
        std::cerr << "Failed to remove " << path.value().string() << "\n";
        return false;
    }
    return true;
}

template <typename T>
bool Serializer<T>::deleteAllSaves() const
{
    if (!std::filesystem::exists(save_directory_))
    {
        std::cerr << "Directory \"" << save_directory_ << "\" does not exist\n";
        return false;
    }

    if (std::filesystem::is_empty(save_directory_))
    {
        std::cerr << "Directory \"" << save_directory_ << "\" is empty\n";
        return false;
    }
    
    std::filesystem::remove_all(save_directory_);
    return false;
}
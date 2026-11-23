#pragma once

#include <string>
#include <utility>
#include <vector>
#include <unordered_map>
#include <variant>
#include <cstdint>
#include <optional>

struct CoverArt {
    CoverArt(std::vector<std::uint8_t> data, std::string file_type, std::string description = "None") :
        data(std::move(data)),
        file_type(std::move(file_type)),
        description(std::move(description)) 
    {}

    std::vector<std::uint8_t> data;
    std::string file_type;
    std::string description;
};

class Metadata {
public:
    enum class Status {
        not_loaded,
        success,
        file_not_found,
        no_tags_found
    };
    
    using TagMap = std::unordered_map<std::string, std::variant<std::string, int>>;
    Metadata(std::string song_path);

    Metadata& load_metadata();
    Status get_status() const;

    bool check_status(Status stat) const;
    std::vector<std::string> discovered_tags() const;

    const TagMap& get_tags() const;


    operator bool() const;


    TagMap::const_iterator begin() const; // Pra permitir iteração sobre nosso objeto
    TagMap::const_iterator end() const;

    std::variant<std::string, int> operator[](const std::string& key) const;

    bool contains(const std::string& item) const;

    std::optional<CoverArt> get_cover() const;
    
private:
    std::string song_path;
    Status status = Status::not_loaded;

    TagMap tags;
};
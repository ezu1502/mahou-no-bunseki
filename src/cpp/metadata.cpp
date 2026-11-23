#include <string>
#include <utility>
#include <vector>
#include <unordered_map>
#include <initializer_list>
#include <variant>

#include <taglib/fileref.h>
#include <taglib/tag.h>
#include <taglib/audioproperties.h>

#include <bunseki/metadata.hpp>


Metadata::Metadata(std::string song_path) : song_path(std::move(song_path)) {}

Metadata& Metadata::load_metadata(){
    using namespace TagLib;

    tags.clear();
    status = Status::not_loaded;

    FileRef audio_file(song_path.c_str());
    
    if (audio_file.isNull()){
        status = Status::file_not_found;
        return *this;
    }
    
    Tag* tag = audio_file.tag();

    if (tag->isEmpty()){
        status = Status::no_tags_found;
        return *this;
    }

    #define READ_TAG(name) \
        std::string name = tag->name().to8Bit(true)
    
    // .to8Bit converte de TagLib::String pra std::string, (true) mantém em UTF-8
    
    READ_TAG(title);
    READ_TAG(artist);
    READ_TAG(album);
    READ_TAG(comment);
    READ_TAG(genre);
    int year = tag->year();
    int track = tag->track();

    #undef READ_TAG

    for (const auto& [key, value] : { // Adiciona as strings não vazias ao nosso mapa de tags
        std::pair{"title", title},
        std::pair{"artist", artist},
        std::pair{"album", album},
        std::pair{"comment", comment},
        std::pair{"genre", genre}
    }) {
        if (!value.empty()) {
            tags[key] = value;
        }
    }

    if (year != 0){
        tags["year"] = year;
    }        
    if (track != 0){
        tags["track"] = track;
    }

    this->status = Status::success;
    return *this; 
}

Metadata::Status Metadata::get_status() const{
    return this->status;
}

bool Metadata::check_status(Status stat) const {
    return this->status == stat;
}

std::vector<std::string> Metadata::discovered_tags() const {
    std::vector<std::string> result;

    for (const auto& t : tags){
        result.push_back(t.first);
    }

    return result;
}

const Metadata::TagMap& Metadata::get_tags() const {
    return this->tags;
}


Metadata::operator bool() const {
    return check_status(Status::success);
}

Metadata::TagMap::const_iterator Metadata::begin() const{ // Serve pra permitir que Metadata seja iterável, estilo __iter__
    return tags.cbegin();
}

Metadata::TagMap::const_iterator Metadata::end() const {
    return tags.cend();
}

std::variant<std::string, int> Metadata::operator[](const std::string& key) const {
    return tags.at(key); // Mesma coisa que tags[key], mas lança erro se n existir
}

bool Metadata::contains(const std::string& item) const{
    return tags.contains(item);
}

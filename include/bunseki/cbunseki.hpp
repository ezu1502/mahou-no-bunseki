#pragma once

#include <vector>
#include <utility>

#include <bunseki/analysis.hpp>
#include <bunseki/transform.hpp>

class CBunseki {
public:
    explicit CBunseki(std::vector<float> samples);

    std::vector<float> samples;
    
    std::size_t frame_count() const {
        return this->samples.size();
    }

    Analysis analysis;
    Transform transform;
};
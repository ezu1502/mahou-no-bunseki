#pragma once

#include <vector>
#include <utility>

#include <bunseki/analysis.hpp>
#include <bunseki/transform.hpp>

std::vector<float> samples_to_mono(std::vector<float> samples);

class CBunseki {
public:
    explicit CBunseki(std::vector<float> samples);

    std::vector<float> samples;
    std::size_t frame_count;

    Analysis analysis;
    Transform transform;
};
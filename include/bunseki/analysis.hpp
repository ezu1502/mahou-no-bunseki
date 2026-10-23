#pragma once

#include <vector>
#include <utility>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <stdexcept>

#include <bunseki/constants.hpp>

class Analysis {
public:
    Analysis(const std::vector<float>& samples);

    const std::vector<float>& samples; // já recebe convertida pra mono

    float rms();    
    float rms(const std::vector<float>& vec);

    float p2p_amplitude();
    float peak_amplitude();
    float zero_crossing_rate();

    std::vector<std::pair<float, float>> silence_detection(float threshold = 0.01); 
    std::vector<float> rms_windows(std::size_t window_size, std::size_t hop_size);

    std::vector<std::size_t> onset_detection(float threshold);
    std::vector<std::size_t> onset_detection(const std::vector<float>& rms_values, float threshold);

    std::vector<std::size_t> onset_intervals(const std::vector<std::size_t>& onsets);

    // float bpm();
    // float bpm(const std::vector<std::size_t>& intervals);
private:
    std::size_t frame_count;
};
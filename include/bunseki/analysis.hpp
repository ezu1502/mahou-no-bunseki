#pragma once

#include <vector>
#include <utility>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <stdexcept>

#include <bunseki/buntools.hpp>

class Analysis {
public:
    Analysis(const std::vector<float>& samples);

    const std::vector<float>& samples; // já recebe convertida pra mono

    float rms() const;    

    float p2p_amplitude() const;
    float peak_amplitude() const;
    float zero_crossing_rate() const;

    std::vector<std::pair<float, float>> silence_detection(float threshold = 0.01f) const; 

    std::vector<float> rms_windows(std::size_t window_size, std::size_t hop_size) const;

    std::vector<std::size_t> onset_detection(float threshold, int min_spacing = 2) const;

    std::vector<std::size_t> onset_intervals() const;

    float onset_bpm() const;

    std::vector<float> auto_correlation() const;

    std::vector<std::size_t> peak_detection(const std::vector<float>& rms_values) const;

    float lag_bpm() const;
private:
    std::size_t frame_count() const {
        return this->samples.size();
    }

    float rms(const std::vector<float>& vec) const;

    std::vector<std::size_t> onset_detection(
        const std::vector<float>& rms_values,
        float threshold,
        int min_spacing = 2
    ) const;

    float onset_bpm(const std::vector<std::size_t>& intervals, std::size_t hop_size) const;

    std::vector<float> auto_correlation(const std::vector<float>& rms_windows) const;

    std::vector<std::size_t> onset_intervals(const std::vector<std::size_t>& onsets) const;


};
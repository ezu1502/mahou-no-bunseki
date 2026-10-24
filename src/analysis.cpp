#include <vector>
#include <utility>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <iostream>

#include <bunseki/analysis.hpp>

Analysis::Analysis(const std::vector<float>& samples) : samples(samples), frame_count(this->samples.size()){}

float Analysis::rms(){
    return rms(samples);
}

float Analysis::rms(const std::vector<float>& vec){
    if (vec.empty()){
        return 0.0f;
    }
    double sum = 0.0;
    for (float sample : vec){
        sum += (static_cast<double>(sample) * sample);
    }

    return std::sqrt(sum / vec.size());
}

float Analysis::p2p_amplitude(){
    if (samples.empty()){
        return 0.0f;
    }

    float smallest = samples[0];
    float greatest = samples[0];

    for (float sample : samples){
        if (sample < smallest){
            smallest = sample;
        }
        if (sample > greatest){
            greatest = sample;
        }
    }

    return greatest - smallest;
}

float Analysis::peak_amplitude(){
    float greatest = 0.0f;

    for (float sample : samples){
        float abs = std::abs(sample);
        if (abs > greatest){
            greatest = abs;
        }
    }

    return greatest;
}

float Analysis::zero_crossing_rate(){
    if (samples.empty()){
        return 0.0f;
    }

    int crosses = 0;

    for (std::size_t i = 1; i < samples.size(); i++){
        if ((samples[i] > 0 && samples[i-1] < 0) || (samples[i] < 0 && samples[i-1] > 0)){
            crosses++;
        }
    }

    return crosses / static_cast<float>(samples.size() - 1);
}

std::vector<std::pair<float, float>> Analysis::silence_detection(float threshold){
    if (threshold <= 0){
        throw std::invalid_argument("Threshold must be greater than 0");
    }
    using Interval = std::pair<float, float>;

    std::vector<Interval> result;
    std::int64_t open_silence_index = -1;

    for (std::size_t i = 0; i < samples.size(); i++){
        if (std::abs(samples[i]) <= threshold){
            if (open_silence_index < 0){
                open_silence_index = i;
            }
        }
        else {
            if (open_silence_index >= 0){
                result.emplace_back((open_silence_index) / static_cast<float>(SAMPLE_RATE), i / static_cast<float>(SAMPLE_RATE));
                open_silence_index = -1;
            }
        }
    }

    if (open_silence_index >= 0){
        result.emplace_back(open_silence_index / static_cast<float>(SAMPLE_RATE), samples.size() / static_cast<float>(SAMPLE_RATE));
    }


    return result;
}

std::vector<float> Analysis::rms_windows(std::size_t window_size, std::size_t hop_size){
    if (window_size == 0 || hop_size == 0){
        throw std::invalid_argument("Window size and hop size must be positive values");
    }
    if (samples.empty()){
        throw std::runtime_error("Samples is empty!");
    }

    std::vector<float> result;

    for (std::size_t i = 0, len = samples.size(); i <= len && window_size <= len - i; i += hop_size){
        std::vector<float> window(
            samples.begin() + i,
            samples.begin() + i + window_size
        );

        result.push_back(rms(std::move(window)));
    }

    return result;
}

std::vector<std::size_t> Analysis::onset_detection(float threshold){
    return onset_detection(samples, threshold);
}

std::vector<std::size_t> Analysis::onset_detection(const std::vector<float>& rms_values, float threshold){
    if (rms_values.empty()){
        throw std::runtime_error("rms_values is empty!");
    }
    if (threshold <= 0){
        throw std::invalid_argument("Threshold must be a positive value");
    }

    std::vector<std::size_t> result;
    
    for (std::size_t i = 1, len = rms_values.size(); i < len; i++){
        if ((rms_values[i] - rms_values[i-1] > threshold) && (result.empty() || i - result.back() >= 2)){
            result.push_back(i);
        }
    }

    return result;
}

std::vector<std::size_t> onset_intervals(const std::vector<std::size_t>& onsets){
    std::vector<std::size_t> intervals;

    for (std::size_t i = 1; i < onsets.size(); i++){
        intervals.push_back(onsets[i] - onsets[i-1]);
    }

    return intervals;
}
/* 
float bpm(const std::vector<std::size_t>& intervals, std::size_t hop_size){
    if (intervals.empty() || hop_size == 0){
        throw std::invalid_argument("Intervals and hop size must be valid");
    }

    float sum = 0;

    for (auto interval : intervals){
        sum += interval;
    }

    float mean = sum / intervals.size();

    return 60 * SAMPLE_RATE / (mean * hop_size);
} */

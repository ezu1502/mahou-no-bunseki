#include <vector>
#include <utility>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <iostream>

#include <bunseki/analysis.hpp>

Analysis::Analysis(const std::vector<float>& samples) : samples(samples){}

// * PUBLIC

float Analysis::rms() const{
    return rms(samples);
}

float Analysis::p2p_amplitude() const{
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

float Analysis::peak_amplitude() const{
    float greatest = 0.0f;

    for (float sample : samples){
        float abs = std::abs(sample);
        if (abs > greatest){
            greatest = abs;
        }
    }

    return greatest;
}

float Analysis::zero_crossing_rate() const{
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

std::vector<std::pair<float, float>> Analysis::silence_detection(float threshold) const{
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

std::vector<float> Analysis::rms_windows(std::size_t window_size, std::size_t hop_size) const{
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

// PUBLIC::TEMPO

std::vector<std::size_t> Analysis::onset_detection(float threshold, int min_spacing) const{
    return onset_detection(rms_windows(STANDARD_WINDOW_SIZE, STANDARD_HOP_SIZE), threshold, min_spacing);
}

std::vector<std::size_t> Analysis::onset_intervals() const{
    return onset_intervals(onset_detection(rms_windows(STANDARD_WINDOW_SIZE, STANDARD_HOP_SIZE), 0.01f, 5));
}

float Analysis::onset_bpm() const{
    auto onsets = onset_detection(0.05f, 5); // magic numbers, cuidado

    if (onsets.size() < 2){
        return 0.0f;
    }

    auto intervals = onset_intervals(onsets);

    return onset_bpm(intervals, STANDARD_HOP_SIZE);
}

std::vector<float> Analysis::auto_correlation() const{
    return auto_correlation(rms_windows(STANDARD_WINDOW_SIZE, STANDARD_HOP_SIZE));
}

std::vector<std::size_t> Analysis::peak_detection(const std::vector<float>& rms_values) const{
    if (rms_values.empty()){
        throw std::runtime_error("rms_values is empty!");
    }

    std::vector<std::size_t> result;
    
    for (std::size_t i = 1, len = rms_values.size(); i + 1 < len; i++){
        if (rms_values[i] > rms_values[i-1] && rms_values[i] > rms_values[i+1]){
            result.push_back(i);
        }
    }
    return result;
}

float Analysis::lag_bpm() const{
    std::vector<float> correlation = auto_correlation();

    std::vector<std::size_t> peaks = peak_detection(correlation);

    std::vector<std::size_t> filtered;

    for (std::size_t peak_index : peaks){
        if (peak_index >= 30 && peak_index <= 100){
            filtered.push_back(peak_index);
        }

        // if (peak_index >= 30 && peak_index <= 100) {
        //     return 60.0f * SAMPLE_RATE /
        //            (peak_index * STANDARD_HOP_SIZE);
        // }
        //  o método acima só escolhe o primeiro pico
    }

    if (filtered.empty()){
        return 0.0f;
    }

    std::size_t best_peak = filtered[0];

    auto score = [&correlation](std::size_t index){
        float a = 1.0f;
        float b = 0.0f; // antes era 0.5f
        float c = 0.0f; // antes era 0.25f

        // desativados para testes

        return a * correlation[index] + b * correlation[index * 2] + c * correlation[index * 3];
    };

    for (std::size_t peak_index : filtered){
        if (score(peak_index) > score(best_peak)){
            best_peak = peak_index;
        }
    }

    return 60.0f * SAMPLE_RATE / (best_peak * STANDARD_HOP_SIZE);
}


// * PRIVATE

float Analysis::rms(const std::vector<float>& vec) const{
    if (vec.empty()){
        return 0.0f;
    }
    double sum = 0.0;
    for (float sample : vec){
        sum += (static_cast<double>(sample) * sample);
    }

    return std::sqrt(sum / vec.size());
}

std::vector<std::size_t> Analysis::onset_detection(const std::vector<float>& rms_values, float threshold, int min_spacing) const{
    if (rms_values.empty()){
        throw std::runtime_error("rms_values is empty!");
    }
    if (threshold <= 0){
        throw std::invalid_argument("Threshold must be a positive value");
    }

    std::vector<std::size_t> result;
    
    for (std::size_t i = 1, len = rms_values.size(); i < len; i++){
        if ((rms_values[i] - rms_values[i-1] > threshold) && (result.empty() || i - result.back() >= min_spacing)){
            result.push_back(i);
        }
    }

    return result;
}

std::vector<std::size_t> Analysis::onset_intervals(const std::vector<std::size_t>& onsets) const{
    std::vector<std::size_t> intervals;

    for (std::size_t i = 1; i < onsets.size(); i++){
        intervals.push_back(onsets[i] - onsets[i-1]);
    }

    return intervals;
}

float Analysis::onset_bpm(const std::vector<std::size_t>& intervals, std::size_t hop_size) const{
    if (intervals.empty() || hop_size == 0){
        throw std::invalid_argument("Intervals and hop size must be valid");
    }

    float sum = 0;

    for (auto interval : intervals){
        sum += interval;
    }

    float mean = sum / intervals.size();

    if (mean == 0){
        return 0.0f;
    }

    return 60 * SAMPLE_RATE / (mean * hop_size);
}

std::vector<float> Analysis::auto_correlation(const std::vector<float>& rms_windows) const{
    if (rms_windows.empty()){
        throw std::invalid_argument("Rms_windows must not be empty");
    }

    std::size_t len = rms_windows.size();
    std::vector<float> result(len);



    for (std::size_t k = 0; k < len; k++){
        float k_result = 0;

        for (std::size_t i = 0; i <= len - k - 1; i++){
            k_result += rms_windows[i] * rms_windows[i + k];
        }

        result[k] = k_result;
    }

    return result;
}

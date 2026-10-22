#include <vector>
#include <utility>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <stdexcept>

constexpr std::int64_t SAMPLE_RATE = 44100;

class Analysis {
public:
    Analysis(const std::vector<float>& samples);

    const std::vector<float>& samples; // já recebe convertida pra mono
    std::size_t frame_count;

    float rms();
    float p2p_amplitude();
    float peak_amplitude();
    float zero_crossing_rate();

    std::vector<std::pair<float, float>> silence_detection(float threshold = 0.01); 

};

Analysis::Analysis(const std::vector<float>& samples) : samples(samples), frame_count(this->samples.size()){}

float Analysis::rms(){
    if (samples.empty()){
        return 0.0f;
    }

    double sum = 0.0;
    for (float sample : samples){
        sum += (sample * sample);
    }

    return std::sqrt(sum / frame_count);
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
        throw std::invalid_argument("Threshold must be non-negative!");
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
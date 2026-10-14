#include <vector>
#include <cstdint>
#include <utility>
#include <cmath>

#include "analysis.hpp"


double round_to(double number, int decimal_places){
    double multiplier = std::pow(10.0, decimal_places);
    return std::round(number * multiplier) / multiplier;
}

Analysis::Analysis(std::vector<float> samples, std::uint64_t frame_count)
    : samples(std::move(samples)), frame_count(static_cast<std::uint64_t>(frame_count)){}

bool Analysis::empty(){
    return samples.size() == 0;
}

float Analysis::frame_to_time(int frame_number){
    if (frame_number < 0 || frame_number > frame_count){
        return -1.0f;
    }

    float time_seconds = frame_number / 44100.0f;
    
    return static_cast<float>(round_to(time_seconds, 3));
}

float Analysis::ptp_amplitude(){
    if (empty()){
        return 0.0f;
    }
    float greatest = -1;
    float smallest = 1;


    for (std::size_t i = 0; i + 1 < samples.size(); i+=2){
        float l = samples[i];
        float r = samples[i+1];

        if (l > greatest){
            greatest = l;
        }

        if (r > greatest){
            greatest = r;
        }

        if (l < smallest){
            smallest = l;
        }

        if (r < smallest){
            smallest = r;
        }
    }

    return greatest - smallest;
}

float Analysis::peak_amplitude(){
    float peak = 0.0f;

    float absolute;
    for (float sample : samples){
        if ((absolute = std::abs(sample)) > peak){
            peak = absolute;
        }
    }

    return peak;
}

float Analysis::rms(){
    if (empty()){
        return 0.0f;
    }
    float square_sum = 0;
    for (std::size_t i = 0; i < samples.size(); i++){
        square_sum += samples[i] * samples[i];
    }

    return std::sqrt(square_sum / samples.size());
}

float Analysis::dc_offset(){
    if (empty()){
        return 0.0f;
    }

    float sum = 0;
    for (std::size_t i = 0; i < samples.size(); i++){
        sum += samples[i];
    }

    return sum / samples.size();
}

float Analysis::crest_factor(){
    if (empty()){
        return 0.0f;
    }

    float root_mean_square = rms();
    return root_mean_square != 0 ? peak_amplitude() / root_mean_square : 0.0f;
}

float Analysis::zero_crossing_rate(){
    if (empty()){
        return 0.0f;
    }

    int zeros = 0;

    for (int i = 0; i + 3 < samples.size(); i+=4){

        
        if ((samples[i] < 0 && samples[i+2] > 0) || (samples[i] > 0 && samples[i+2] < 0)){
            zeros++;
        }

        if ((samples[i+1] < 0 && samples[i+3] > 0) || (samples[i+1] > 0 && samples[i+3] < 0)){
            zeros++;
        }
    }


    return zeros / static_cast<float>(samples.size() - 2);
}

std::vector<Interval> Analysis::silence_detection(int window_size_ms, float threshold){
    if (window_size_ms <= 0 || empty()){
        return std::vector<Interval>();
    }

    const int window_size = round(44.1 * window_size_ms * 2);

    std::vector<Interval> silences;

    for (std::size_t i = 0; i + window_size - 1 < samples.size(); i += window_size){
        double square_sum = 0;

        for (std::size_t j = i; j < i + window_size; j++){
            square_sum += samples[j] * samples[j];
        }

        const float interval_rms = static_cast<float>(std::sqrt(square_sum / window_size));

        if (interval_rms <= threshold){
            const int start = i / 2; // é i/2 porque vamos considerar o frame, que tem duas samples
            const int end = (i + window_size) / 2;

            if (!silences.empty() && silences.back().second == start){
                silences.back().second = end;
            }
            else {
                silences.emplace_back(start, end);
            }
        }
    }

    for (Interval& inter : silences){
        inter.first = frame_to_time(inter.first);
        inter.second = frame_to_time(inter.second);
    }

    return silences;
}



#include <bunseki/cbunseki.hpp>

std::vector<float> samples_to_mono(std::vector<float> samples){
    std::vector<float> result;
    result.reserve(samples.size() / 2);

    for (std::size_t i = 0; i + 1 < samples.size(); i+=2){
        result.push_back((samples[i] + samples[i+1]) / 2.0f);
    }

    return result;
}

CBunseki::CBunseki(std::vector<float> samples) :
    samples(samples_to_mono(std::move(samples))),
    frame_count(this->samples.size()),
    analysis(this->samples),
    transform(this->samples)
{}
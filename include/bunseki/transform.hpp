#pragma once

#include <vector>
#include <complex>
#include <numbers>
#include <cstdint>
#include <iostream>
#include <cstddef>

#include <bunseki/buntools.hpp>

using Complex = std::complex<float>;
using ComplexVec = std::vector<Complex>;


struct FrequencyBin {
    float frequency;
    float magnitude;
};

class Transform {
public:
    Transform(const std::vector<float>& samples);

    const std::vector<float>& samples; // já recebe convertida pra mono

    ComplexVec dft() const;

    ComplexVec fft() const;
    std::vector<FrequencyBin> fft_frequencies() const;


    std::vector<ComplexVec> stft(int window_size, int hop_size) const;
    std::vector<std::vector<FrequencyBin>> stft_frequencies(int window_size, int hop_size) const;

    float spectral_centroid() const;

private:
    std::size_t frame_count() const {
        return this->samples.size();
    }

    ComplexVec fft_auxiliar(const std::vector<float>& vec) const;

    std::vector<FrequencyBin> fft_frequencies(const ComplexVec& vec) const;
    float spectral_centroid(const ComplexVec& fft_result) const;
};

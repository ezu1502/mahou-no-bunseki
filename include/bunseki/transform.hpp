#pragma once

#include <vector>
#include <complex>
#include <numbers>
#include <cstdint>
#include <iostream>

#include <bunseki/constants.hpp>

using Complex = std::complex<float>;
using ComplexVec = std::vector<Complex>;


bool is_power_of_two(std::size_t number);

void apply_hann_window(std::vector<float>& vec);

struct FrequencyBin {
    float frequency;
    float magnitude;
};

class Transform {
public:
    Transform(const std::vector<float>& samples);

    const std::vector<float>& samples; // já recebe convertida pra mono

    ComplexVec dft();
    ComplexVec fft();
    std::vector<ComplexVec> stft(int window_size, int hop_size);

    std::vector<FrequencyBin> fft_frequencies();
    std::vector<FrequencyBin> fft_frequencies(const ComplexVec& vec);

    std::vector<std::vector<FrequencyBin>> stft_frequencies(int window_size, int hop_size);

    float spectral_centroid();
    float spectral_centroid(const ComplexVec& fft_result);

private:
    ComplexVec fft_auxiliar(const std::vector<float>& vec);
    std::size_t frame_count;

};

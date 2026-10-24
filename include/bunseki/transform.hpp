#pragma once

#include <vector>
#include <complex>
#include <numbers>
#include <cstdint>
#include <iostream>

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

    ComplexVec dft();

    ComplexVec fft();
    std::vector<FrequencyBin> fft_frequencies();


    std::vector<ComplexVec> stft(int window_size, int hop_size);
    std::vector<std::vector<FrequencyBin>> stft_frequencies(int window_size, int hop_size);

    float spectral_centroid();

private:
    std::size_t frame_count;

    ComplexVec fft_auxiliar(const std::vector<float>& vec);

    std::vector<FrequencyBin> fft_frequencies(const ComplexVec& vec);
    float spectral_centroid(const ComplexVec& fft_result);
};

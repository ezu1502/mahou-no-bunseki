#pragma once

#include <vector>
#include <complex>
#include <numbers>
#include <cstdint>
#include <iostream>
#include <cstddef>
#include <array>
#include <utility>

#include <bunseki/buntools.hpp>

using Complex = std::complex<float>;
using ComplexVec = std::vector<Complex>;



struct FrequencyBin {
    float frequency;
    float magnitude;
};

struct ChromaFeatures {
    ChromaFeatures(
        std::vector<std::array<float, 12>> chroma_windows,
        std::array<float, 12> global_chroma,
        std::array<float, 12> mean_chroma,
        std::array<float, 12> chroma_variance
    );
    std::vector<std::array<float, 12>> chroma_windows;

    std::array<float, 12> global_chroma;
    std::array<float, 12> mean_chroma;
    std::array<float, 12> chroma_variance;
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

    
    std::vector<std::array<float, 12>> chroma_windows() const;
    std::vector<std::array<float, 12>> chroma_windows(const std::vector<std::vector<FrequencyBin>>& stft_freqs) const;

    std::array<float, 12> global_chroma() const;

    std::array<float, 12> mean_chroma() const;
    std::array<float, 12> mean_chroma(const std::vector<std::array<float, 12>>& chroma_vector) const;

    std::array<float, 12> chroma_variance(const std::vector<std::array<float, 12>>& chroma_vector) const;
    std::array<float, 12> chroma_variance() const;

    ChromaFeatures chroma_features() const;

private:
    std::size_t frame_count() const {
        return this->samples.size();
    }

    ComplexVec fft_auxiliar(const std::vector<float>& vec) const;

    std::vector<FrequencyBin> fft_frequencies(const ComplexVec& vec) const;
    float spectral_centroid(const ComplexVec& fft_result) const;

    std::array<float, 12> chroma(const std::vector<FrequencyBin>& fft_freqs) const;
};  

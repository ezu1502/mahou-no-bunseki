#pragma once

#include <vector>
#include <cstdint>
#include <complex>
#include <numbers>

using Complex = std::complex<double>;

const double PI = std::numbers::pi;

class Transform {
public:
    Transform(std::vector<float> samples, std::uint64_t frame_count);

    std::vector<float> samples;


    std::uint64_t frame_count;

    std::vector<Complex> dft();

    std::vector<Complex> fft();


    const std::vector<float>& mono_samples();


private:
    std::vector<float> _mono_samples;
    std::vector<Complex> fft(const std::vector<Complex>& samps);
};



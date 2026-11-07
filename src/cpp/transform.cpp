#include <vector>
#include <complex>
#include <stdexcept>
#include <numbers>
#include <cmath>
#include <cstdint>
#include <array>

#include <iostream>

#include <bunseki/transform.hpp>
#include <bunseki/buntools.hpp>

ChromaFeatures::ChromaFeatures(
    std::vector<std::array<float, 12>> chroma_windows,
    std::array<float, 12> global_chroma,
    std::array<float, 12> mean_chroma,
    std::array<float, 12> chroma_variance
) :
    chroma_windows(chroma_windows),
    global_chroma(global_chroma),
    mean_chroma (mean_chroma),
    chroma_variance(chroma_variance)
{}

Transform::Transform(const std::vector<float>& samples) : samples(samples){}

ComplexVec Transform::dft() const{
    if (samples.empty()){
        throw std::runtime_error("Samples is empty!");
    }

    ComplexVec result;
    result.reserve(samples.size());

    for (std::size_t k = 0; k < samples.size(); k++){
        Complex k_result(0.0f, 0.0f);
        
        for (std::size_t n = 0; n < samples.size(); n++){
            double angle = 2 * PI * k * n / samples.size();

            k_result.real(k_result.real() + (samples[n] * std::cos(angle)));
            k_result.imag(k_result.imag() + (samples[n] * -std::sin(angle)));
        }

        result.push_back(k_result);
    }

    return result;
}

ComplexVec Transform::fft() const{
    // PADDING 

    if (samples.empty()){
        throw std::runtime_error("Samples is empty!");
    }
    
    std::size_t size = samples.size();

    std::vector<float> padded = samples;

    std::size_t two_power = 1;

    while (size > two_power){
        two_power <<= 1;
    }

    std::vector<float> padding(two_power - size);
    padded.insert(padded.end(), padding.begin(), padding.end());

    size = padded.size();

    // FFT em si

    return fft_auxiliar(padded);
}

ComplexVec Transform::fft_auxiliar(const std::vector<float>& vec) const{
    if (vec.size() == 1){
        return {Complex(vec[0], 0.0f)};
    }

    std::vector<float> odd;
    std::vector<float> even;

    odd.reserve(vec.size() / 2);
    even.reserve(vec.size() / 2);

    for (std::size_t i = 0; i < vec.size(); i++){
        if (i % 2 != 0){
            odd.push_back(vec[i]);
        }
        else{
            even.push_back(vec[i]);
        }
    }

    ComplexVec odd_result = fft_auxiliar(odd);
    ComplexVec even_result = fft_auxiliar(even);
    
    ComplexVec result(vec.size());

    for (std::size_t k = 0, size = vec.size(); k < size / 2; k++){
        double angle = 2 * PI * k / size;
        
        result[k] = even_result[k] + Complex(
            std::cos(angle),
            -std::sin(angle)
        ) * odd_result[k];

        result[k + size/2] = even_result[k] - Complex(
            std::cos(angle),
            -std::sin(angle)
        ) * odd_result[k];
    }


    return result;
}

std::vector<ComplexVec> Transform::stft(int window_size, int hop_size) const{
    if (samples.empty()){
        throw std::runtime_error("Samples is empty!");
    }

    if (hop_size <= 0 || window_size < 2){
        throw std::invalid_argument("hop_size must be positive and window_size must be at least 2");
    }

    if (!buntools::is_power_of_two(window_size)){
        throw std::invalid_argument("window_size must be a power of 2");
    }
    
    std::vector<ComplexVec> result;

    for (std::size_t i = 0, len = samples.size(); i + window_size <= len; i += hop_size){
        std::vector<float> window(
            samples.begin() + i,
            samples.begin() + i + window_size
        );

        buntools::apply_hann_window(window);

        auto start = buntools::perf_counter();
        auto auxiliar = fft_auxiliar(window);

        std::cout << buntools::perf_counter() - start << "\n";

        result.push_back(auxiliar);
        
    }

    return result;
}

std::vector<FrequencyBin> Transform::fft_frequencies() const{
    ComplexVec fft_result = fft();

    std::vector<FrequencyBin> result;

    for (std::size_t k = 0, len = fft_result.size(); k <= len / 2; k++){
        float magnitude = std::abs(fft_result[k]) / len;

        if (k != 0 && k != len/2){
            magnitude *= 2.0f;
        }

        result.push_back(FrequencyBin{
            static_cast<float>(k) * SAMPLE_RATE / len,
            magnitude
        });
    }

    return result;
}

std::vector<FrequencyBin> Transform::fft_frequencies(const ComplexVec& vec) const{
    /*
        This one receives a valid FFT
    */

    if (vec.empty()){
        throw std::invalid_argument("Vec cannot be empty!");
    }
    std::vector<FrequencyBin> result;

    for (std::size_t k = 0, len = vec.size(); k <= len / 2; k++){
        float magnitude = std::abs(vec[k]) / len;

        if (k != 0 && !(len % 2 == 0 && k == len / 2)){
            magnitude *= 2.0f;
        }

        result.push_back(FrequencyBin{
            static_cast<float>(k) * SAMPLE_RATE / len,
            magnitude
        });
    }

    return result;
}


std::vector<std::vector<FrequencyBin>> Transform::stft_frequencies(int window_size, int hop_size) const{
    std::vector<ComplexVec> windows = stft(window_size, hop_size);

    std::cout << "stft in freqs finished";
    std::vector<std::vector<FrequencyBin>> result;
    result.reserve(windows.size());

    for (const auto& window : windows){
        result.push_back(fft_frequencies(window));
    }

    return result;
}

float Transform::spectral_centroid() const{
    return spectral_centroid(fft());
}

float Transform::spectral_centroid(const ComplexVec& vec) const{

    if (vec.empty()){
        throw std::invalid_argument("FFT result cannot be empty!");
    }
    std::size_t size = vec.size();

    auto frequency = [size](std::size_t k){
        return static_cast<float>(k) * SAMPLE_RATE / size;
    };

    float numerator = 0.0f;
    float denominator = 0.0f;

    for (std::size_t k = 0; k <= size / 2; k++){
        float xk = std::abs(vec[k]);


        numerator += frequency(k) * xk;
        denominator += xk;
    }

    if (denominator == 0.0f) {
        return 0.0f;
    }

    return numerator / denominator;
}


std::array<float, 12> Transform::chroma(const std::vector<FrequencyBin>& fft_freqs) const {
    if (fft_freqs.empty()){
        throw std::invalid_argument("fft_freqs is empty!");
    }

    std::array<float, 12> result{};

    float sum = 0.0f;

    for (const FrequencyBin& bin : fft_freqs){
        if (bin.frequency <= 0){
            continue;
        }

        float key_n = 69 + 12 * std::log2(bin.frequency / 440);

        int key_midi = static_cast<int>(std::round(key_n));

        int pitch_class = ((key_midi % 12) + 12) % 12;

        float squared = bin.magnitude * bin.magnitude;

        result[pitch_class] += squared;
        sum += squared;
    }

    if (sum > 0){
        for (float& f : result){
            f /= sum;
        }
    }

    return result;
}


std::vector<std::array<float, 12>> Transform::chroma_windows(const std::vector<std::vector<FrequencyBin>>& stft_freqs) const {
    std::vector<std::array<float, 12>> result;
    result.reserve(stft_freqs.size());

    for (const auto& window_fft : stft_freqs){
        std::array<float, 12> chroma_window = chroma(window_fft);

        result.push_back(chroma_window);
    }

    return result;
}

std::vector<std::array<float, 12>> Transform::chroma_windows() const {
    auto stft = stft_frequencies(HIGH_RES_WINDOW_SIZE, HIGH_RES_HOP_SIZE);
    std::cout << "Stft finished!";
    return chroma_windows(stft);
}

std::array<float, 12> Transform::global_chroma() const{
    if (samples.empty()){
        throw std::runtime_error("Samples is empty!");
    }

    return chroma(fft_frequencies());
}

std::array<float, 12> Transform::mean_chroma() const {
    return mean_chroma(chroma_windows());
}

std::array<float, 12> Transform::mean_chroma(const std::vector<std::array<float, 12>>& chroma_vector) const {
    if (chroma_vector.empty()){
        throw std::invalid_argument("Chroma vector is empty!");
    }
    std::size_t len = chroma_vector.size();
    std::array<float, 12> result{};

    for (const auto& c_window : chroma_vector){
        for (std::size_t i = 0; i < 12; i++){
            result[i] += c_window[i]; 
        }
    }

    for (float& key : result){
        key /= len;
    }

    return result;
}

std::array<float, 12> Transform::chroma_variance(const std::vector<std::array<float, 12>>& chroma_vector) const {
    if (chroma_vector.empty()){
        throw std::invalid_argument("Chroma vector is empty!");
    }

    std::size_t len = chroma_vector.size();

    auto mean = mean_chroma(chroma_vector);

    std::array<float, 12> result{};

    for (const auto& c_window : chroma_vector){
        for (std::size_t i = 0; i < 12; i++){
            float deviation = (c_window[i] - mean[i]);
            result[i] += deviation * deviation;
        }
    }

    for (float& key : result){
        key /= len;
    }

    return result;
}

std::array<float, 12> Transform::chroma_variance() const {
    return chroma_variance(chroma_windows());
}




ChromaFeatures Transform::chroma_features() const {
    std::cout << "Calculating chroma_windows...";
    std::vector<std::array<float, 12>> c_windows = chroma_windows();

    // std::array<float, 12> c_global = chroma(fft_frequencies());
    std::array<float, 12> c_global{};
    
    std::cout << "Calculating mean_chroma...";

    std::array<float, 12> c_mean = mean_chroma(c_windows);

    std::cout << "Calculating chroma_variance...";

    std::array<float, 12> c_variance = chroma_variance(c_windows);

    return ChromaFeatures(
        std::move(c_windows),
        c_global,
        c_mean,
        c_variance
    );
}

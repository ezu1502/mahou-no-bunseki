#include <vector>
#include <complex>
#include <stdexcept>
#include <numbers>
#include <cmath>
#include <cstdint>
#include <iostream>

constexpr std::int64_t SAMPLE_RATE = 44100;
using Complex = std::complex<float>;
using ComplexVec = std::vector<Complex>;

constexpr double PI = std::numbers::pi;

bool is_power_of_two(std::size_t number){
    return number > 0 && (number & (number - 1)) == 0;
}

void apply_hann_window(std::vector<float>& vec){
    std::size_t len = vec.size();

    if (len <= 1){
        throw std::invalid_argument("Hann window cannot be applied in a vector smaller than 2");
    }

    for (std::size_t n = 0; n < len; n++){
        float wn = 0.5f * (1.0f - std::cos((2.0f * PI * n) / (len - 1)));
        vec[n] *= wn;
    }
}   

struct FrequencyBin {
    float frequency;
    float magnitude;
};


class Transform {
public:
    Transform(const std::vector<float>& samples);

    const std::vector<float>& samples; // já recebe convertida pra mono
    std::size_t frame_count;

    ComplexVec dft();
    ComplexVec fft();
    std::vector<ComplexVec> stft(int window_size, int hop_size);

    std::vector<FrequencyBin> fft_frequencies();
    std::vector<FrequencyBin> fft_frequencies(const ComplexVec& vec);

    std::vector<std::vector<FrequencyBin>> stft_frequencies(int window_size, int hop_size);

    float central_spectroid();
    float central_spectroid(const ComplexVec& fft_result);

private:
    ComplexVec fft_auxiliar(const std::vector<float>& vec);
};

Transform::Transform(const std::vector<float>& samples) : samples(samples), frame_count(this->samples.size()){}

ComplexVec Transform::dft(){
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

ComplexVec Transform::fft(){
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

ComplexVec Transform::fft_auxiliar(const std::vector<float>& vec){
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

std::vector<ComplexVec> Transform::stft(int window_size, int hop_size){
    if (samples.empty()){
        throw std::runtime_error("Samples is empty!");
    }

    if (hop_size <= 0 || window_size < 2){
        throw std::invalid_argument("hop_size must be positive and window_size must be at least 2");
    }

    if (!is_power_of_two(window_size)){
        throw std::invalid_argument("window_size must be a power of 2");
    }
    
    std::vector<ComplexVec> result;

    for (std::size_t i = 0, len = samples.size(); i + window_size <= len; i += hop_size){
        std::vector<float> window(
            samples.begin() + i,
            samples.begin() + i + window_size
        );

        apply_hann_window(window);

        result.push_back(fft_auxiliar(window));
    }

    return result;
}


std::vector<FrequencyBin> Transform::fft_frequencies(){
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

std::vector<FrequencyBin> Transform::fft_frequencies(const ComplexVec& vec){
    /*
        This one receives a valid FFT
    */

    if (vec.empty()){
        throw std::invalid_argument("Vec cannot be empty!");
    }
    std::vector<FrequencyBin> result;

    for (std::size_t k = 0, len = vec.size(); k <= len / 2; k++){
        float magnitude = std::abs(vec[k]) / len;

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


std::vector<std::vector<FrequencyBin>> Transform::stft_frequencies(int window_size, int hop_size){
    std::vector<ComplexVec> windows = stft(window_size, hop_size);

    std::vector<std::vector<FrequencyBin>> result;
    result.reserve(windows.size());

    for (const auto& window : windows){
        result.push_back(fft_frequencies(window));
    }

    return result;
}

float Transform::central_spectroid(){
    return central_spectroid(fft());
}

float Transform::central_spectroid(const ComplexVec& vec){

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

int main(){
    std::vector<float> samples = {
        0.0000f,  0.7071f,  1.0000f,  0.7071f,
        0.0000f, -0.7071f, -1.0000f, -0.7071f,
        0.0000f,  0.7071f,  1.0000f,  0.7071f,
        0.0000f, -0.7071f, -1.0000f, -0.7071f,
        0.0000f,  0.7071f,  1.0000f,  0.7071f,
        0.0000f, -0.7071f, -1.0000f, -0.7071f,
        0.0000f,  0.7071f,  1.0000f,  0.7071f,
        0.0000f, -0.7071f, -1.0000f, -0.7071f
    };

    Transform t(samples);

    auto tr = t.fft_frequencies();

    for (auto bin : tr){
        std::cout << "Frequency: " << bin.frequency
          << ", Magnitude: " << bin.magnitude << '\n';
    }
    
}
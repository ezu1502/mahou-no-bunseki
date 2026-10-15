#include <vector>
#include <cstdint>
#include <utility>
#include <complex>
#include <numbers>
#include <cmath>
#include <cstddef>
#include <iostream>

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
    bool empty();

    std::vector<float> _mono_samples;
        std::vector<Complex> fft(const std::vector<Complex>& samps);
};

const std::vector<float>& Transform::mono_samples(){
    if (samples.empty()){
        return samples;
    }

    if (_mono_samples.empty()){
        std::vector<float> mono;

        for (std::size_t i = 0; i + 1 < samples.size(); i+=2){
            mono.push_back((samples[i] + samples[i + 1]) / 2);
        }

        _mono_samples = std::move(mono);
    }   
    return _mono_samples;
}

Transform::Transform(std::vector<float> samples, std::uint64_t frame_count) :
    samples(std::move(samples)), frame_count(frame_count){}

std::vector<Complex> Transform::dft(){
    auto mono = mono_samples(); 
    std::size_t frames = mono.size();

    std::vector<Complex> result;

    for (std::size_t k = 0; k < frames; k++){
        double real = 0;
        double imaginary = 0;

        for (std::size_t n = 0; n < frames; n++){
            real += mono[n] * (std::cos((2 * PI * k * n) / frames));
            imaginary -= mono[n] * (std::sin((2 * PI * k * n) / frames));
        }

        result.push_back(Complex(real, imaginary));
    }

    return result;
}


std::vector<Complex> Transform::fft(){ // info PÚBLICA
    using ComplexVec = std::vector<Complex>;

    auto mono = mono_samples(); 
    std::size_t frames = mono.size();
    if (frames == 0){
        return ComplexVec();
    }

    if (frames == 1){
        ComplexVec base_case;
        base_case.emplace_back(mono[0], 0);
        return base_case;
    }

    std::size_t two_power = 1;
    while (two_power < frames){
        two_power <<= 1;
    }  

    std::vector<float> padding(two_power - frames);
    mono.insert(mono.end(), padding.begin(), padding.end());

    ComplexVec mono_complex;

    for (float float_sample : mono){
        mono_complex.emplace_back(float_sample, 0);
    }
    
    return fft(mono_complex);
}

std::vector<Complex> Transform::fft(const std::vector<Complex>& samps){ // info * PRIVADA
    using ComplexVec = std::vector<Complex>;

    auto mono = samps; 
    std::size_t frames = mono.size();
    

    if (frames == 2){
        ComplexVec result;

        result.emplace_back(samps[0] + samps[1]);
        result.emplace_back(samps[0] - samps[1]);

        return result;
    }

    ComplexVec odd;
    ComplexVec even;

    for (std::size_t i = 0; i < frames; i++){
        if(i % 2 != 0){
            odd.push_back(mono[i]);
        }
        else {
            even.push_back(mono[i]);
        }
    }

    auto odd_fft = fft(odd);
    auto even_fft = fft(even);

    ComplexVec result(frames);

    for (std::size_t k = 0; k < odd_fft.size(); k++){
        auto odd = odd_fft[k];
        auto even = even_fft[k];

        Complex almost_t(std::cos((2 * PI * k) / frames), -std::sin((2 * PI * k) / frames));

        Complex t = odd * almost_t;

        result[k] = even + t;
        result[k + frames / 2] = even - t;

    }
    // fator wkn é cos(2*pi*k / N) - i * sin(2*pi*k / N)

    return result;

}



int main(){
    std::vector<float> s = {1, 1, 2, 2, 3, 3, 4, 4};
    std::uint64_t f = 2;

    Transform t(s, f);

    auto vec = t.fft();

    auto cev = t.dft();



    for (const auto& value : vec){
        std::cout << value << "\n";
    }

    std::cout << "\n";

    for (const auto& value : cev){
        std::cout << value << "\n";
    }
}
#include <vector>
#include <stdexcept>
#include <cmath>
#include <algorithm>
#include <bunseki/buntools.hpp>

#include <fstream>
#include <sstream>
#include <unordered_map>

namespace buntools {
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

    std::vector<float> samples_to_mono(std::vector<float> samples){
        std::vector<float> result;
        result.reserve(samples.size() / 2);

        for (std::size_t i = 0; i + 1 < samples.size(); i+=2){
            result.push_back((samples[i] + samples[i+1]) / 2.0f);
        }

        return result;
    }

    std::vector<float> generate_sine_wave(float frequency, float time, int sample_rate){
        std::vector<float> result(time * sample_rate);

        for (std::size_t i = 0, len = result.size(); i < len; i++){
            result[i] = std::sin(2 * PI * frequency * (static_cast<double>(i) / sample_rate));
        }

        return result;
    }

    template <typename value_type, typename lambda>
    value_type max(const std::vector<value_type>& values, lambda key){
        if (values.empty()){
            throw std::invalid_argument("max() cannot be used in empty vectors");
        }

        auto iterator = std::max_element(values.begin(), values.end(), key);

        return *iterator;
    }

    template <typename value_type>
    value_type max(const std::vector<value_type>& values){
        if (values.empty()){
            throw std::invalid_argument("max() cannot be used in empty vectors");
        }

        auto iterator = std::max_element(values.begin(), values.end());

        return *iterator;
    }


    std::unordered_map<std::string, int> read_dataset(const std::string& csv_path){
        std::ifstream file(csv_path);

        if (!file){
            throw std::runtime_error("Couldn't read dataset");
        }
        std::string line;

        std::getline(file, line); // isso é pra pular o cabeçalho

        std::unordered_map<std::string, int> result;

        while (std::getline(file, line)){
            std::stringstream ss(line);

            std::string song_name;
            int real_bpm{};

            std::getline(ss, song_name, ',');

            const bool parsed = static_cast<bool>(ss >> real_bpm); // aqui vai ler e converter pra int
            
            if (!parsed){ 
                continue;
            } 

            result[song_name] = real_bpm;
        }
        return result;
    }
    
}

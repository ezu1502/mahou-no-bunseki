#define MINIAUDIO_IMPLEMENTATION
#include "../external/miniaudio.h"

#include <iostream>
#include <vector>
#include <tuple>
#include <string>
#include <expected>
#include <cstdint>
#include <utility>

using SampleResults = std::tuple<std::vector<float>, ma_uint64>;

std::expected<SampleResults, std::string> read_samples(const std::string& song_path){
    /*
        Receives a song path, reads it and puts samples into a std::vector.
        Returns the vector and the number of frames miniaudio could read, or an error message.
    */
   
    if (song_path.empty()){
        return std::unexpected("Empty song_path!");
    }   


    ma_decoder decoder;

    ma_decoder_config config = ma_decoder_config_init(ma_format_f32, 2, 44100); // Configura o decoder pra float 32bits, 2 canais (stereo) e 44100Hz de sample rate 
    ma_result result = ma_decoder_init_file(song_path.c_str(), &config, &decoder); // Inicializa

    if (result != MA_SUCCESS){
        return std::unexpected("Couldn't decode audio");
    }

    std::vector<float> samples;


    std::vector<float> buffer(88200); // 44100 frames, 2 canais

    std::uint64_t total_frames_read = 0;

    while (true){
        ma_uint64 frames_read = 0; // Onde o miniaudio joga a qtde. de frames lidos

        result = ma_decoder_read_pcm_frames(
            &decoder, // Esse é o decoder que criamos, ele vai ler e decodificar o arquivo de áudio
            buffer.data(), // vector.data() retorna um ponteiro pro começo do vetor
            44100, // Máximo de frames lidos nessa chamada
            &frames_read // o miniaudio vai jogar o número de frames lidos aqui
        );

        if (result != MA_SUCCESS){
            // Se der merda, desinicializamos o decoder
            ma_decoder_uninit(&decoder);
            return std::unexpected("Couldn't read audio samples");
        }

        total_frames_read += frames_read;
        samples.insert(samples.end(), buffer.begin(), buffer.begin() + (frames_read * 2));

        if (frames_read < 44100){
            break;
        }
    }
    

    ma_decoder_uninit(&decoder);

    return std::make_tuple(std::move(samples), total_frames_read);
} 

int main(int argc, char *argv[]){
    if (argc != 2){
        std::cerr << "Invalid argument number";
        return 1;
    }

    auto result = read_samples(argv[1]);

    if (result){
        auto [samples, frames_read] = *result;
        std::cout << frames_read;
        
        return 0;
    }
    else {
        std::cerr << result.error();
        return 1;
    }
}
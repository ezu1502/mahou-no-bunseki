#include <bunseki/sample_reader.hpp>

std::expected<std::vector<float>, std::string> read_samples(const std::string& song_path){
    if (song_path.empty()){
        return std::unexpected("Song path is empty!");
    }

    ma_decoder decoder;

    ma_decoder_config config = ma_decoder_config_init(ma_format_f32, CHANNELS, SAMPLE_RATE);
    ma_result result = ma_decoder_init_file(song_path.c_str(), &config, &decoder);

    if (result != MA_SUCCESS){
        return std::unexpected("Couldn't decode audio\nError Code: " + std::to_string(result));
    }

    std::vector<float> samples;

    std::vector<float> buffer(FRAMES_PER_READ * CHANNELS);

    while (true){
        ma_uint64 frames_read = 0;

        result = ma_decoder_read_pcm_frames(
            &decoder,
            buffer.data(),
            FRAMES_PER_READ,
            &frames_read
        );

        if (result != MA_SUCCESS){
            ma_decoder_uninit(&decoder);
            return std::unexpected("Couldn't read samples\nError Code: " + std::to_string(result));
        }

        samples.insert(samples.end(), buffer.begin(), buffer.begin() + (frames_read * CHANNELS));

        if (frames_read < FRAMES_PER_READ){
            break;
        }
    }

    ma_decoder_uninit(&decoder);

    return samples;
}
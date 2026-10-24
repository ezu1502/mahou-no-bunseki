#pragma once

#include <cstdint>
#include <numbers>
#include <vector>
#include <cstddef>
#include <unordered_map>
#include <string>

inline constexpr std::uint64_t SAMPLE_RATE = 44100;
inline constexpr std::uint32_t CHANNELS = 2;
inline constexpr std::uint64_t FRAMES_PER_READ = SAMPLE_RATE;

inline constexpr double PI = std::numbers::pi;

inline constexpr std::size_t STANDARD_WINDOW_SIZE = 1024;
inline constexpr std::size_t STANDARD_HOP_SIZE = 441;

namespace buntools {
    bool is_power_of_two(std::size_t number);

    void apply_hann_window(std::vector<float>& vec);

    std::vector<float> samples_to_mono(std::vector<float> samples);

    std::vector<float> generate_sine_wave(float frequency, float time, int sample_rate = SAMPLE_RATE);

    std::unordered_map<std::string, int> read_dataset(const std::string& csv_path);

}






#pragma once

#include <cstdint>
#include <numbers>

constexpr std::uint64_t SAMPLE_RATE = 44100;
constexpr std::uint32_t CHANNELS = 2;
constexpr std::uint64_t FRAMES_PER_READ = SAMPLE_RATE;

inline constexpr double PI = std::numbers::pi;



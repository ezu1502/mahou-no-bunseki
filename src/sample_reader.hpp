#include "../external/miniaudio.h"

#include <vector>
#include <string>
#include <expected>

constexpr ma_uint64 SAMPLE_RATE = 44100;
constexpr ma_uint32 CHANNELS = 2;
constexpr ma_uint64 FRAMES_PER_READ = SAMPLE_RATE;

std::expected<std::vector<float>, std::string> read_samples(const std::string& song_path);
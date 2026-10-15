#pragma once

#include "../../external/miniaudio.h"

#include <tuple>
#include <expected>
#include <vector>
#include <string>

using SampleResults = std::tuple<std::vector<float>, ma_uint64>;
std::expected<SampleResults, std::string> read_samples(const std::string& song_path);

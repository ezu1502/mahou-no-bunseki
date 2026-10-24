#pragma once

#include <miniaudio.h>

#include <vector>
#include <string>
#include <expected>
#include <bunseki/buntools.hpp>

std::expected<std::vector<float>, std::string> read_samples(const std::string& song_path, bool to_mono = true);
#include <bunseki/sample_reader.hpp>
#include <bunseki/cbunseki.hpp>
#include <vector>
#include <iostream>
#include <utility>
#include <bunseki/buntools.hpp>
#include <algorithm>
#include <unordered_map>
#include <print>
#include <cmath>

void test_one(const std::string& song_name, const std::unordered_map<std::string, int>& dataset){
    auto it = dataset.find(song_name);
    if (it == dataset.end()){
        return;
    }

    int real_bpm = it->second;
    
    if (real_bpm == 0){
        std::println("{} has a BPM of zero?", song_name);
        return;
    }

    auto result = read_samples("test_tracks/" + song_name + ".mp3", true);
        
    if (!result){
        std::cerr << result.error() << song_name << "\n";
        return;
    }

    auto samples = result.value();

    CBunseki bunseki(std::move(samples));

    float estimated_bpm = bunseki.analysis.lag_bpm();
    float error = std::abs(estimated_bpm - real_bpm);

    std::println(
        "{} - Estimated: {}BPM, Real: {}BPM,  Error: {:.0f}BPM | {:.2f}%",
        song_name, estimated_bpm, real_bpm, error, (error / real_bpm) * 100
    );
}

void test_all(const std::unordered_map<std::string, int>& dataset){
    float error_bpm = 0.0f;
    float error_percent = 0.0f;

    int evaluated = 0;

    for (const auto& [song_name, real_bpm] : dataset){
        if (real_bpm == 0){
            std::println("{} has a BPM of zero?", song_name);
            continue;
        }
        
        auto result = read_samples("test_tracks/" + song_name + ".mp3", true);
        
        if (!result){
            std::cerr << result.error() << song_name << "\n";
            continue;
        }

        auto samples = result.value();

        CBunseki bunseki(std::move(samples));

        float estimated_bpm = bunseki.analysis.lag_bpm();
        float error = std::abs(estimated_bpm - real_bpm);

        std::println(
            "{} - Estimated: {}BPM, Real: {}BPM,  Error: {:.0f}BPM | {:.2f}%",
            song_name, estimated_bpm, real_bpm, error, (error / real_bpm) * 100
        );

        error_bpm += error;
        error_percent += (error / real_bpm) * 100;
        ++evaluated;

    }

    if (evaluated == 0){
        return;
    }

    std::cout << "\n";
    std::println("Mean Absolute Error: {}BPM | {:.3f}%", error_bpm / evaluated, error_percent / evaluated);
}


int main(){
    std::cout << "Loading database...\n";

    std::unordered_map<std::string, int> dataset = buntools::read_dataset("datasets/bpm.csv");

    std::cout << "Database Loaded!\n";

    test_one("elf", dataset);
}

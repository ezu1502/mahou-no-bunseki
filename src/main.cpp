#include <bunseki/sample_reader.hpp>
#include <bunseki/cbunseki.hpp>
#include <vector>
#include <iostream>
#include <utility>

int main(){
    auto result = read_samples("aitakute.mp3");

    if (!result){
        std::cerr << result.error();
        return 1;
    }

    std::vector<float> samples = result.value();

    CBunseki bunseki(std::move(samples));

    std::cout << bunseki.analysis.rms();
}
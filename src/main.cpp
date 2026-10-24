#include <bunseki/sample_reader.hpp>
#include <bunseki/cbunseki.hpp>
#include <vector>
#include <iostream>
#include <utility>
#include <bunseki/buntools.hpp>
#include <algorithm>

int main(){
    std::cout << "Lendo samples...\n";
    auto result = read_samples("0.mp3", true);
    
    if (!result){
        std::cerr << result.error() << "\n";
        return 1;
    }

    auto samples = result.value();

    CBunseki bunseki(std::move(samples));

    std::cout << "Calculando BPM...\n";

    std::cout << bunseki.analysis.bpm();
}
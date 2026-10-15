#include <iostream>
#include "sample_reader.hpp"

int main(int argc, char *argv[]){
    if (argc != 2){
        std::cerr << "Invalid argument number";
        return 1;
    }

    auto result = read_samples(argv[1]);

    if (!result){
        std::cerr << result.error() << "\n";
        return 1;
    }

    auto& [samples, frames_read] = *result;
    
    std::cout << frames_read << "\n";
    return 0;
}
#include "sample_reader.hpp"
#include <iostream>
#include <utility>
    
int main(){
    std::string path = "aitakute.mp3";
    auto result = read_samples(path);

    if (!result){
        std::cerr << result.error();
        return 1;
    }

    std::vector<float> samples = std::move(*result);
    
    std::cout << result->size();
}
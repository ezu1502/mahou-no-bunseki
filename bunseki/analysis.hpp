#include <vector>
#include <cstdint>

using Interval = std::pair<float, float>;

double round_to(double number, int decimal_places);

class Analysis {
public:
    std::vector<float> samples;
    std::uint64_t frame_count;
    
    Analysis(std::vector<float> samples, std::uint64_t frame_count);

    float ptp_amplitude();
    float peak_amplitude();
    float rms();

    float dc_offset();
    float crest_factor();
    float zero_crossing_rate();

    std::vector<Interval> silence_detection(int window_size_ms = 20, float threshold = 0.01f);

private:
    bool empty();
    float frame_to_time(int frame_number);
};


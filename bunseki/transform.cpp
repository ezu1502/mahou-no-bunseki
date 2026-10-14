#include <vector>
#include <cstdint>
#include <utility>

class Transform {
public:
    Transform(std::vector<float> samples, std::uint64_t frame_count);

    std::vector<float> samples;
    std::uint64_t frame_count;

private:
    bool empty();
};

Transform::Transform(std::vector<float> samples, std::uint64_t frame_count) :
    samples(std::move(samples)), frame_count(frame_count){}



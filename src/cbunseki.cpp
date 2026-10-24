#include <bunseki/cbunseki.hpp>

#include <bunseki/buntools.hpp>

CBunseki::CBunseki(std::vector<float> samples) :
    samples((std::move(samples))),
    frame_count(this->samples.size()),
    analysis(this->samples),
    transform(this->samples)
{}
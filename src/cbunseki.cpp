#include <bunseki/cbunseki.hpp>

#include <bunseki/buntools.hpp>

CBunseki::CBunseki(std::vector<float> samples) :
    samples((std::move(samples))),
    analysis(this->samples),
    transform(this->samples)
{}
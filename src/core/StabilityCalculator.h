#pragma once
#include <cstddef>

namespace cockpit {

// Computes a stability index (0..1) from a circular history of thrust samples.
// Lower standard deviation -> higher stability (approaches 1.0).
// history points to an array of sampleCount elements.
double computeStabilityIndex(const double* history, std::size_t sampleCount);

}

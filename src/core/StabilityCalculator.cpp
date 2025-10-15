#include "core/StabilityCalculator.h"
#include <cmath>

namespace cockpit {

double computeStabilityIndex(const double* history, std::size_t sampleCount) {
  if (!history || sampleCount < 2) return 1.0; // trivially stable
  double sum = 0.0; for (std::size_t i=0;i<sampleCount;++i) sum += history[i];
  double mean = sum / static_cast<double>(sampleCount);
  double varSum = 0.0; for (std::size_t i=0;i<sampleCount;++i) { double d = history[i] - mean; varSum += d*d; }
  double variance = varSum / static_cast<double>(sampleCount-1);
  double stddev = std::sqrt(variance);
  double norm = stddev / 50.0; // heuristic normalization
  double stability = 1.0 - norm;
  if (stability < 0.0) stability = 0.0; else if (stability > 1.0) stability = 1.0;
  return stability;
}

}

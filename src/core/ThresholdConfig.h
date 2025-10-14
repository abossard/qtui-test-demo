#pragma once
#include <optional>
#include <QString>

// Threshold configuration stub (Task T017)
struct ThresholdConfig {
  double radiationSpikeThreshold{0.0};
  int radiationConsecSamples{0};
  double tempDelta{0.0};
  int tempWindowMs{0};
  double pressureVariance{0.0};
};

inline std::optional<QString> validate(const ThresholdConfig& cfg) {
  if (cfg.radiationSpikeThreshold < 0) return QString("radiationSpikeThreshold negative");
  if (cfg.radiationConsecSamples < 0) return QString("radiationConsecSamples negative");
  if (cfg.tempWindowMs < 0) return QString("tempWindowMs negative");
  if (cfg.pressureVariance < 0) return QString("pressureVariance negative");
  return std::nullopt;
}

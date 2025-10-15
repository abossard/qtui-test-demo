// SimulationCore physics implementation (initial green for velocity/altitude/fuel)
#include "core/SimulationCore.h"
#include "core/Constants.h"
#include "services/AlertBus.h"
#include <QtGlobal>
#include <QDateTime>

namespace { constexpr double kMinFuel = 0.0; constexpr double kMaxFuel = 100.0; }

void SimulationCore::setThrustPercent(double pct) {
  if (pct < 0.0) pct = 0.0; else if (pct > 100.0) pct = 100.0;
  thrustPercent_ = pct;
  // Record thrust in history for stability variance computation
  thrustHistory_[thrustHistoryHead_] = thrustPercent_;
  thrustHistoryHead_ = (thrustHistoryHead_ + 1) % kThrustHistoryCapacity;
  if (thrustHistorySize_ < kThrustHistoryCapacity) thrustHistorySize_++;
}

void SimulationCore::refuel(double amount) {
  if (amount <= 0.0) return;
  setFuel(fuelLevel_ + amount);
}

void SimulationCore::setFuel(double pct) {
  if (pct < 0.0) pct = 0.0; else if (pct > 100.0) pct = 100.0;
  bool wasBelow = (fuelLevel_ < cockpit::kLowFuelThresholdPercent);
  fuelLevel_ = pct;
  if (wasBelow && fuelLevel_ >= cockpit::kLowFuelThresholdPercent) {
    // Reset alert latch so a new crossing triggers later.
    lowFuelAlertActive_ = false;
  }
}

void SimulationCore::tick(double dtSeconds) {
  if (dtSeconds <= 0.0) {
    emit telemetryProduced(current());
    return;
  }

  // Simple vertical thrust -> velocity -> altitude integration (no drag/gravity yet)
  const double accel = (thrustPercent_ / 100.0) * cockpit::kMaxAcceleration; // m/s^2
  velocity_ += accel * dtSeconds;               // m/s
  altitude_ += velocity_ * dtSeconds;           // m

  // Fuel burn proportional to thrust and time
  const double burn = (thrustPercent_ / 100.0) * cockpit::kFuelBurnPerSecAtFull * dtSeconds;
  fuelLevel_ -= burn;
  if (fuelLevel_ < kMinFuel) fuelLevel_ = kMinFuel;
  if (fuelLevel_ > kMaxFuel) fuelLevel_ = kMaxFuel; // (should not exceed, but defensive)

  // Update telemetry snapshot (stabilityIndex placeholder = 0)
  currentState_.altitude = altitude_;
  currentState_.verticalVelocity = velocity_;
  currentState_.thrustPercent = thrustPercent_;
  currentState_.fuelRemaining = fuelLevel_;
  // Stability: compute variance of thrust history; lower variance -> higher stability
  if (thrustHistorySize_ > 1) {
    double sum = 0.0; for (int i=0;i<thrustHistorySize_;++i) sum += thrustHistory_[i];
    double mean = sum / thrustHistorySize_;
    double varSum = 0.0; for (int i=0;i<thrustHistorySize_;++i) { double d = thrustHistory_[i] - mean; varSum += d*d; }
    double variance = varSum / (thrustHistorySize_-1);
    // Normalize: assume max meaningful variance ~ (100^2)/4 for swings 0↔100; use sqrt for stddev
    double stddev = std::sqrt(variance);
    double norm = stddev / 50.0; // 0..~2
    double stability = 1.0 - norm;
    if (stability < 0.0) stability = 0.0; else if (stability > 1.0) stability = 1.0;
    stabilityIndex_ = stability;
  }
  currentState_.stabilityIndex = stabilityIndex_;
  currentState_.timestamp = QDateTime::currentMSecsSinceEpoch();

  // Low fuel alert crossing detection
  if (!lowFuelAlertActive_ && fuelLevel_ < cockpit::kLowFuelThresholdPercent && alertBus_) {
    Alert a; a.severity = Alert::WARNING; a.message = QString("Low fuel (%.1f%% remaining)").arg(fuelLevel_);
    alertBus_->raise(a);
    lowFuelAlertActive_ = true;
  } else if (lowFuelAlertActive_ && fuelLevel_ >= cockpit::kLowFuelThresholdPercent) {
    // Reset latch if refueled / threshold recovered (future refuel logic)
    lowFuelAlertActive_ = false;
  }

  emit telemetryProduced(current());
}

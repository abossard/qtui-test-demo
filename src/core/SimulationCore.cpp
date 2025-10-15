// SimulationCore physics implementation (initial green for velocity/altitude/fuel)
#include "core/SimulationCore.h"
#include "core/Constants.h"
#include "services/AlertBus.h"
#include <QtGlobal>
#include <QDateTime>
#include <QDebug>
#include "core/StabilityCalculator.h"

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
    stabilityIndex_ = cockpit::computeStabilityIndex(thrustHistory_, thrustHistorySize_);
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

  qInfo() << "[tick] thrust%=" << thrustPercent_ << " alt=" << altitude_ << " vel=" << velocity_ << " fuel%=" << fuelLevel_ << " stability=" << stabilityIndex_;
  emit telemetryProduced(current());
}

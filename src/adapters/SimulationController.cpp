#include "adapters/SimulationController.h"
#include "core/SimulationCore.h"

SimulationController::SimulationController(SimulationCore* core, QObject* parent)
  : QObject(parent), core_(core) {
  if (core_) {
    // Bridge core emission to adapter signal (core uses telemetryProduced)
    QObject::connect(core_, &SimulationCore::telemetryProduced, this, &SimulationController::telemetryTick);
  }
}

void SimulationController::setThrustPercent(double pct) {
  if (!core_) return;
  core_->setThrustPercent(pct);
}

TelemetryState SimulationController::current() const {
  if (!core_) return {};
  return core_->current();
}

void SimulationController::manualTick() {
  if (!core_) return;
  // Placeholder: simply re-emit current state (no physics yet)
  emit telemetryTick(core_->current());
}

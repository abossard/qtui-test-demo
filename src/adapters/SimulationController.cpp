#include "adapters/SimulationController.h"
#include "core/SimulationCore.h"
#include <QTimer>

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
  core_->tick(0.1); // 100ms tick for manual
  emit telemetryTick(core_->current());
}

void SimulationController::startTimer(double intervalSec) {
  if (timer_) return; // already running
  timer_ = new QTimer(this);
  connect(timer_, &QTimer::timeout, this, [this, intervalSec]() {
    if (core_) core_->tick(intervalSec);
    emit telemetryTick(core_->current());
  });
  timer_->start(static_cast<int>(intervalSec * 1000));
}

void SimulationController::stopTimer() {
  if (!timer_) return;
  timer_->stop();
  timer_->deleteLater();
  timer_ = nullptr;
}

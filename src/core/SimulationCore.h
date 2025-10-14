#pragma once
#include <QObject>
#include "core/types.h"

// Minimal SimulationCore interface (Task T007)
class SimulationCore : public QObject {
  Q_OBJECT
public:
  explicit SimulationCore(QObject* parent = nullptr) : QObject(parent) {}
  virtual ~SimulationCore() = default;

  // Set desired thrust percent (will be clamped 0..100 in implementation phase)
  virtual void setThrustPercent(double /*pct*/) {}

  // Return last known telemetry snapshot
  virtual TelemetryState current() const { return currentState_; }

signals:
  void telemetryProduced(const TelemetryState&);

protected:
  TelemetryState currentState_{}; // placeholder state storage
};

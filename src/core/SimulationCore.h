#pragma once
#include <QObject>
#include "core/types.h"
class AlertBus; // forward decl

// Minimal SimulationCore interface (Task T007)
class SimulationCore : public QObject {
  Q_OBJECT
public:
  explicit SimulationCore(QObject* parent = nullptr) : QObject(parent) {}
  virtual ~SimulationCore() = default;

  // Optional alert bus injection (for low fuel alert). Not owning.
  void setAlertBus(AlertBus* bus) { alertBus_ = bus; }

  // --- API (Early Red Phase Stubs) --------------------------------------------------
  // Future: clamp 0..100 and emit telemetry after each tick()
  virtual void setThrustPercent(double pct);
  virtual double thrustPercent() const { return thrustPercent_; }

  // Called by controller / simulation loop (dt in seconds)
  // Stub: does not yet update velocity/altitude/fuel -> ensures tests fail logically.
  virtual void tick(double dtSeconds);

  virtual double velocity() const { return velocity_; }
  virtual double altitude() const { return altitude_; }
  virtual double fuelLevel() const { return fuelLevel_; }
  virtual void refuel(double amount);
  virtual void setFuel(double pct);

  // Return last known telemetry snapshot (currently unused placeholder structure)
  virtual TelemetryState current() const { return currentState_; }

signals:
  void telemetryProduced(const TelemetryState&);

protected:
  TelemetryState currentState_{}; // placeholder state storage
  double thrustPercent_{0.0};
  double velocity_{0.0};
  double altitude_{0.0};
  double fuelLevel_{100.0};
  // Stability computation
  double stabilityIndex_{1.0};
  static constexpr int kThrustHistoryCapacity = 32;
  double thrustHistory_[kThrustHistoryCapacity]{}; // circular buffer
  int thrustHistorySize_{0};
  int thrustHistoryHead_{0};
  bool lowFuelAlertActive_{false};
  AlertBus* alertBus_{nullptr};
};

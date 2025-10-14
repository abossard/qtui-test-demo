#pragma once
#include <QObject>
#include "core/types.h"

class SimulationCore; // fwd

// Adapter responsible for exposing telemetryTick signal & delegating to SimulationCore (Task T008)
class SimulationController : public QObject {
  Q_OBJECT
public:
  explicit SimulationController(SimulationCore* core, QObject* parent = nullptr);

  void setThrustPercent(double pct); // forwards to core
  TelemetryState current() const;    // snapshot

signals:
  void telemetryTick(const TelemetryState&);

public slots:
  void manualTick(); // For early tests (no timer loop yet)

private:
  SimulationCore* core_{nullptr};
};

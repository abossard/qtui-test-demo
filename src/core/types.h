#pragma once
#include <QtGlobal>

struct TelemetryState {
  double altitude{0.0};
  double verticalVelocity{0.0};
  double thrustPercent{0.0}; // 0-100
  double fuelRemaining{100.0}; // 0-100
  double stabilityIndex{0.0}; // 0-1 derived
  qint64 timestamp{0};        // ms since epoch
};

struct PowerAllocation {
  int lifeSupport{25};
  int navigation{25};
  int weapons{25};
  int research{15};
  int reserve{10};
};

#pragma once
#include <QString>
#include <QMetaType>

// Logging categories enumeration (Task T010)
namespace LogCategory {
  enum Type {
    SIMULATION,
    POWER,
    SENSOR,
    ALERT,
    AUTOPILOT,
    REPLAY,
    THEME,
    UI
  };
}
Q_DECLARE_METATYPE(LogCategory::Type)

inline const char* toString(LogCategory::Type c) {
  switch (c) {
    case LogCategory::SIMULATION: return "SIMULATION";
    case LogCategory::POWER: return "POWER";
    case LogCategory::SENSOR: return "SENSOR";
    case LogCategory::ALERT: return "ALERT";
    case LogCategory::AUTOPILOT: return "AUTOPILOT";
    case LogCategory::REPLAY: return "REPLAY";
    case LogCategory::THEME: return "THEME";
    case LogCategory::UI: return "UI";
  }
  return "UNKNOWN";
}

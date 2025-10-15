// Central physics & threshold constants
#pragma once
namespace cockpit {
constexpr double kMaxAcceleration = 8.0;            // m/s^2 at 100% thrust
constexpr double kFuelBurnPerSecAtFull = 0.5;       // fuel units/sec at 100% thrust
constexpr double kLowFuelThresholdPercent = 15.0;   // alert threshold
}
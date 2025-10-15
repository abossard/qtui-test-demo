/**
 * Feature: Flight Console basic telemetry propagation
 *
 *  Narrative:
 *    As a pilot user viewing the Flight Console
 *    I want applied thrust to propagate to velocity then altitude
 *    So that I see immediate and believable flight feedback.
 *
 *  Rules (initial slice – expanded later):
 *    R1 Thrust percent (0..100) drives positive velocity (>0) within one tick when >0.
 *    R2 Altitude increases monotonically (Δalt > 0) after velocity becomes positive.
 *    R3 Fuel level decreases over time while thrust > 0.
 *    R4 (Deferred) Low fuel (< 15%) raises an alert exactly once per crossing.
 *
 *  Acceptance (this test enforces R1,R2,R3 now; R4 placeholder red assertion for future impl):
 *    Given thrust is set to 60%
 *    When one simulation second elapses
 *    Then velocity should be > 0 and altitude > 0 and fuel level < initial.
 *    And (future) a low fuel alert appears after extended burn (NOT YET IMPLEMENTED).
 *
 *  Red Phase Intent:
 *    SimulationCore::tick currently performs no physics; these assertions MUST fail
 *    (except structural ones) until physics loop (T022+) is implemented. This preserves
 *    behavior-first development. The low fuel alert portion is explicitly marked TODO
 *    and will remain failing / skipped logic until alerting is wired.
 */

#include <gtest/gtest.h>
#include <QCoreApplication>
#include <vector>
#include "adapters/SimulationController.h"
#include "core/SimulationCore.h"
#include "services/AlertBus.h"

class DummySimulationCore : public SimulationCore {
  Q_OBJECT
public:
  using SimulationCore::SimulationCore;
};

// Need Q_OBJECT moc generation in this TU
#include "test_flight_console_behavior.moc"

TEST(FlightConsoleBehavior, ThrustDrivesVelocityAltitudeAndFuelBurn) {
  int argc = 0; char** argv = nullptr;
  QCoreApplication app(argc, argv);
  DummySimulationCore core;
  SimulationController controller(&core);

  const double initialFuel = core.fuelLevel();
  core.setThrustPercent(60.0);

  // Simulate one second.
  core.tick(1.0);

  EXPECT_GT(core.thrustPercent(), 0.0) << "Sanity: thrust should have been set";
  EXPECT_GT(core.velocity(), 0.0) << "Velocity should become > 0 when thrust > 0";
  EXPECT_GT(core.altitude(), 0.0) << "Altitude should increase after velocity > 0";
  EXPECT_LT(core.fuelLevel(), initialFuel) << "Fuel should decrease under thrust";
}

TEST(FlightConsoleBehavior, LowFuelAlertRaisedOnceWhenCrossingThreshold) {
  int argc = 0; char** argv = nullptr;
  QCoreApplication app(argc, argv);
  DummySimulationCore core;
  AlertBus bus; core.setAlertBus(&bus);
  SimulationController controller(&core);

  core.setThrustPercent(70.0);
  std::vector<Alert> captured;
  QObject::connect(&bus, &AlertBus::alertRaised, [&captured](const Alert& a){ captured.push_back(a); });

  // Burn across multiple ticks to drive fuel below threshold.
  for (int i = 0; i < 400 && core.fuelLevel() > 0.0; ++i) {
    core.tick(1.0); // 120 seconds hypothetical burn
  }
  ASSERT_FALSE(captured.empty()) << "Expected at least one low fuel alert";
  // Ensure exactly one alert despite many ticks below threshold
  EXPECT_EQ(captured.size(), 1u) << "Should only raise a single low fuel alert for first crossing";
  EXPECT_EQ(captured[0].severity, Alert::WARNING);
  EXPECT_TRUE(captured[0].message.contains("Low fuel"));
}

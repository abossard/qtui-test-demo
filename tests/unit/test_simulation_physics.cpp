#include <gtest/gtest.h>
#include <QCoreApplication>
#include "core/SimulationCore.h"

// Red phase unit test (T021): physics not implemented; ensures we detect missing thrust->velocity->altitude chain.
// Once physics is implemented, adjust expectations and potentially add tolerance-based comparisons.

class StubSimulationCore : public SimulationCore {
  Q_OBJECT
public:
  using SimulationCore::SimulationCore;
};

#include "test_simulation_physics.moc"

TEST(SimulationPhysics, ThrustProducesPositiveVelocity_RED) {
  int argc = 0; char** argv = nullptr; QCoreApplication app(argc, argv);
  StubSimulationCore core;
  core.setThrustPercent(50.0);
  core.tick(0.5); // half second
  EXPECT_GT(core.thrustPercent(), 0.0);
  EXPECT_GT(core.velocity(), 0.0) << "RED: velocity should increase under thrust (physics not yet implemented)";
}

TEST(SimulationPhysics, AltitudeIncreasesAfterVelocity_RED) {
  int argc = 0; char** argv = nullptr; QCoreApplication app(argc, argv);
  StubSimulationCore core;
  core.setThrustPercent(70.0);
  core.tick(1.0);
  EXPECT_GT(core.velocity(), 0.0) << "Prereq RED: velocity remains zero until physics done";
  EXPECT_GT(core.altitude(), 0.0) << "RED: altitude should climb when velocity > 0 (not yet implemented)";
}

#include <gtest/gtest.h>
#include <QCoreApplication>
#include "adapters/SimulationController.h"
#include "core/SimulationCore.h"

// Placeholder failing feature-level test (Task T018)
// Intention: Will be replaced with real behavior spec asserting thrust -> altitude dynamics.

class DummySimulationCore : public SimulationCore {
  Q_OBJECT
public:
  using SimulationCore::SimulationCore;
};

// Need Q_OBJECT moc generation in this TU
#include "test_flight_console_behavior.moc"

TEST(FlightConsoleBehavior, PlaceholderFailsUntilImplemented) {
  int argc = 0; char** argv = nullptr;
  QCoreApplication app(argc, argv);
  DummySimulationCore core;
  SimulationController controller(&core);
  // Intentional failing assertion to enforce behavior-first red state
  EXPECT_TRUE(false) << "Placeholder failing expectation (replace in T020)";
}

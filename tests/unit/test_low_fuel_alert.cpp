#include <gtest/gtest.h>
#include <QCoreApplication>
#include <vector>
#include "core/SimulationCore.h"
#include "core/Constants.h" // for cockpit::kLowFuelThresholdPercent
#include "services/AlertBus.h"

class AlertSimulationCore : public SimulationCore {
  Q_OBJECT
public:
  using SimulationCore::SimulationCore;
};

#include "test_low_fuel_alert.moc"

TEST(LowFuelAlert, SingleEmissionAndLatchUntilRecovery) {
  int argc = 0; char** argv = nullptr; QCoreApplication app(argc, argv);
  AlertSimulationCore core; AlertBus bus; core.setAlertBus(&bus);
  core.setThrustPercent(90.0);
  std::vector<Alert> alerts;
  QObject::connect(&bus, &AlertBus::alertRaised, [&alerts](const Alert& a){ alerts.push_back(a); });

  // Burn until below threshold
  for (int i = 0; i < 400 && alerts.empty(); ++i) core.tick(1.0);
  ASSERT_EQ(alerts.size(), 1u) << "Expected exactly one alert at threshold crossing";

  // Continue burning further below threshold; no new alerts should appear
  for (int i = 0; i < 50; ++i) core.tick(1.0);
  EXPECT_EQ(alerts.size(), 1u) << "No additional alerts while still below threshold";

  // Refuel above threshold to reset latch
  double beforeRefuel = core.fuelLevel();
  EXPECT_LT(beforeRefuel, cockpit::kLowFuelThresholdPercent);
  core.refuel(50.0); // should bring us well above threshold and reset latch
  EXPECT_GE(core.fuelLevel(), cockpit::kLowFuelThresholdPercent);

  // Burn again to trigger a second crossing and second alert
  size_t previousAlertCount = alerts.size();
  for (int i = 0; i < 500 && alerts.size() < previousAlertCount + 1; ++i) {
    core.tick(1.0);
  }
  EXPECT_EQ(alerts.size(), previousAlertCount + 1) << "Expected a second low fuel alert after refuel and re-burn";
}
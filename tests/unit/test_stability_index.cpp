#include <gtest/gtest.h>
#include <QCoreApplication>
#include "core/SimulationCore.h"

class StabilityCore : public SimulationCore { Q_OBJECT public: using SimulationCore::SimulationCore; };
#include "test_stability_index.moc"

TEST(StabilityIndex, ConstantThrustRemainsHigh) {
  int argc=0; char** argv=nullptr; QCoreApplication app(argc, argv);
  StabilityCore core; core.setThrustPercent(60.0);
  for (int i=0;i<30;++i) core.tick(0.5);
  EXPECT_GE(core.current().stabilityIndex, 0.90) << "Stability should stay high under constant thrust";
}

TEST(StabilityIndex, FluctuatingThrustLowersIndex) {
  int argc=0; char** argv=nullptr; QCoreApplication app(argc, argv);
  StabilityCore core;
  for (int i=0;i<40;++i) { core.setThrustPercent(i % 2 ? 10.0 : 90.0); core.tick(0.25); }
  EXPECT_LE(core.current().stabilityIndex, 0.75) << "Stability should drop with large thrust variance";
}

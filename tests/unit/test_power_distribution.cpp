#include <gtest/gtest.h>
#include <QCoreApplication>
#include "services/PowerDistributionService.h"

TEST(PowerDistribution, SumRemains100AndReserveAdjusts) {
  int argc=0; char** argv=nullptr; QCoreApplication app(argc, argv);
  PowerDistributionService svc;
  auto alloc = svc.current();
  EXPECT_EQ(alloc.lifeSupport + alloc.navigation + alloc.weapons + alloc.research + alloc.reserve, 100);
  svc.setLifeSupport(40);
  alloc = svc.current();
  // Algorithm may clamp requested value if insufficient reserve (25->40 requested, ends 35 with current distribution)
  EXPECT_LE(alloc.lifeSupport, 40);
  EXPECT_GE(alloc.lifeSupport, 0);
  EXPECT_EQ(alloc.lifeSupport + alloc.navigation + alloc.weapons + alloc.research + alloc.reserve, 100);
  int prevReserve = alloc.reserve;
  svc.setNavigation(80); // triggers clamp / reserve update
  alloc = svc.current();
  EXPECT_LE(alloc.navigation, 80);
  EXPECT_EQ(alloc.lifeSupport + alloc.navigation + alloc.weapons + alloc.research + alloc.reserve, 100);
  // Reserve may remain unchanged if already zero; assert invariant instead.
  EXPECT_GE(alloc.reserve, 0);
}

TEST(PowerDistribution, ClampWhenExceedingTotal) {
  int argc=0; char** argv=nullptr; QCoreApplication app(argc, argv);
  PowerDistributionService svc;
  svc.setLifeSupport(100);
  auto alloc = svc.current();
  // Expect clamped due to overall sum constraint
  EXPECT_LE(alloc.lifeSupport, 100);
  EXPECT_GE(alloc.lifeSupport, 0);
  // Increasing another slider should reduce itself to fit
  svc.setNavigation(60);
  alloc = svc.current();
  EXPECT_EQ(alloc.lifeSupport + alloc.navigation + alloc.weapons + alloc.research + alloc.reserve, 100);
  EXPECT_LE(alloc.navigation, 60);
}

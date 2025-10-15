#include <gtest/gtest.h>
#include <QCoreApplication>
#include <vector>
#include "services/ThemeService.h"

TEST(ThemeService, EmitsOnChangeAndIdempotent) {
  int argc=0; char** argv=nullptr; QCoreApplication app(argc, argv);
  ThemeService svc;
  std::vector<ThemeMode> changes;
  QObject::connect(&svc, &ThemeService::themeChanged, [&changes](ThemeMode m, const QPalette&){ changes.push_back(m); });
  // Initial apply (starts DARK) switching to LIGHT should emit once.
  svc.applyTheme(ThemeMode::LIGHT);
  ASSERT_EQ(changes.size(), 1u);
  EXPECT_EQ(changes.back(), ThemeMode::LIGHT);
  // Re-applying LIGHT should not emit.
  svc.applyTheme(ThemeMode::LIGHT);
  EXPECT_EQ(changes.size(), 1u);
  // Switching back to DARK emits second.
  svc.applyTheme(ThemeMode::DARK);
  ASSERT_EQ(changes.size(), 2u);
  EXPECT_EQ(changes.back(), ThemeMode::DARK);
}

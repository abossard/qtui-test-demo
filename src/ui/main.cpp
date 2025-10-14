// Phase 1 skeleton entry point (Task T005)
// Guarded by ENABLE_SPACE_COCKPIT build option (see root CMakeLists.txt).

#include <iostream>

#ifdef ENABLE_SPACE_COCKPIT
#include <QApplication>
#include <QLabel>

int main(int argc, char** argv) {
  QApplication app(argc, argv);
  QLabel splash("Space Cockpit Kitchensink - Skeleton\n(Phase 1 Setup)");
  splash.setObjectName("rootSplashLabel");
  splash.setAlignment(Qt::AlignCenter);
  splash.resize(520, 140);
  splash.show();
  std::clog << "[startup] Space Cockpit skeleton running (ENABLE_SPACE_COCKPIT=ON)" << std::endl;
  return app.exec();
}
#else
int main() {
  std::clog << "Space Cockpit disabled (ENABLE_SPACE_COCKPIT=OFF)" << std::endl;
  return 0;
}
#endif

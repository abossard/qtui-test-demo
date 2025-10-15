// Phase 1 skeleton entry point (Task T005)
// Guarded by ENABLE_SPACE_COCKPIT build option (see root CMakeLists.txt).

#include <iostream>

#ifdef ENABLE_SPACE_COCKPIT
#include <QApplication>
#include <QLabel>
#include <QTimer>
#include "core/SimulationCore.h"
#include "services/AlertBus.h"
#include "ui/FlightConsolePanel.h"
#include "ui/PowerPanel.h"
#include "services/PowerDistributionService.h"
#include "services/ThemeService.h"
#include "adapters/SimulationController.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>

int main(int argc, char** argv) {
  QApplication app(argc, argv);
  std::clog << "[startup] Space Cockpit running (ENABLE_SPACE_COCKPIT=ON)" << std::endl;

  auto* core = new SimulationCore();
  auto* alertBus = new AlertBus();
  core->setAlertBus(alertBus);
  auto* controller = new SimulationController(core, &app);
  controller->setThrustPercent(55.0); // initial demo thrust

  // Container widget
  QWidget window;
  window.setWindowTitle("Space Cockpit Demo");
  auto* panel = new FlightConsolePanel(&window);
  auto* powerPanel = new PowerPanel(&window);
  auto* powerService = new PowerDistributionService(&window);
  auto* themeService = new ThemeService(&window);
  powerPanel->attachService(powerService);
  panel->attachCore(core, alertBus, controller);

  // Theme toggle buttons
  auto* darkBtn = new QPushButton("Dark", &window);
  auto* lightBtn = new QPushButton("Light", &window);
  QObject::connect(darkBtn, &QPushButton::clicked, [themeService]() { themeService->applyTheme(ThemeMode::DARK); });
  QObject::connect(lightBtn, &QPushButton::clicked, [themeService]() { themeService->applyTheme(ThemeMode::LIGHT); });
  QObject::connect(themeService, &ThemeService::themeChanged, [&window](ThemeMode, const QPalette& pal){
    qApp->setPalette(pal);
  });
  // Apply initial dark theme
  themeService->applyTheme(ThemeMode::DARK);

  // Layout: left column panels, right column theme buttons
  auto* layout = new QHBoxLayout(&window);
  auto* leftCol = new QVBoxLayout();
  leftCol->addWidget(panel, 1);
  leftCol->addWidget(powerPanel, 0);
  layout->addLayout(leftCol, 1);
  auto* rightCol = new QVBoxLayout();
  rightCol->addWidget(new QLabel("Theme:"));
  rightCol->addWidget(darkBtn);
  rightCol->addWidget(lightBtn);
  rightCol->addStretch(1);
  layout->addLayout(rightCol, 0);
  window.setLayout(layout);
  window.resize(760, 300);
  window.show();

  // Drive simulation at 10Hz using controller timer (100ms interval)
  controller->startTimer(0.1);

  // Optional: connect controller telemetryTick if future consumers need adapter signal
  // QObject::connect(controller, &SimulationController::telemetryTick, [](const TelemetryState&){ /* hook */ });

  return app.exec();
}
#else
int main() {
  std::clog << "Space Cockpit disabled (ENABLE_SPACE_COCKPIT=OFF)" << std::endl;
  return 0;
}
#endif

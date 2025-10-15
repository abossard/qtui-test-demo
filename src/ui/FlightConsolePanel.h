#pragma once
#include <QWidget>
#include <QLabel>
#include <QPointer>
#include <QSlider>
#include <cmath>
#include <QString>
#include <QColor>
#include "core/types.h"

class AlertBus;
class SimulationCore;
class SimulationController;
class QPushButton;

class FlightConsolePanel : public QWidget {
  Q_OBJECT
public:
  explicit FlightConsolePanel(QWidget* parent = nullptr);
  void attachCore(SimulationCore* core, AlertBus* alertBus, SimulationController* controller = nullptr);

private slots:
  void onTelemetry(const TelemetryState& ts);
  void onAlert(const class Alert& alert);

private:
  QLabel* altitudeLabel_{};
  QLabel* velocityLabel_{};
  QLabel* fuelLabel_{};
  QLabel* stabilityLabel_{};
  QLabel* lowFuelIndicator_{};
  QLabel* alertBanner_{};
  QPushButton* refuelButton_{};
  QSlider* thrustSlider_{};
  QPointer<SimulationCore> core_;
  QPointer<SimulationController> controller_;
  QPointer<AlertBus> alertBus_;
  void updateLowFuel(double fuelPct);
  void updateStability(double idx);
};

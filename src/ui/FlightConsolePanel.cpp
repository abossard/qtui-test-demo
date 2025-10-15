#include "ui/FlightConsolePanel.h"
#include "services/AlertBus.h"
#include "core/SimulationCore.h"
#include "core/Constants.h"
#include <QVBoxLayout>
#include <QPalette>
#include <QPushButton>

FlightConsolePanel::FlightConsolePanel(QWidget* parent) : QWidget(parent) {
  auto* layout = new QVBoxLayout(this);
  altitudeLabel_ = new QLabel("Altitude: 0 m", this);
  velocityLabel_ = new QLabel("Velocity: 0 m/s", this);
  fuelLabel_     = new QLabel("Fuel: 100%", this);
  stabilityLabel_ = new QLabel("Stability: 1.00", this);
  lowFuelIndicator_ = new QLabel("Fuel OK", this);
  alertBanner_ = new QLabel("", this);
  alertBanner_->setObjectName("alertBanner");
  alertBanner_->setVisible(false);
  refuelButton_ = new QPushButton("Refuel +10%", this);
  thrustSlider_ = new QSlider(Qt::Horizontal, this);
  thrustSlider_->setObjectName("thrustSlider");
  thrustSlider_->setRange(0,100);
  thrustSlider_->setValue(55);
  layout->addWidget(altitudeLabel_);
  layout->addWidget(velocityLabel_);
  layout->addWidget(fuelLabel_);
  layout->addWidget(stabilityLabel_);
  layout->addWidget(lowFuelIndicator_);
  layout->addWidget(alertBanner_);
  layout->addWidget(new QLabel("Thrust %", this));
  layout->addWidget(thrustSlider_);
  layout->addWidget(refuelButton_);
  setLayout(layout);
  setObjectName("FlightConsolePanel");
}

void FlightConsolePanel::attachCore(SimulationCore* core, AlertBus* alertBus) {
  core_ = core; alertBus_ = alertBus;
  if (core_) {
    QObject::connect(core_, SIGNAL(telemetryProduced(TelemetryState)), this, SLOT(onTelemetry(TelemetryState)));
    QObject::connect(refuelButton_, &QPushButton::clicked, [this]() {
      if (core_) core_->refuel(10.0);
    });
    QObject::connect(thrustSlider_, &QSlider::valueChanged, [this](int v){ if (core_) core_->setThrustPercent(static_cast<double>(v)); });
  }
  if (alertBus_) QObject::connect(alertBus_, &AlertBus::alertRaised, this, &FlightConsolePanel::onAlert);
}

void FlightConsolePanel::onTelemetry(const TelemetryState& ts) {
  altitudeLabel_->setText(QString("Altitude: %1 m").arg(ts.altitude, 0, 'f', 1));
  velocityLabel_->setText(QString("Velocity: %1 m/s").arg(ts.verticalVelocity, 0, 'f', 2));
  fuelLabel_->setText(QString("Fuel: %1% ").arg(ts.fuelRemaining, 0, 'f', 1));
  updateStability(ts.stabilityIndex);
  updateLowFuel(ts.fuelRemaining);
  refuelButton_->setEnabled(ts.fuelRemaining < 99.5);
}

void FlightConsolePanel::updateLowFuel(double fuelPct) {
  if (fuelPct < cockpit::kLowFuelThresholdPercent) {
    lowFuelIndicator_->setText(QString("LOW FUEL (%1%)").arg(fuelPct, 0, 'f', 1));
    QPalette p = lowFuelIndicator_->palette();
    p.setColor(QPalette::WindowText, Qt::red);
    lowFuelIndicator_->setPalette(p);
  } else {
    lowFuelIndicator_->setText("Fuel OK");
    QPalette p = lowFuelIndicator_->palette();
    p.setColor(QPalette::WindowText, Qt::darkGreen);
    lowFuelIndicator_->setPalette(p);
  }
}

void FlightConsolePanel::onAlert(const Alert& alert) {
  alertBanner_->setText(QString("[%1] %2").arg(alert.severity == Alert::WARNING ? "WARN" : "ALERT", alert.message));
  alertBanner_->setVisible(true);
  QPalette p = alertBanner_->palette();
  p.setColor(QPalette::WindowText, alert.severity == Alert::CRITICAL ? Qt::red : Qt::yellow);
  alertBanner_->setPalette(p);
}

void FlightConsolePanel::updateStability(double idx) {
  stabilityLabel_->setText(QString("Stability: %1").arg(idx,0,'f',2));
  QPalette p = stabilityLabel_->palette();
  QColor c = Qt::green;
  if (idx < 0.6) c = Qt::red; else if (idx < 0.85) c = QColor("#d0a000");
  p.setColor(QPalette::WindowText, c);
  stabilityLabel_->setPalette(p);
}

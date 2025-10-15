#pragma once
#include <QWidget>
#include <QSlider>
#include <QLabel>
#include <QGridLayout>
#include "core/types.h"

class PowerDistributionService;

class PowerPanel : public QWidget {
  Q_OBJECT
public:
  explicit PowerPanel(QWidget* parent=nullptr);
  void attachService(PowerDistributionService* svc);

private slots:
  void onLifeChanged(int v);
  void onNavChanged(int v);
  void onWeaponsChanged(int v);
  void onResearchChanged(int v);
  void onPowerChanged(const PowerAllocation& alloc);

private:
  QSlider *life_{}, *nav_{}, *weapons_{}, *research_{};
  QLabel *reserveLabel_{};
  PowerDistributionService* service_{nullptr};
  void applyStyle(QSlider* s);
};

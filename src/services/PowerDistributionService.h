#pragma once
#include <QObject>
#include "core/types.h"

class PowerDistributionService : public QObject {
  Q_OBJECT
public:
  explicit PowerDistributionService(QObject* parent=nullptr) : QObject(parent) {}
  PowerAllocation current() const { return allocation_; }

  // Set one category (lifeSupport, navigation, weapons, research). Reserve auto-adjusted.
  void setLifeSupport(int v) { setAndRebalance(v, allocation_.lifeSupport, &PowerAllocation::lifeSupport); }
  void setNavigation(int v)  { setAndRebalance(v, allocation_.navigation, &PowerAllocation::navigation); }
  void setWeapons(int v)     { setAndRebalance(v, allocation_.weapons, &PowerAllocation::weapons); }
  void setResearch(int v)    { setAndRebalance(v, allocation_.research, &PowerAllocation::research); }

signals:
  void powerChanged(const PowerAllocation&);

private:
  PowerAllocation allocation_{}; // initialized with defaults summing to <= 100

  template<typename MemberPtr>
  void setAndRebalance(int value, int& fieldRef, MemberPtr member) {
    if (value < 0) value = 0; if (value > 100) value = 100;
    fieldRef = value;
    // Compute sum without reserve
    int mainSum = allocation_.lifeSupport + allocation_.navigation + allocation_.weapons + allocation_.research;
    if (mainSum > 100) {
      // Reduce the just-changed field to fit
      int overflow = mainSum - 100;
      fieldRef -= overflow;
      if (fieldRef < 0) fieldRef = 0; // minimal safety
      mainSum = allocation_.lifeSupport + allocation_.navigation + allocation_.weapons + allocation_.research;
    }
    allocation_.reserve = 100 - mainSum;
    emit powerChanged(allocation_);
  }
};

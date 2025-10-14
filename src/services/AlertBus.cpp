#include "services/AlertBus.h"
#include <QDateTime>

void AlertBus::raise(Alert alert) {
  if (alert.id.isEmpty()) {
    alert.id = QString("ALRT-%1-%2").arg(QDateTime::currentMSecsSinceEpoch()).arg(reinterpret_cast<quintptr>(this) & 0xFFFF, 4, 16, QChar('0'));
  }
  if (alert.activeSince == 0) alert.activeSince = QDateTime::currentMSecsSinceEpoch();
  emit alertRaised(alert);
}

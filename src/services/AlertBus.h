#pragma once
#include <QObject>
#include <QString>
#include <QVector>

struct Alert {
  QString id;
  enum Severity { CRITICAL, WARNING, INFO } severity{INFO};
  QString message;
  qint64 activeSince{0};
  bool acknowledged{false};
};

// Alert bus stub (Task T014)
class AlertBus : public QObject {
  Q_OBJECT
public:
  explicit AlertBus(QObject* parent = nullptr) : QObject(parent) {}
  void raise(Alert alert);
signals:
  void alertRaised(const Alert&);
};

#pragma once
#include <QObject>
#include <QString>
#include <QJSEngine>

// Script program status enum (simplified)
enum class ScriptStatus { INVALID, VALID, ACTIVE, TIMEOUT, ERROR };

// Script sandbox stub (Task T016)
class ScriptSandbox : public QObject {
  Q_OBJECT
public:
  explicit ScriptSandbox(QObject* parent = nullptr) : QObject(parent) {}
  bool load(const QString& source);
  bool activate();
  void deactivate();
  ScriptStatus status() const { return status_; }
signals:
  void scriptStatusChanged(ScriptStatus);
private:
  QJSEngine engine_{}; // future: limited API exposure
  QString source_;
  ScriptStatus status_{ScriptStatus::INVALID};
};

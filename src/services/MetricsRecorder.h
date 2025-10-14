#pragma once
#include <QObject>
#include <QString>
#include <QHash>
#include <QElapsedTimer>

// Minimal metrics recorder stub (Task T012)
class MetricsRecorder : public QObject {
  Q_OBJECT
public:
  explicit MetricsRecorder(QObject* parent = nullptr) : QObject(parent) {}
  void increment(const QString& key) { counters_[key] += 1; }
  void recordDurationMs(const QString& key, qint64 ms) { durations_[key] = ms; }
  qint64 counter(const QString& key) const { return counters_.value(key, 0); }
  qint64 duration(const QString& key) const { return durations_.value(key, -1); }
private:
  QHash<QString, qint64> counters_;
  QHash<QString, qint64> durations_;
};

#pragma once
#include <QObject>
#include "core/types.h"

// Replay mode enum
enum class ReplayMode { LIVE, REPLAYING };

// Replay service stub (Task T015)
class ReplayService : public QObject {
  Q_OBJECT
public:
  explicit ReplayService(QObject* parent = nullptr) : QObject(parent) {}
  ReplayMode mode() const { return mode_; }
  void startReplay() {
    if (mode_ == ReplayMode::REPLAYING) return;
    mode_ = ReplayMode::REPLAYING;
    emit replayStateChanged(true);
  }
  void stopReplay() {
    if (mode_ == ReplayMode::LIVE) return;
    mode_ = ReplayMode::LIVE;
    emit replayStateChanged(false);
  }
signals:
  void replayStateChanged(bool replaying);
private:
  ReplayMode mode_{ReplayMode::LIVE};
};

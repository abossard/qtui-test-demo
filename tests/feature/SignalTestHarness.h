#pragma once
#include <QObject>
#include <QSignalSpy>
#include <optional>

// Simple signal wait harness (Task T011)
namespace testutil {

inline bool waitForSignal(QObject* sender, const char* signal, int timeoutMs = 2000) {
  QSignalSpy spy(sender, signal);
  if (!spy.isValid()) return false;
  return spy.wait(timeoutMs);
}

} // namespace testutil

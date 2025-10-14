#include "services/ScriptSandbox.h"

bool ScriptSandbox::load(const QString& source) {
  source_ = source;
  // Placeholder: treat non-empty as valid
  status_ = source_.isEmpty() ? ScriptStatus::INVALID : ScriptStatus::VALID;
  emit scriptStatusChanged(status_);
  return status_ == ScriptStatus::VALID;
}

bool ScriptSandbox::activate() {
  if (status_ != ScriptStatus::VALID) return false;
  status_ = ScriptStatus::ACTIVE;
  emit scriptStatusChanged(status_);
  return true;
}

void ScriptSandbox::deactivate() {
  if (status_ == ScriptStatus::ACTIVE) {
    status_ = ScriptStatus::VALID; // revert to compiled
    emit scriptStatusChanged(status_);
  }
}

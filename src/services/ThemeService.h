#pragma once
#include <QObject>

enum class ThemeMode { DARK, LIGHT, HIGH_CONTRAST };

// Theme service stub (Task T013)
class ThemeService : public QObject {
  Q_OBJECT
public:
  explicit ThemeService(QObject* parent = nullptr) : QObject(parent) {}
  ThemeMode currentTheme() const { return theme_; }
  void setTheme(ThemeMode m) {
    if (m == theme_) return;
    theme_ = m;
    emit themeChanged(theme_);
  }
signals:
  void themeChanged(ThemeMode);
private:
  ThemeMode theme_{ThemeMode::DARK};
};

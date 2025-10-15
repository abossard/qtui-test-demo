#pragma once
#include <QObject>
#include <QPalette>

enum class ThemeMode { DARK, LIGHT };

// Theme service provides application-wide palette switching.
class ThemeService : public QObject {
  Q_OBJECT
public:
  explicit ThemeService(QObject* parent = nullptr) : QObject(parent) {}
  ThemeMode currentTheme() const { return theme_; }

  void applyTheme(ThemeMode m) {
    if (m == theme_) return; // idempotent
    theme_ = m;
    emit themeChanged(theme_, paletteFor(theme_));
  }

  static QPalette paletteFor(ThemeMode m) {
    if (m == ThemeMode::LIGHT) {
      QPalette pal;
      pal.setColor(QPalette::Window, QColor(240,240,240));
      pal.setColor(QPalette::WindowText, Qt::black);
      pal.setColor(QPalette::Base, Qt::white);
      pal.setColor(QPalette::AlternateBase, QColor(225,225,225));
      pal.setColor(QPalette::ToolTipBase, Qt::white);
      pal.setColor(QPalette::ToolTipText, Qt::black);
      pal.setColor(QPalette::Text, Qt::black);
      pal.setColor(QPalette::Button, QColor(235,235,235));
      pal.setColor(QPalette::ButtonText, Qt::black);
      pal.setColor(QPalette::Highlight, QColor(30,144,255));
      pal.setColor(QPalette::HighlightedText, Qt::white);
      return pal;
    }
    // Dark palette
    QPalette dark;
    dark.setColor(QPalette::Window, QColor(40,40,48));
    dark.setColor(QPalette::WindowText, Qt::white);
    dark.setColor(QPalette::Base, QColor(30,30,36));
    dark.setColor(QPalette::AlternateBase, QColor(45,45,52));
    dark.setColor(QPalette::ToolTipBase, QColor(80,80,90));
    dark.setColor(QPalette::ToolTipText, Qt::white);
    dark.setColor(QPalette::Text, Qt::white);
    dark.setColor(QPalette::Button, QColor(55,55,65));
    dark.setColor(QPalette::ButtonText, Qt::white);
    dark.setColor(QPalette::Highlight, QColor(90,140,200));
    dark.setColor(QPalette::HighlightedText, Qt::black);
    return dark;
  }

signals:
  void themeChanged(ThemeMode, const QPalette&);

private:
  ThemeMode theme_{ThemeMode::DARK};
};

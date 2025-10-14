# Squish for Qt (C++/Qt Widgets & QML) – Research & Practical Guide

> IMPORTANT: Squish **does not execute test scripts written in C++**. Supported scripting languages are (per official material) **Python, JavaScript, Perl, Ruby, Tcl**. At your request, all examples below have been converted to **C++ AUT-side code** (showing how a Qt application can be structured for testability) instead of Squish scripting snippets. Comments indicate how Squish symbolic names would typically target these widgets. Keep using a supported scripting language for the actual Squish test scripts; map the concepts 1:1.

Focus: Core functionality, testability patterns, tables/models & scrolling techniques, and Qt signal handling—now illustrated from the perspective of C++ application code design. All descriptions are paraphrased; see the Reference Links section for authoritative docs.

---
## 1. Overview
Squish is a GUI test automation tool that drives Qt (Widgets & QML) applications at the object level. It provides:
- Multi-language scripting (Python, JavaScript, Ruby, Perl, Tcl)
- Object map abstraction (symbolic names → real object queries)
- Toolkit-specific convenience APIs (e.g. `clickButton`, `clickItem` for Qt item views)
- Generic interaction primitives (mouse, keyboard, image/ocr, properties)
- Synchronization helpers (`waitForObject`, `waitForSignal`, property waits)
- Logging & verification (`test.log`, `test.compare`, `test.verify`, `test.fail`)
- Data-driven & modularization support (shared scripts, global scripts, test data tables)

Typical test flow:
1. Start or attach to AUT (`startApplication`, `attachToApplication`)
2. Resolve objects (symbolic name or ad‑hoc query) using `waitForObject`
3. Interact (click, type, select)
4. Synchronize (wait for UI state or signal)
5. Verify (content, properties, visuals) and log results
6. Teardown (optional) or proceed to next case

---
## 2. Scripting Structure & Core Syntax
### 2.1 Project Layout
- Test Suite: Folder containing shared scripts, object map, test cases.
- Test Case: Script file (e.g. `test_case.py`) with an entry function `main()`.
- Object Map: Maps symbolic names like `:LoginDialog.usernameEdit` to a real name (property-based query hash or textual form) so tests resist UI changes.

### 2.2 Key Functions / Objects (Selected)
| Category | Representative API (non-exhaustive) | Purpose |
|----------|--------------------------------------|---------|
| AUT lifecycle | `startApplication(appName)`, `attachToApplication(pattern)` | Launch or attach to running AUT |
| Object lookup | `waitForObject(nameOrQuery)`, `findObject(nameOrQuery)`, `waitForObjectItem(view, itemPath)` | Resolve widget references (waiting vs immediate) |
| Interaction (generic) | `mouseClick(obj, x, y, button, modifier)`, `mouseDrag`, `mouseWheel`, `keyPress`, `type(text)` | Low-level events |
| Interaction (Qt convenience) | `clickButton(obj)`, `doubleClickItem(view, path)`, `clickItem(view, path)` | Higher-level semantic actions |
| Verification | `test.compare(a, b)`, `test.verify(cond, msg)`, `test.log(value)`, `test.fail(msg)` | Assertions & logging |
| Synchronization | `waitForObject`, `waitForSignal(obj, "sig()")`, property polling, timeouts via `testSettings.waitForObjectTimeout` | Ensure stable state before acting |
| Object info | `object.properties(obj)`, `className(obj)`, `obj.children`, `grabWidget(obj)` | Introspection |
| Object map | `objectMap.symbolicName(obj)`, `objectMap.realName(symbolic)` | Indirection/maintenance |
| Data / utilities | `testData.field(row, col)`, `findImage`, `findAllObjects(query)` | Data-driven & discovery |

### 2.3 Symbolic Names vs Real Names
- Symbolic names (preferred in scripts) offer stability; the real name is a property-based query (e.g. `{type='QLineEdit' name='userNameEdit' visible='1'}`) that Squish can regenerate.
- If live debugging or quick prototyping: you may inline real name queries; migrate stable ones into object map later.

### 2.4 Waiting & Finding
- `waitForObject` waits up to configured timeout and raises if not found → safer for visible widgets.
- `findObject` returns immediately (fast fail) → good for non-visual or already-known existing objects.
- `waitForObjectItem` and item convenience functions help with complex view hierarchies (trees, lists, tables).

### 2.5 Property Access & Modification
In Python you can directly read/write Qt properties exposed via the meta-object system (e.g. `lineEdit.text`, `scrollBar.value`). Some properties may need method invocation if side-effects are required (e.g. using a setter ensures signals fire consistently in the AUT).

### 2.6 Timeouts & Settings
- Global test settings via `testSettings` (e.g. `testSettings.waitForObjectTimeout` in ms)
- You can pass explicit timeout as second param in some waits or as an override (e.g. `waitForObject(name, 10000)`).

---
## 3. Basic UI Test Example (Qt Widgets – C++ AUT Perspective)
Below is C++ code for a simple login dialog and main window. Squish tests (written in a supported scripting language) would interact with the objects whose `objectName` values are set. Comments show likely symbolic names (e.g. `:LoginDialog.usernameEdit`).

```cpp
// login_dialog.h
#pragma once
#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>

class LoginDialog : public QDialog {
    Q_OBJECT
public:
    explicit LoginDialog(QWidget* parent = nullptr) : QDialog(parent) {
        setObjectName("LoginDialog");
        m_user = new QLineEdit(this);  m_user->setObjectName("usernameEdit");
        m_pass = new QLineEdit(this);  m_pass->setObjectName("passwordEdit");
        m_pass->setEchoMode(QLineEdit::Password);
        m_login = new QPushButton(tr("Login"), this); m_login->setObjectName("loginButton");
        auto *lay = new QVBoxLayout; lay->addWidget(m_user); lay->addWidget(m_pass); lay->addWidget(m_login); setLayout(lay);
        connect(m_login, &QPushButton::clicked, this, &LoginDialog::onLoginClicked);
    }
signals:
    void credentialsAccepted(const QString& user);
private slots:
    void onLoginClicked() {
        if (!m_user->text().isEmpty() && !m_pass->text().isEmpty()) {
            emit credentialsAccepted(m_user->text());
            accept();
        }
    }
private:
    QLineEdit *m_user {nullptr};
    QLineEdit *m_pass {nullptr};
    QPushButton *m_login {nullptr};
};
```

```cpp
// main_window.h
#pragma once
#include <QMainWindow>
#include <QLabel>
#include <QMenuBar>
class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(const QString& user, QWidget* parent=nullptr) : QMainWindow(parent) {
        setObjectName("MainWindow");
        m_status = new QLabel(tr("Welcome, %1").arg(user), this); m_status->setObjectName("statusLabel");
        setCentralWidget(m_status);
        QMenu *fileMenu = menuBar()->addMenu(tr("File"));
        fileMenu->setObjectName("menu_File");
    }
private:
    QLabel *m_status {nullptr};
};
```

```cpp
// main.cpp
#include <QApplication>
#include "login_dialog.h"
#include "main_window.h"
int main(int argc, char **argv) {
    QApplication app(argc, argv);
    LoginDialog dlg;
    MainWindow *mw = nullptr;
    QObject::connect(&dlg, &LoginDialog::credentialsAccepted, [&](const QString& user){
        mw = new MainWindow(user);
        mw->show();
    });
    dlg.exec();
    return app.exec();
}
```

Test intentions (mapped to Squish symbolic names):
1. `:LoginDialog.usernameEdit` → type username
2. `:LoginDialog.passwordEdit` → type password
3. `:LoginDialog.loginButton` → click
4. `:MainWindow` appears → wait
5. `:MainWindow.statusLabel` text equals `Welcome, <User>`
6. `:MainWindow.menu_File` exists

Key points for testability:
- Assign stable `objectName` values.
- Keep logic in slots—tests trigger signals by simulating UI actions.
- Avoid dynamic object names that include volatile data (timestamps, etc.).

---
## 4. Working with Tables, Models & Scrolling (C++ AUT Perspective)
Provide stable `objectName` values on views and (optionally) delegate widgets so Squish can locate them.

### 4.1 Simple Model Implementation
```cpp
// orders_model.h
#pragma once
#include <QAbstractTableModel>
struct Order { int id; QString customer; QString status; };

class OrdersModel : public QAbstractTableModel {
    Q_OBJECT
public:
    explicit OrdersModel(QObject* parent=nullptr) : QAbstractTableModel(parent) {}
    int rowCount(const QModelIndex& parent = {}) const override { Q_UNUSED(parent); return m_data.size(); }
    int columnCount(const QModelIndex& parent = {}) const override { Q_UNUSED(parent); return 3; }
    QVariant data(const QModelIndex& idx, int role) const override {
        if (!idx.isValid() || role != Qt::DisplayRole) return {};
        const auto &o = m_data.at(idx.row());
        switch(idx.column()) { case 0: return o.id; case 1: return o.customer; case 2: return o.status; }
        return {};
    }
    QVariant headerData(int section, Qt::Orientation orient, int role) const override {
        if (role != Qt::DisplayRole || orient != Qt::Horizontal) return {};
        switch(section){ case 0: return "ID"; case 1: return "Customer"; case 2: return "Status"; }
        return {};
    }
    void addOrder(const Order& o) {
        beginInsertRows({}, m_data.size(), m_data.size());
        m_data.push_back(o);
        endInsertRows();
    }
private:
    QVector<Order> m_data { { {1,"Acme","Pending"}, {2,"Globex","Shipped"}, {3,"Initech","Pending"} } };
};
```

### 4.2 View Setup
```cpp
// inside MainWindow constructor (after creating menu)
auto *tableView = new QTableView(this);
tableView->setObjectName("ordersTable");
m_ordersModel = new OrdersModel(this);
tableView->setModel(m_ordersModel);
setCentralWidget(tableView); // or add to a splitter
```

### 4.3 Editing Support
To allow Squish to trigger in-place editing (e.g. double click), ensure default edit triggers:
```cpp
tableView->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::EditKeyPressed);
```

### 4.4 Scrolling & Visibility
Expose consistent row heights (default is fine). Squish can auto-scroll when using item path helpers; alternatively ensure the vertical scrollbar has a stable `objectName`:
```cpp
tableView->verticalScrollBar()->setObjectName("ordersTable_vScroll");
```

### 4.5 Programmatic Row Selection (Test Hook Option)
```cpp
void MainWindow::selectOrderRow(int r) {
    if (auto *tv = findChild<QTableView*>("ordersTable")) {
        tv->selectRow(r);
    }
}
```
Tests could invoke selection by simulating a click; this helper is optional for internal diagnostics.

### 4.6 Header Verification Strategy (From Test Side)
Because headers return display data via `headerData`, providing deterministic strings (`ID`, `Customer`, `Status`) allows straightforward assertions by the Squish script.

### 4.7 Large Table Considerations
- Implement lazy loading via `fetchMore` if necessary; surface progress via a `QProgressBar` with `objectName`.
- Emphasize filter widgets (e.g. a `QLineEdit` named `orderFilterEdit`) so tests can reduce data set before assertions.

### 4.8 Scrolling Behavior
If pixel-perfect scrolling is needed: 
```cpp
tableView->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
```
This ensures smoother wheel steps; Squish wheel events then move by pixel rather than row.

---
## 5. Signal Handling (Qt C++ Perspective)
Design signals so Squish can synchronize reliably.

### 5.1 Emitting Title Change
```cpp
// inside MainWindow after some async operation finishes
setWindowTitle("Dashboard – Ready"); // Emits titleChanged() automatically
```
Test script (Python/JS) would call `waitForSignal(waitForObject(":MainWindow"), "titleChanged()")` then verify `windowTitle` property.

### 5.2 Custom Loader Object
```cpp
// data_loader.h
#pragma once
#include <QObject>
#include <QTimer>
class DataLoader : public QObject {
    Q_OBJECT
public:
    DataLoader() { setObjectName("DataLoader"); }
public slots:
    void startLoad() {
        QTimer::singleShot(200, this, [this]{ emit dataLoaded(42); });
    }
signals:
    void dataLoaded(int count);
};
```
Expose instance via `objectName` = `DataLoader`. Squish can `waitForObject(":DataLoader")` then `waitForSignal(obj, "dataLoaded(int)")`.

### 5.3 Progress Example
```cpp
// progress_task.h
class ProgressTask : public QObject {
    Q_OBJECT
    Q_PROPERTY(int progress READ progress NOTIFY progressUpdated)
public:
    explicit ProgressTask(QObject* parent=nullptr):QObject(parent){ setObjectName("ProgressTask"); }
    int progress() const { return m_progress; }
    void start() {
        m_progress = 0; emit progressUpdated(m_progress);
        for (int i=1;i<=100;++i) {
            QTimer::singleShot(i*5, this, [this,i]{ m_progress = i; emit progressUpdated(m_progress); });
        }
    }
signals:
    void progressUpdated(int value);
private:
    int m_progress {0};
};
```
Test side: wait for `progressUpdated(int)` several times or poll until `progress == 100`.

### 5.4 Rare / Intermittent Signals
Provide fallback explicit state queries (e.g. final model size) after expecting a signal. Consider adding a diagnostic signal (`diagnosticMessage(QString)`) emitted on internal branches to aid triage.

### 5.5 Test Hooks (Optional)
Under a compile flag, you can expose a `Q_INVOKABLE` method to dump internal state that Squish can call via the scripting language using Qt meta-object invocation.

```cpp
#ifdef SQUISH_TEST_HOOKS
Q_INVOKABLE QString dumpOrderSummary() const {
    return QString::number(m_ordersModel->rowCount()) + " orders";
}
#endif
```

---
## 6. Synchronization Patterns Cheat Sheet
| Scenario | Recommended Pattern |
|----------|--------------------|
| Widget must appear | `waitForObject(symbolicName)` |
| Background computation finishes | `waitForSignal(object, "finished()")` |
| Property reaches value | Loop + `object.property == expected` with timeout, or convenience wait if available |
| Table content grows | Capture `before = model.rowCount()` → trigger → wait loop until `model.rowCount() > before` |
| Progress bar completes | Poll `value` property after `waitForSignal` or during loop |
| Delayed dialog | `waitForObject("*:QDialog")` with broader query then refine |

---
## 7. Robustness & Maintainability Tips
- Keep object map curated; remove stale entries after refactors.
- Use descriptive symbolic names (DialogName.controlRole) for clarity.
- Wrap common flows (login, data load) in helper functions.
- Add defensive waits right before interactions that historically flake.
- Prefer verifying business effect (model count, status text) over just presence of a widget.
- Limit reliance on fixed sleeps; use event or condition-driven waits.
- For multi-language UI testing, generate locale-specific object maps programmatically (strategy referenced in official docs).

---
## 8. Common Pitfalls
| Pitfall | Mitigation |
|---------|------------|
| Using `findObject` on not-yet-realized widget | Switch to `waitForObject` |
| Flaky timing due to animations | Wait on final property/state rather than immediate signal only |
| Hard-coded coordinates | Use object-relative queries / item paths / model indices |
| Overly specific queries (e.g. volatile geometry) | Limit to stable identifiers: `type`, `name`, key text |
| Silent test pass despite missing assertion | Always assert post-action state; log intermediate states |
| Signal never emitted causing long timeout | Combine action pre-check, then `waitForSignal` with shorter custom timeout & fallback diagnostics |

---
## 9. Utility Snippets (C++ Testability Aids)
### 9.1 Assigning Object Names Systematically
```cpp
void OrderRowWidget::init() {
    customerLabel->setObjectName(QString("orderRow_customer_%1").arg(orderId));
}
```
Ensures Squish can target specific row elements when necessary (use sparingly to avoid churn).

### 9.2 Test Hook Wrapper (Conditional Compilation)
```cpp
#ifdef SQUISH_TEST_HOOKS
Q_INVOKABLE int totalOrders() const { return m_ordersModel->rowCount(); }
#endif
```

### 9.3 Facilitating Deterministic Timing
Replace chained singleshot timers with a small internal scheduler object exposing a `drain()` method (invoked only in test builds) to advance state instantly, reducing flakiness.

### 9.4 Stable Identifiers for Dynamic Menus
```cpp
QAction *act = menu->addAction(tr("Export"));
act->setObjectName("action_Export");
```
Avoids reliance on action text in locales with translation variance.

---
## 10. Reference Links (Authoritative Documentation)
(Access these for full, definitive semantics and updated API details.)
- Squish API Overview: https://doc.qt.io/squish/squish-api.html
- Object Identification: https://doc.qt.io/squish/how-to-identify-and-access-objects.html
- Qt Convenience Functions: https://doc.qt.io/squish/qt-convenience-api.html
- `findObject` Function: https://doc.qt.io/squish/findobject-function.html
- `clickItem` (Qt): https://doc.qt.io/squish/qt-clickitem.html
- `mouseWheel` (Qt): https://doc.qt.io/squish/qt-mousewheel.html
- `waitForSignal` (Qt): https://doc.qt.io/squish/qt-waitforsignal.html
- Testing Qt Applications: https://doc.qt.io/squish/how-to-use-the-qt-api.html

---
## 11. Future Enhancements (Optional Next Steps)
- Add QML-specific examples (binding loops, `waitForSignal` for property change signals like `textChanged(QString)`).
- Include visual verification sample via `grabWidget` and image compare.
- Extend data-driven pattern with `testData` usage.
- Provide a helper layer for model navigation (e.g. name → row lookup cache).

---
## 12. Summary
This guide outlined core Squish concepts for Qt, now re-framed with **C++ AUT design examples** (since Squish itself doesn’t run C++ scripts). Use the reference links plus supported scripting languages (Python/JS/etc.) to implement the actual automation.

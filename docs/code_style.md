# Code Style Guide

Applies to all C++ and Qt source in this repository. Deviations MUST be justified in code review.

## Language & Versions
- Target Standard: **C++23** (use C++20 only if a specific Qt 6.8 incompatibility is documented with link to upstream issue or Qt bug ID).
- Qt: **6.8 LTS**.

## Formatting (clang-format)
A `.clang-format` file SHOULD be added (pending); default baseline:
- BasedOnStyle: LLVM (with modifications below)
- ColumnLimit: 120
- IndentWidth: 4
- ContinuationIndentWidth: 8
- PointerAlignment: Left
- AllowShortFunctionsOnASingleLine: Empty
- SortIncludes: true (group order: std / qt / third_party / local)

(If not yet present, create `.clang-format` using above; CI will fail on mismatch once enforced.)

## File Conventions
- One primary class/struct per header unless strongly cohesive.
- Use `#pragma once` (Qt 6.8 supported compilers accept it) – no traditional include guards unless required by external tooling.
- Headers own declarations only; implementation in `.cpp` except trivial inline templates.

## Naming
| Entity | Style | Example |
|--------|-------|---------|
| Classes / Structs | PascalCase | `MainWindow`, `OrdersModel` |
| Functions / Methods | camelCase | `selectOrderRow`, `addOrder` |
| Variables (locals) | camelCase | `rowCount`, `userName` |
| Members | `m_` prefix + camelCase | `m_ordersModel` |
| Constants / Enums | PascalCase or SCREAMING_SNAKE for macros | `DefaultTimeoutMs` |
| Namespaces | lower_snake | `ui_tests`, `test_hooks` |
| Qt `objectName` | descriptiveCamelCase / namespaced pattern | `ordersTable_vScroll`, `loginButton` |

`objectName` MUST be stable and omit volatile values (timestamps, random IDs). Suffix scrollbars or specialized widgets (`_vScroll`, `_hHeader`).

## Includes & Dependencies
- Prefer forward declarations in headers when possible; include only what you use.
- Order groups (blank line separated):
  1. C / C++ std libs
  2. Qt framework headers (alphabetical within group)
  3. Third-party libs
  4. Local project headers
- Never include headers from tests inside production code.

## Error Handling & Assertions
- Use Qt signals / return status objects for recoverable conditions.
- For invariants: `Q_ASSERT` (debug only) plus a defensive runtime check if violating would corrupt state.

## Logging
- (Planned) Introduce lightweight logging wrapper with categories (e.g., `ui.lifecycle`, `data.load`). Until then, prefer `qInfo()` / `qWarning()` sparingly.

## Testing Hooks
- Guard test-only introspection with `#ifdef SQUISH_TEST_HOOKS`.
- Test hooks MUST NOT mutate production state or introduce timing changes unless explicitly labeled.

## Modern C++ Usage Guidelines
- Prefer `std::unique_ptr` / RAII over manual `new` where ownership semantics matter; Qt parent-child hierarchy is acceptable for most widgets.
- Use `std::optional` for nullable value semantics instead of sentinel values.
- Use range-based for loops and `<algorithm>` when clearer.
- Avoid premature `constexpr` unless compile-time evaluation materially improves clarity or performance.
- Coroutines: Only if example explicitly demonstrates async patterns; clearly label.

## Qt Specific
- Set `objectName` in constructors immediately after widget creation.
- Avoid subclassing if a composition approach (signals/slots) suffices.
- Prefer model/view patterns over manual table population.
- Always list required signals for synchronization in example documentation.

## Copilot / AI Generated Code Policy
- All AI-assisted blocks MUST be reviewed; remove redundant wrappers, ensure Principle I (Simplicity) is preserved.
- Commit message SHOULD tag with `ai:` if majority generated.

## Commit Messages
Format: `<type>: <description>` where type in {feat, fix, refactor, docs, test, chore}. Scope optional.

## Example Skeleton (Naming + Includes)
```cpp
#include <QTableView>
#include <QVBoxLayout>
#include <memory>

#include "orders_model.h" // local include last

class OrdersPanel : public QWidget {
    Q_OBJECT
public:
    explicit OrdersPanel(QWidget* parent = nullptr);
private:
    QTableView *m_table {nullptr};
};
```

## Enforcement Roadmap
1. Add `.clang-format` (baseline above).
2. Add formatting CI job (clang-format --dry-run --Werror).
3. Add script to verify no unguarded `SQUISH_TEST_HOOKS` usages.
4. Integrate flakiness reporting.

## Deviation Process
Any deviation requires:
- Inline comment `// DEVIATION:<ticket-id> <short reason>`
- Linked issue describing rationale and sunset plan.

---
This guide is authoritative for style; amendments follow the Constitution governance process.

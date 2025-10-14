## Quickstart: Space Cockpit Kitchensink

### 1. Prerequisites
- Qt 6.8 LTS (Widgets + QJSEngine modules; optional: QML if later extended)
- CMake ≥ 3.24 (planned build system)
- C++23 capable compiler (Clang 17+, GCC 13+, MSVC 19.38+)
- GoogleTest / GoogleMock (FetchContent or external) for unit/integration tests
- Squish (installed separately) for GUI behavior tests

### 2. Repository Layout (Feature-Relevant)
```
src/
  core/          # Pure logic (physics, power, thresholds, ring buffer)
  services/      # ScriptSandbox, ReplayService, AlertBus, ThemeService
  ui/            # Panels & main window (objectName anchors)
  adapters/      # Glue: converts core structs → Qt signals
tests/
  feature/       # Behavior-level tests (C++ harness) – to be added Phase 2
  unit/          # Pure logic tests (GoogleTest)
specs/001-space-cockpit-kitchensink/
  plan.md
  research.md
  data-model.md
  contracts/openapi.yaml
```

### 3. Build (Planned)
Create initial CMakeLists (future):
- Enable C++23, fallback guard macro if Qt forces C++20
- Option `ENABLE_SPACE_COCKPIT=ON`
- Link required Qt components (Core, Widgets, Gui, Qml?)

### 4. Running (Planned Flow)
1. Launch executable -> main window loads Dark theme by default.
2. Panels initialize in passive state until first `telemetryTick` (within 100ms).
3. Interact with thruster slider; observe altitude & stability changes.
4. Modify power sliders; reserve adjusts automatically.
5. Open Settings → adjust thresholds → save → new anomalies trigger accordingly.
6. Load autopilot script sample → activate → watch thrust adjustments.
7. Trigger replay (toolbar) → live updates pause → scrub timeline → stop → continuity marker logged.

### 5. Testing Strategy (Behavior-First)
Feature-level tests (added in Phase 2) will:
1. Start minimal simulation harness (headless, no full UI where possible).
2. Wait on required signals with bounded timeout (2s) instead of sleeps.
3. Assert invariants (e.g., power sum == 100) and Success Criteria metrics (hover stabilization time).

Unit tests cover:
- Physics tick integration (thrust → velocity → altitude).
- Power redistribution algorithm edge cases.
- Threshold anomaly detection windows.
- Replay ring buffer overwrite behavior.

Integration tests cover:
- ScriptSandbox activation + watchdog timeout path.
- ReplayService mutual exclusion with ScriptSandbox.

GUI (Squish) tests (later):
- Validate objectName presence & theming switch repaint <300ms.
- Alert stack ordering under multi-alert burst.

### 6. Accessibility & Theming
- Palette contrast auto-check (planned utility) validates WCAG AA before merge.
- Theme switch triggers metrics capture (`theme.switch.ms`).

### 7. Metrics Collection (Planned Hooks)
Instrumentation points (guarded by build flag):
- Tick duration
- Alert latency
- Replay drift frames
- Script execution time

### 8. Next After Quickstart
- Add initial CMake scaffolding.
- Implement SimulationCore with deterministic loop (mockable clock for tests).
- Author failing feature tests (Phase 2 tasks).

---
This quickstart evolves with implementation; any deviation requires updating plan Constitution Check evidence.

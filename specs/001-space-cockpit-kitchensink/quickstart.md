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

### 4a. Current Running Feature Set (Refuel, Stability, Theming)
Implemented so far:
- Physics loop: thrust → acceleration → velocity → altitude + proportional fuel burn.
- Stability index (0..1): variance-based metric of recent thrust samples (steady thrust ≈ 1.00; volatile thrust lowers score). Color coding: green ≥0.85, amber 0.60–<0.85, red <0.60.
- Low fuel alert: single WARNING emitted when first crossing below 15% fuel. Alert latch resets automatically once fuel rises back ≥15% (e.g., via refuel) allowing a future re-crossing to emit again.
- Refuel mechanism: `Refuel +10%` button increments fuel (capped 100%), disabled above ~99.5%. Crossing threshold upward clears the low fuel latch.
- `FlightConsolePanel` shows altitude, velocity, fuel %, stability index (with color), low fuel status, latest alert banner, and refuel button.
- `PowerPanel` sliders (life, nav, weapons, research) auto-adjust reserve to keep total = 100.
- Theming: Dark/Light toggle buttons update a shared palette (default Dark). Theme changes propagate via `ThemeService::themeChanged` carrying palette.

Demo behavior after build:
1. App starts (Dark theme) with thrust preset (~55%) and begins ticking every 1s.
2. Altitude & velocity rise; fuel gradually decreases; stability usually near 1 unless you manipulate thrust.
3. When fuel <15%, low fuel alert emits once (banner visible). Repeated ticks below threshold produce no duplicates.
4. Press `Refuel +10%` (may need multiple presses) to raise fuel above 15%: low fuel indicator returns to “Fuel OK” and alert latch resets (banner remains until an implementation decides to clear—current behavior leaves last alert text visible).
5. Burn down again to <15%: a second alert is emitted (validated by test) proving re-arm works.
6. Toggle Dark/Light: entire window palette updates (no runtime restart required).

Developer notes:
- Alert emission & re-arm tested via updated `unit_low_fuel_alert` (ensures second alert after refuel) + feature test.
- Stability behavior covered by `unit_stability_index` (steady vs fluctuating thrust scenarios).
- Power invariants enforced in `unit_power_distribution` (sum = 100, reserve auto-balancing).
- Theming: `unit_theme_service` validates emission only on actual theme change (idempotent apply). Palette logic lives in `ThemeService::paletteFor`.
- Constants centralized in `core/Constants.h` – adjust `kFuelBurnPerSecAtFull` for faster demo cycles.
- UI now includes refuel + theme toggles; remaining future polish: auto-hide or timeout old alert banner; high-contrast theme (placeholder for accessibility).

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
- Current: Dark & Light palettes; color contrast chosen for legibility, not yet formally audited.
- Planned: Add high-contrast theme + automated contrast validation (WCAG AA) pre-commit.
- Theme switch hook placeholder for metrics (`theme.switch.ms`)—not yet instrumented.

### 7. Metrics Collection (Planned Hooks)
Instrumentation points (guarded by build flag):
- Tick duration
- Alert latency
- Replay drift frames
- Script execution time

### 8. Next After Quickstart
- (Done) Initial CMake scaffolding.
- (Done) SimulationCore physics (basic vertical model).
- (Done) Behavior-first failing tests converted to passing for physics & alert; low fuel alert implemented.
- (In Progress Soon) Add power redistribution, stability index, and script sandbox integration.
- (Planned) Theming system & accessibility checks.
- (Planned) Replay buffering and timeline scrub UI.

---
This quickstart evolves with implementation; any deviation requires updating plan Constitution Check evidence.

# GitHub Copilot Context

**Project**: qtui-test-demo  
**Feature Branch**: 001-space-cockpit-kitchensink  
**Last updated**: 2025-10-14

## Active Technologies
- C++23 + Qt 6.8 LTS (Core, Widgets, QJSEngine) (001-space-cockpit-kitchensink)
- GoogleTest + GoogleMock (unit/integration tests) (001-space-cockpit-kitchensink)
- Squish (GUI tests) (001-space-cockpit-kitchensink)

## Project Structure (Planned)
```
src/
  core/
  services/
  ui/
  adapters/
  theming/

tests/
  feature/
  integration/
  unit/
  gui/
```

## Build / Test Commands (Planned)
- Configure: cmake -S . -B build -DENABLE_SPACE_COCKPIT=ON
- Build: cmake --build build -j
- Unit tests: ctest --test-dir build (after adding GoogleTest)

## Behavior-First Notes
- Author failing feature-level tests before implementing SimulationCore & services.
- Synchronize tests via signals (telemetryTick, powerChanged, alertRaised, replayStateChanged, themeChanged).

## Recent Changes
- 001-space-cockpit-kitchensink: Added initial planning artifacts (plan, research, data model, contracts, quickstart).

(Generated manually; scripted update-agent-context execution unavailable in current environment.)

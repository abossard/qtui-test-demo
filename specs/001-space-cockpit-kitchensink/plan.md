## Implementation Plan: Space Cockpit Kitchensink

**Branch**: `001-space-cockpit-kitchensink` | **Date**: 2025-10-14 | **Spec**: `specs/001-space-cockpit-kitchensink/spec.md`
**Input**: Feature specification from `specs/001-space-cockpit-kitchensink/spec.md`

### Summary
Interactive Qt 6.8 LTS desktop “space cockpit” demo showcasing: deterministic simulation loop (10Hz physics, 2Hz sensors), power allocation constraints, alert/anomaly handling, autopilot scripting via QJSEngine sandbox, mission log + fixed 60s replay buffer, and full theming (dark/light/high-contrast) with WCAG AA compliance. Technical approach: isolate a pure simulation/core model layer (no Qt Widgets), emit strongly‑typed domain states via adapters/signals to thin UI panels; enforce behavior‑first TDD with feature‑level blackbox tests waiting on signals (no sleeps) and stable objectName anchors. Sandbox autopilot constrained by whitelisted API surface & watchdog. Persistent settings (thresholds + theme) via lightweight config serialization (QSettings).

### Technical Context
**Language/Version**: C++23 (fallback C++20 only if Qt 6.8 incompatibility documented)  
**Primary Dependencies**: Qt 6.8 LTS (Core, Widgets, QML optional later, QJSEngine), GoogleTest/GoogleMock (unit/integration), Squish (GUI tests)  
**Storage**: In-memory only (ring buffers + QSettings for persistence)  
**Testing**: GoogleTest/GoogleMock (unit/integration), Squish (GUI), custom feature-level harness using signals  
**Target Platform**: Desktop (macOS/Linux/Windows)  
**Project Type**: Single desktop application (demo)  
**Performance Goals**: 60 FPS UI responsiveness; SC targets (alert latency <300ms median, theme switch <300ms, replay drift ≤1 frame, hover stabilization ≤5s)  
**Constraints**: Deterministic tick cadence (10Hz physics, 2Hz sensors); WCAG AA contrast; flakiness <1% (rolling 50 runs); watchdog 250ms script CPU; replay fixed 60s  
**Scale/Scope**: Single executable, <15 core domain classes, <10 UI panels, ≤500 events in mission log buffer  

Previously Unresolved -> NOW RESOLVED (Phase 0 research below):  
- Unit test framework selection → GoogleTest/GoogleMock chosen (revised from initial Catch2 pre-code)  
- Alert ID / identity scheme → ALRT-<epochMillis>-<seq> monotonic uniqueness  
- Observability taxonomy → Log categories + metrics enumerated  
- Lifecycle state diagrams (autopilot, replay) → textual specification  

### Constitution Check (Pre-Design Gate)
1. Simplicity: Planned separation: Pure `SimulationCore`, thin adapters (`SimulationController`), no premature plugin system.  
2. Deep Module: Planned public surface (core): `SimulationCore`, `PowerManager`, `AlertBus`, `ReplayService`, `ScriptSandbox`; invariants will be documented in headers.  
3. Behavior-First TDD: Feature spec already contains Behavior Specification; initial failing tests to author before implementation: `test_flight_console_behavior.cpp`, `test_power_allocation_behavior.cpp`, `test_environment_alerts_behavior.cpp`.  
4. Deterministic Testability: Primary sync signals: `telemetryTick(TelemetryState)`, `powerChanged(PowerAllocation)`, `alertRaised(Alert)`, `replayStateChanged(bool)`, `themeChanged(ThemeMode)`.  
5. Example Integrity: Will guard with `ENABLE_SPACE_COCKPIT`; quickstart to document isolated build & tests.  
6. Blackbox Discipline: Tests assert emitted signals, UI text, and log events—no internal container size assertions.  
7. Flakiness Budget: Strategy: Wait on signals with timeout; zero arbitrary sleeps >100ms.  
8. TODOs: None outstanding after Phase 0 research resolution.  

Gate Status: PASS (all mandatory evidence identified; concrete artifacts to follow in Phase 0/1).

### Project Structure
```
src/
  core/                # Pure domain logic (physics, power allocation, replay buffer)
  services/            # ScriptSandbox, AlertBus, ReplayService, ThemeService
  ui/                  # Qt Widgets/QML panels (telemetry, power, sensors, alerts, console, replay, settings)
  adapters/            # Qt signal adapters bridging core <-> ui
  theming/             # Palette / QSS definitions

tests/
  feature/             # Behavior-level (blackbox) tests (headless or minimal harness)
  integration/         # Cross-service without UI
  unit/                # Pure logic (core algorithms)
  gui/                 # Squish scripts (external repo path or folder placeholder)
```

**Structure Decision**: Option 1 (single project) adopted; multi-front-end or mobile structures removed to avoid premature complexity.

### Phase 0: Research (Outputs in `research.md`)
Unknowns converted to decisions (see file). All NEEDS CLARIFICATION markers cleared before Phase 1.

### Phase 1: Design & Contracts
Artifacts produced: `data-model.md`, `contracts/openapi.yaml`, `quickstart.md`. Public invariants documented. Post-design Constitution re-check below.

### Phase 1 Post-Design Constitution Re-Check
| Principle / Gate | Evidence |
|------------------|----------|
| Simplicity | Data model keeps <15 entities; no plugin/event bus over-engineering |
| Deep Module | Each service exposes minimal methods (documented in data-model.md invariants section) |
| Behavior-First TDD | Behavior spec unchanged; test filenames reserved before code |
| Determinism | Signals list extended with `themeChanged`; replay & autopilot mutual exclusion explicit |
| Example Integrity | Quickstart includes isolated build/test commands |
| Blackbox Discipline | OpenAPI/feature contracts restrict UI tests to observable states |
| Flakiness Budget | Strategy documented (signal waits + bounded polling) |
| TODOs | None pending |

Status: PASS – no remediation required.

### Complexity Tracking
No violations requiring justification at this stage.

### Next Steps (Beyond /speckit.plan Scope)
- Author failing feature-level tests (Phase 2 via `/speckit.tasks`).
- Implement minimal simulation tick loop & signals.
- Implement power allocation & alert rules.
- Add ScriptSandbox with whitelisted API surface and watchdog.
- Add theming + persistence (QSettings) + contrast verification harness.

---
Generated by manual execution of plan workflow (script invocation unavailable in current environment). All decisions traceable to `research.md`.
# Implementation Plan: [FEATURE]

**Branch**: `[###-feature-name]` | **Date**: [DATE] | **Spec**: [link]
**Input**: Feature specification from `/specs/[###-feature-name]/spec.md`

**Note**: This template is filled in by the `/speckit.plan` command. See `.specify/templates/commands/plan.md` for the execution workflow.

## Summary

[Extract from feature spec: primary requirement + technical approach from research]

## Technical Context

<!--
  ACTION REQUIRED: Replace the content in this section with the technical details
  for the project. The structure here is presented in advisory capacity to guide
  the iteration process.
-->

**Language/Version**: [e.g., Python 3.11, Swift 5.9, Rust 1.75 or NEEDS CLARIFICATION]  
**Primary Dependencies**: [e.g., FastAPI, UIKit, LLVM or NEEDS CLARIFICATION]  
**Storage**: [if applicable, e.g., PostgreSQL, CoreData, files or N/A]  
**Testing**: [e.g., pytest, XCTest, cargo test or NEEDS CLARIFICATION]  
**Target Platform**: [e.g., Linux server, iOS 15+, WASM or NEEDS CLARIFICATION]
**Project Type**: [single/web/mobile - determines source structure]  
**Performance Goals**: [domain-specific, e.g., 1000 req/s, 10k lines/sec, 60 fps or NEEDS CLARIFICATION]  
**Constraints**: [domain-specific, e.g., <200ms p95, <100MB memory, offline-capable or NEEDS CLARIFICATION]  
**Scale/Scope**: [domain-specific, e.g., 10k users, 1M LOC, 50 screens or NEEDS CLARIFICATION]

## Constitution Check

*GATE: Must pass before Phase 0 research. Re-check after Phase 1 design.*

The following MUST be evidenced (link to lines / test diffs):

1. Simplicity (Principle I): No unnecessary state, duplication, or premature abstraction introduced.
2. Deep Module (Principle II): Public API surface minimal; invariants documented.
3. Behavior-First TDD (Principle III): Behavior Specification (inputs, triggers, observable outcomes, invariants, edge cases) authored; failing feature-level blackbox test present before implementation.
4. Deterministic Testability (Principle IV): Explicit synchronization strategy (signals/properties) – no arbitrary sleeps.
5. Example Integrity (Principle V): New example build + test commands documented; prior examples unaffected.
6. Blackbox Discipline: Tests avoid over-specifying internals (no class-count, private method, or call-sequence assertions without justification).
7. Flakiness Budget: Any touched GUI test has historical flakiness <1% or remediation plan attached.
8. TODOs: Any remaining TODO(...) items justified with issue link.

## Project Structure

### Documentation (this feature)

```
specs/[###-feature]/
├── plan.md              # This file (/speckit.plan command output)
├── research.md          # Phase 0 output (/speckit.plan command)
├── data-model.md        # Phase 1 output (/speckit.plan command)
├── quickstart.md        # Phase 1 output (/speckit.plan command)
├── contracts/           # Phase 1 output (/speckit.plan command)
└── tasks.md             # Phase 2 output (/speckit.tasks command - NOT created by /speckit.plan)
```

### Source Code (repository root)
<!--
  ACTION REQUIRED: Replace the placeholder tree below with the concrete layout
  for this feature. Delete unused options and expand the chosen structure with
  real paths (e.g., apps/admin, packages/something). The delivered plan must
  not include Option labels.
-->

```
# [REMOVE IF UNUSED] Option 1: Single project (DEFAULT)
src/
├── models/
├── services/
├── cli/
└── lib/

tests/
├── contract/
├── integration/
└── unit/

# [REMOVE IF UNUSED] Option 2: Web application (when "frontend" + "backend" detected)
backend/
├── src/
│   ├── models/
│   ├── services/
│   └── api/
└── tests/

frontend/
├── src/
│   ├── components/
│   ├── pages/
│   └── services/
└── tests/

# [REMOVE IF UNUSED] Option 3: Mobile + API (when "iOS/Android" detected)
api/
└── [same as backend above]

ios/ or android/
└── [platform-specific structure: feature modules, UI flows, platform tests]
```

**Structure Decision**: [Document the selected structure and reference the real
directories captured above]

## Complexity Tracking

*Fill ONLY if Constitution Check has violations that must be justified*

| Violation | Why Needed | Simpler Alternative Rejected Because |
|-----------|------------|-------------------------------------|
| [e.g., 4th project] | [current need] | [why 3 projects insufficient] |
| [e.g., Repository pattern] | [specific problem] | [why direct DB access insufficient] |

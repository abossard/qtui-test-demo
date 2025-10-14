# Feature Specification: Space Cockpit Kitchensink

**Feature Branch**: `[001-space-cockpit-kitchensink]`  
**Created**: 2025-10-14  
**Status**: Draft  
**Input**: User description: "QT UI Kitchensink Application about space control; mimic lunar lander / alien themed engine with emerging behavior; dark futuristic spaceship cockpit; lots of displays, sliders, buttons, text outputs and other Qt UI elements."

## User Scenarios & Testing *(mandatory)*

### User Story 1 - Core Flight Console (Priority: P1)
Pilot can monitor and adjust primary descent/ascent parameters (thrust, attitude, fuel flow) via unified cockpit pane to achieve stable hover or controlled landing.

**Why this priority**: Foundational interactive loop; enables all other subsystems (navigation, power, environment) to have observable context.

**Independent Test**: Simulate thrust and attitude adjustments; verify derived telemetry (vertical velocity trend, fuel consumption) and stability indicator reach target range.

**Acceptance Scenarios**:
1. Given craft idle on pad, When pilot increases main thruster slider above 30%, Then vertical velocity indicator shows positive climb within 2s.
2. Given a climb state, When pilot reduces thrust to hover band (auto‑calculated), Then stability indicator turns green and altitude drift remains < 0.5m over 5s simulation.
3. Given remaining fuel < 10%, When sustained thrust > 70% for 3s, Then LOW FUEL warning panel flashes amber.

---
### User Story 2 - Power Distribution Grid (Priority: P2)
Engineer can reroute energy between subsystems (Life Support, Navigation, Weapons, Research) with sliders / matrix, observing immediate effect on subsystem efficiency gauges.

**Why this priority**: Demonstrates dynamic model/view updates and constraint propagation (sums to 100%).

**Independent Test**: Allocate power percentages; verify total = 100%, gauges animate to new efficiency values, and over-allocation prevented.

**Acceptance Scenarios**:
1. Given total power budget 100%, When user sets Life Support 40%, Navigation 40%, Others auto adjust to remain ≤ 100%, Then system enforces cap and displays remaining budget.
2. When user drags a slider causing budget exceed, Then slider snaps back and warning toast appears.
3. When power to Navigation < 15%, Then course deviation noise increases (simulated jitter metric exposed) within 3s.

---
### User Story 3 - Environmental & Anomaly Monitor (Priority: P3)
Operations officer views sensor panels (radiation, hull temp, external pressure) each with trend sparkline; anomalies trigger alert stack.

**Why this priority**: Adds time‑series visualization and asynchronous signal handling.

**Independent Test**: Inject sensor events; verify rolling averages update, anomaly thresholds raise distinct alerts with proper priority ordering.

**Acceptance Scenarios**:
1. Given radiation baseline safe, When spike > threshold X for >2 consecutive samples, Then RADIATION ALERT (red) appears and audible cue event emitted.
2. Given hull temperature rising, When delta > 10C in under 5s, Then TEMPERATURE WARNING (amber) logged and escalation timer starts.
3. When multiple alerts active, Then highest severity pinned top.

---
### User Story 4 - Autopilot Script Console (Priority: P3)
Pilot can load/edit constrained JavaScript (Qt QJSEngine sandbox) to engage auto-hover; console outputs status lines & error diagnostics.

**Why this priority**: Demonstrates text I/O, validation, and controlled execution feedback loop.

**Independent Test**: Provide script, inject simulation ticks, confirm autopilot adjusts thrust toward hover band and logs state transitions.

**Acceptance Scenarios**:
1. Given valid script, When user presses Activate, Then autopilot status = ACTIVE and thrust commands issued within 1 tick.
2. Given syntax error, When Activate pressed, Then no state change and error line annotated in editor.
3. When user deactivates autopilot, Then manual controls regain authority immediately (next tick acknowledges).

---
### User Story 5 - Mission Log & Replay (Priority: P4)
Crew can view chronological log (events, warnings, commands) and replay last N seconds of telemetry.

**Why this priority**: Introduces persistence-like buffer management and playback UI controls.

**Independent Test**: Record events during simulated maneuvers; initiate replay; verify timeline scrub reproduces telemetry sequence deterministically.

**Acceptance Scenarios**:
1. Given log buffer size 500 events, When events exceed size, Then oldest events roll off without performance degradation.
2. When replay started, Then live updates pause (indicated) and replay timeline controls enable.
3. When replay ends or aborted, Then live stream resumes within 1s and continuity marker inserted in log.

---
### Edge Cases
- Sensor returns NaN or missing sample -> show placeholder and exclude from averages.
- Power sliders sum < 100% -> distribute remainder to "Reserve" bucket automatically.
- Autopilot script infinite loop attempt -> watchdog kills after 250ms wall time and logs FATAL SCRIPT TIMEOUT.
- Fuel depletes mid‑burn -> thrust auto clamps to 0 and triggers HARD SHUTDOWN event.
- Alert storm (>=10 alerts in <5s) -> collapse into summarized STORM MODE indicator with expandable list.
- Replay while autopilot active -> autopilot temporarily suspended; warning logged.
- Negative altitude due to physics drift -> clamp to 0 and log correction.

## Requirements *(mandatory)*

### Functional Requirements
- **FR-001**: System MUST simulate thrust → vertical velocity → altitude chain at 10Hz update.
- **FR-002**: UI MUST expose main thruster slider (0–100%) and show derived hover band range.
- **FR-003**: System MUST compute fuel consumption proportional to thrust and display remaining fuel percentage.
- **FR-004**: System MUST raise LOW FUEL warning below 10% remaining.
- **FR-005**: Power distribution UI MUST enforce sum == 100% (with auto adjust or snap-back on overflow).
- **FR-006**: System MUST provide subsystem efficiency gauges reflecting current allocated power.
- **FR-007**: Environmental monitor MUST process sensor samples (radiation, hull temp, pressure) at 2Hz.
- **FR-008**: System MUST generate alerts when sensor thresholds crossed for configured durations.
- **FR-009**: Autopilot console MUST validate script syntax prior to activation.
- **FR-010**: Valid autopilot MUST issue thrust adjustments toward hover delta < tolerance (configurable) within 5 ticks.
- **FR-011**: System MUST allow deactivation of autopilot restoring manual control immediately.
- **FR-012**: Mission log MUST store last 500 events with O(1) append and discard.
- **FR-013**: Replay MUST render recorded telemetry deterministically (time‑ordered) while pausing live feed.
- **FR-014**: System MUST resume live telemetry within 1s after replay end.
- **FR-015**: Alert system MUST prioritize severity (Critical > Warning > Info) in stack ordering.
- **FR-016**: UI MUST support dark theme (cockpit style) with high‑contrast alert colors.
- **FR-017**: All interactive controls MUST expose stable `objectName` for GUI tests.
- **FR-018**: Fuel depletion MUST hard-stop thrust and log HARD SHUTDOWN.
- **FR-019**: Watchdog MUST terminate scripts exceeding 250ms CPU and log timeout.
- **FR-020**: Replay activation MUST disable autopilot if active and log state transition.
- **FR-021**: Flakiness guard: No feature-level test may rely on fixed sleeps >100ms without signal/property wait fallback.
- **FR-022**: Behavior Specification MUST exist (this doc) before implementing feature-level tests.

*Unclear / Clarification Needed*
<!-- (FR-024 clarified below) -->

### Clarified / Added Functional Requirements
- **FR-023**: System MUST use Qt's embedded QJSEngine (restricted JavaScript subset) for scripting; only whitelisted API objects exposed (autopilot control, read‑only telemetry, logging).
- **FR-024**: System MUST cap replay buffer at a fixed 60s duration (non-configurable) discarding oldest telemetry beyond cap.
- **FR-025**: Script sandbox MUST enforce: 250ms CPU watchdog, no dynamic file/network access, memory usage heuristic (object count or allocated bytes) preventing unbounded growth (fail with SCRIPT_RESOURCE_LIMIT).
- **FR-026**: Autopilot scripts MUST run on simulation tick boundary; partial execution failures revert to last safe thrust command.
- **FR-027**: UI text, critical indicators, and alert labels MUST meet WCAG AA contrast ratios (≥4.5:1 normal text, ≥3:1 large text); failure to meet contrast blocks merge.
- **FR-028**: Sensor anomaly thresholds (radiation spike, temperature delta, pressure variance) MUST be user-configurable via a persistent Settings panel (stored in local config, reloaded at startup) with validation (min/max logical bounds) and immediate application upon save.
 - **FR-029**: Theming system MUST support three selectable modes: Dark (default), Light, High-Contrast; switching applies instantly (palette/QSS reload) and persists across sessions.

### Key Entities
- **TelemetryState**: altitude, velocity, thrustPercent, fuelRemaining, stabilityIndex, timestamp.
- **PowerAllocation**: percentages per subsystem + reserve remainder.
- **SensorSample**: type (radiation/temp/pressure), value, timestamp.
- **Alert**: id, severity, message, activeSince, acknowledged.
- **ScriptProgram**: source, compiled form, status (VALID/ERROR/RUNNING/TIMEOUT).
- **LogEvent**: timestamp, category, level, message, correlationId (optional).

## Success Criteria *(mandatory)*

### Measurable Outcomes
- **SC-001**: Hover stabilization achieved (stabilityIndex within target band) within ≤ 5s for initial ascent in 95% of simulation runs.
- **SC-002**: Power reallocation UI responds (visual update) within < 150ms of slider release.
- **SC-003**: Alert latency from threshold breach to UI display < 300ms median.
- **SC-004**: Replay drift vs original telemetry timeline ≤ 1 frame (100ms at 10Hz) across 60s segment.
- **SC-005**: Autopilot brings thrust error < 5% of target within 10 ticks in 90% of runs.
- **SC-006**: GUI feature-level tests pass with flakiness < 1% over rolling 50 executions.
- **SC-007**: 100% of audited UI text & alert indicators meet WCAG AA contrast (spot-check automation + manual verification per release).
 - **SC-008**: Theme mode switch completes (visible repaint) in < 300ms on reference dev hardware.

### Behavior Specification (Feature-Level, Pre-Test)
Purpose / Value: Provide a rich, dark-themed Qt cockpit demonstrating multi-panel synchronization, signal-driven updates, time-series visualization, and behavior-first TDD patterns.
Inputs / Triggers:
- User interactions: thruster slider, power sliders, autopilot activate/deactivate, replay controls.
- Timed simulation ticks (10Hz physics, 2Hz sensors).
- Script events (autopilot adjustments).
Observable Outputs:
- Altitude / velocity / fuel displays update.
- Stability indicator color transitions (red->amber->green).
- Power gauges animate to new allocation.
- Sensor sparklines progress; alerts populate stack.
- Mission log appends structured events.
- Replay overlays historical telemetry while live paused.
Invariants:
- Power allocation sum == 100%.
- Fuel never < 0 (clamped) and thrust == 0 when fuel == 0.
- Only one autopilot active instance (singleton status) at a time.
- Replay and live modes mutually exclusive (indicator toggles).
Edge Cases:
- Missing sensor -> placeholder state & no alert.
- Script timeout -> autopilot aborted safely.
- Rapid slider jitter -> final debounced value applied once per 100ms.
Non-Goals:
- Realistic orbital mechanics.
- Networking / multiplayer.
- Persistent storage beyond in-memory buffers.
Open Questions:
- Color palette accessibility adjustments.

### Constitution Alignment (Mandatory References)
- Simplicity (I): Modular separation: simulation core (pure), UI panels (thin), controllers (glue); avoid premature micro-controllers.
- Deep Module (II): Public API: `SimulationController`, `PowerManager`, `AlertBus`, `ReplayService`; invariants documented in headers.
- Behavior-First TDD (III): This spec is the Behavior Specification; initial feature-level tests planned: `test_flight_console_behavior.cpp`, `test_power_allocation_behavior.cpp`, `test_environment_alerts_behavior.cpp`.
- Deterministic Testability (IV): Signals: `telemetryTick(TelemetryState)`, `powerChanged(PowerAllocation)`, `alertRaised(Alert)`, `replayStateChanged(bool)`. No arbitrary sleeps; tests wait on signals or poll stabilityIndex with timeout.
- Example Integrity (V): Feature build isolated behind `ENABLE_SPACE_COCKPIT` compile flag; test commands runnable independently.
- Blackbox Discipline: Tests assert observable metrics (display text, signal payloads, log events) only—no internal container shapes.

Any deviation MUST include justification + issue link.

- **SC-001**: (Already listed above)
- **SC-002**: (Already listed above)
- **SC-003**: (Already listed above)
- **SC-004**: (Already listed above)
- **SC-005**: (Already listed above)
- **SC-006**: (Already listed above)

## Clarifications

### Session 2025-10-14
- Q: What scripting engine approach should be used (Lua, custom DSL, QJSEngine, defer)? → A: Use QJSEngine (restricted JS subset).
- Q: What replay duration strategy (fixed, configurable range, unlimited, adaptive)? → A: Fixed 60s cap.
- Q: What accessibility contrast requirement applies (none, AA, AAA, partial)? → A: WCAG AA contrast.
- Q: How are sensor anomaly thresholds configured (hard-coded, config file, debug panel, persistent settings UI)? → A: Persistent Settings panel (user-accessible, saved & reloaded).
 - Q: What theme variant strategy (single dark, dark+contrast, full set, pluggable)? → A: Full set (dark, light, high-contrast).


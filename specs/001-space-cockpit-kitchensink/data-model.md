## Data Model & Domain Contracts

Feature: Space Cockpit Kitchensink  
Date: 2025-10-14

### Overview
Core domain kept intentionally small to preserve deep modules (Constitution Principle II). Pure logic resides in `src/core/`; services coordinate side effects & UI signal emission.

### Entities
| Entity | Fields | Invariants / Rules | Relationships |
|--------|--------|--------------------|---------------|
| TelemetryState | altitude (m), verticalVelocity (m/s), thrustPercent (0-100), fuelRemaining (0-100), stabilityIndex (0-1), timestamp (ms) | fuelRemaining >= 0, thrustPercent clamped, stabilityIndex derived (0 unstable → 1 stable) | Produced by SimulationCore tick; consumed by UI & ReplayService |
| PowerAllocation | lifeSupport%, navigation%, weapons%, research%, reserve% | Sum == 100; each >=0; adjustments redistribute reserve | Inputs affect subsystem efficiency gauges |
| SensorSample | type (enum: RADIATION, TEMP, PRESSURE), value (double), timestamp | NaN → not aggregated; negative pressure invalid -> discard | Aggregated into rolling stats; anomaly detection |
| Alert | id (string), severity (enum: CRITICAL/WARNING/INFO), message, activeSince (ms), acknowledged(bool) | id unique; severity ordering enforced; activeSince immutable | Emitted by AlertBus; displayed in stack |
| ScriptProgram | source (string), status (enum: VALID/ERROR/RUNNING/TIMEOUT), compiledHandle (opaque), lastError (optional) | status transitions follow lifecycle; compiledHandle non-null iff VALID/RUNNING | Owned by ScriptSandbox |
| LogEvent | timestamp, category(enum), level(enum), message, correlationId(optional) | Timestamp monotonic (non-decreasing) | Appended to MissionLog ring buffer |
| MissionLog | buffer[LogEvent] (max 500), nextIndex | Buffer length <= 500 O(1) operations | Receives events from services |
| ReplayBuffer | samples[TelemetryState] (cover ≤60s), startIndex, length | length * tickInterval <= 60s; overwrites oldest | Fed by SimulationCore, consumed by ReplayService |
| ReplayService | mode(enum: LIVE, REPLAYING), cursorIndex, startTimestamp | mode switch emits replayStateChanged; cursorIndex valid in REPLAYING | Reads from ReplayBuffer |
| ScriptSandbox | whitelisted API objects, watchdog ms, activeProgram | Only 1 activeProgram; watchdog enforced per tick | Adjusts SimulationCore via controller adapter |
| SimulationCore | physicsState (fuel, altitude...), config constants | Tick deterministic for same input stream; no Qt types inside | Emits TelemetryState each tick |
| ThresholdConfig | radiationSpikeThreshold, radiationConsecSamples, tempDelta, tempWindowMs, pressureVariance | Validation on save (positive, logical bounds) | Persisted via QSettings; used by anomaly detection |
| ThemeService | currentTheme(enum: DARK/LIGHT/HIGH_CONTRAST), paletteRefs | Theme switch <300ms; persisted | Notifies UI via themeChanged |
| MetricsRecorder | counters, timers | Non-blocking; minimal overhead (<1ms per tick) | Optional injection into services |

### State Machines
Autopilot (ScriptSandbox):  
INACTIVE → ACTIVATING → ACTIVE → (TIMEOUT | ERROR) → INACTIVE  
Replay (ReplayService):  
LIVE ↔ REPLAYING (mutual exclusion: entering REPLAYING forces Autopilot → INACTIVE)

### Validation Rules
- PowerAllocation sum recalculated after any slider change; excess redistributed to reserve or snap-back.  
- ThresholdConfig rejection if any threshold outside mission-defined safe ranges (document in UI).  
- ReplayBuffer insertion wraps with modulo; on overflow oldest overwritten silently.  
- Alert dedup (same message & severity inside 1s window) collapses to single alert (extension logic optional later).  
- Script watchdog: any tick execution >250ms triggers TIMEOUT + Alert(severity=CRITICAL).  

### Public Service Interfaces (Draft)
```cpp
struct TelemetryState { double altitude; double verticalVelocity; double thrustPercent; double fuelRemaining; double stabilityIndex; qint64 timestamp; };
struct PowerAllocation { int lifeSupport; int navigation; int weapons; int research; int reserve; };

class SimulationController {
public:
  void setThrustPercent(double pct);              // clamps & queues for next tick
  TelemetryState current() const;                 // last emitted snapshot
signals:
  void telemetryTick(const TelemetryState&);
};

class PowerManager {
public:
  void setAllocation(const PowerAllocation&);     // enforces sum==100
  PowerAllocation current() const;
signals:
  void powerChanged(const PowerAllocation&);
};

class AlertBus {
public:
  void raise(Alert alert);                        // assigns id if missing
signals:
  void alertRaised(const Alert&);
};

class ReplayService {
public:
  void startReplay(qint64 startTs);               // transitions to REPLAYING
  void stopReplay();                              // back to LIVE
signals:
  void replayStateChanged(bool replaying);
};

class ScriptSandbox {
public:
  bool load(const QString& source);               // compile -> status
  bool activate();                                // transitions if VALID
  void deactivate();
signals:
  void scriptStatusChanged(int newStatus);
};
```

### Invariants Summary
| Interface | Invariant |
|-----------|-----------|
| SimulationController | Emission cadence exactly 100ms unless paused for replay |
| PowerManager | Allocation sum == 100 always observable |
| AlertBus | Alert severity ordering stable by arrival + severity |
| ReplayService | No telemetryTick passes through in REPLAYING mode (replaced by replay frames) |
| ScriptSandbox | At most one ACTIVE program; execution bounded by watchdog |
| ThemeService | Palette objects immutable post-activation (copy-on-switch) |

### Rationale (Deep Modules)
- Core logic avoids Qt dependencies → testable pure functions.  
- Services provide minimal verbs; complexity hidden (e.g., redistribution algorithm inside PowerManager).  
- Replay buffer ring logic encapsulated; external code only sees start/stop + emitted frames.  

### Open Questions / Future (Not Blocking)
- Alert collapsing strategy beyond simple dedup window.  
- Additional metrics (GC-like tracking of script allocations).  

---
This file supports Phase 1 design; updates require re-running Constitution Check if invariants expand public surface.

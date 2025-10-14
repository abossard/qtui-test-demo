## Research: Space Cockpit Kitchensink (Phase 0)

Date: 2025-10-14  
Branch: `001-space-cockpit-kitchensink`

### Decision 1: Unit Test Framework (Revised)
**Decision**: Adopt GoogleTest + GoogleMock for unit & integration tests (supersedes prior Catch2 choice).  
**Rationale**: Broad industry adoption and ecosystem maturity (GoogleTest user’s guide & primer emphasize portability across OSes and compilers) [R1][R2]; built‑in mocking (merged GoogleMock) enables interface isolation patterns without extra dependency [R2]; rich assertions, parameterized & type‑parameterized tests [R2]; portability (works with/without exceptions & RTTI) [R3]; widely used in notable projects (Chromium, LLVM, Protocol Buffers, OpenCV) providing ecosystem familiarity [R2]; aligns with educational goal to demonstrate mainstream tooling.  
**Alternatives Considered**:  
- Catch2: Header‑only convenience and Sections pattern, but lacks native mocking—would require additional framework or manual fakes; switching to GoogleTest prioritizes demonstrating mock-based interaction tests later.  
- doctest: Very lightweight and fast, but reduced matcher/mocking ecosystem relative to GoogleTest for advanced scenarios.  
- Boost.Test: Heavier dependency surface; unnecessary for current scope.  
**Change Note**: Early exploratory selection of Catch2 replaced before implementation began; no refactor cost incurred.  

### Decision 2: Alert Identity Scheme
**Decision**: `ALRT-<epochMillis>-<seq>` (epoch ms capture time + per-process increment).  
**Rationale**: Deterministic ordering, human-inspectable, stable for tests (can regex match prefix + numeric groups).  
**Alternatives Considered**:  
- Simple incremental global counter: Non-stable across replay/time resets.  
- UUID/GUID: Overkill verbosity, harder to visually parse.  
- Hash(content): Collisions risk under similar messages, nondeterministic ordering in rare edge cases.

### Decision 3: Observability Taxonomy
**Decision**: Structured logging categories + minimal metrics set.  
**Categories**: `SIMULATION`, `POWER`, `SENSOR`, `ALERT`, `AUTOPILOT`, `REPLAY`, `THEME`, `UI`.  
**Metrics (sample instrumentation keys)**:  
- `tick.duration.ms` (per 10Hz physics iteration)  
- `script.exec.ms` (autopilot run)  
- `alert.latency.ms` (threshold breach → alertRaised signal)  
- `replay.drift.frames` (original vs replay timeline delta)  
- `theme.switch.ms` (themeChanged signal to repaint completion)  
**Rationale**: Focus on metrics mapped to Success Criteria & flakiness root-cause.  
**Alternatives Considered**:  
- Expansive tracing (span graph) – excessive for educational scope.  
- No metrics – reduces learning value & hampers SC validation.  

### Decision 4: Autopilot Lifecycle States
**Decision**: `INACTIVE → ACTIVATING → ACTIVE → (TIMEOUT|ERROR) → INACTIVE`.  
**Triggers**:  
- User Activate: `INACTIVE -> ACTIVATING` (syntax validation, compile).  
- Successful compile: `ACTIVATING -> ACTIVE`.  
- Watchdog breach / runtime error: `ACTIVE -> TIMEOUT|ERROR -> INACTIVE` (emit alert + log).  
- User Deactivate: `ACTIVE -> INACTIVE`.  
**Mutual Exclusions**: Engaging Replay forces `ACTIVE -> INACTIVE` (autopilot suspended).  
**Rationale**: Explicit transient ACTIVATING isolates validation from active control.  
**Alternatives Considered**: Direct `INACTIVE -> ACTIVE` (less clarity; conflates validation failures with runtime errors).

### Decision 5: Replay Lifecycle States
**Decision**: `LIVE ↔ REPLAYING`.  
**Transitions**: Start replay (pause live signals, feed buffer), end/abort (resume live, insert continuity marker).  
**Rationale**: Two-state machine sufficient; complexity (e.g., BUFFERING) unnecessary with fixed 60s ring.  
**Alternatives Considered**: Multi-stage (BUFFERING, SEEKING) – premature.

### Decision 6: Feature-Level Test Synchronization Strategy
**Decision**: Feature tests wait on domain signals with bounded timeouts (e.g., 2s) and optionally poll invariants at 10–20ms intervals; zero fixed sleeps >100ms.  
**Rationale**: Determinism & flakiness reduction aligned with Constitution Principle IV.  
**Alternatives Considered**: Arbitrary sleeps – risk nondeterministic pass/fail; event loop spin waits – less explicit.

### Decision 7: Theme & Contrast Verification
**Decision**: Programmatic contrast audit helper computing relative luminance for sampled palette roles; fail build if any alert/critical text < required ratio.  
**Rationale**: Enforces SC-007 automatically; educational example of accessibility gating. WCAG AA requires ≥4.5:1 contrast for normal text and ≥3:1 for large text; non‑text UI components require ≥3:1 (WCAG 2.1) [R4][R5].  
**Alternatives Considered**: Manual review only – risk drift; external tooling – adds dependency overhead.

### Consolidated Outcomes
All prior NEEDS CLARIFICATION markers resolved; framework choice updated to GoogleTest before any code dependency committed—no migration overhead.

### External References
| Ref | Topic | Source |
|-----|-------|--------|
| R1 | GoogleTest Primer (independent, repeatable, portability goals) | http://google.github.io/googletest/primer.html |
| R2 | GoogleTest README – notable projects using, merged mocking framework | https://github.com/google/googletest |
| R3 | FAQ: Portability (no exceptions/RTTI required; multi‑platform) | https://chromium.googlesource.com/external/github.com/pwnall/googletest/+/refs/tags/release-1.8.0/googletest/docs/FAQ.md |
| R4 | WCAG contrast ratios (4.5:1 normal, 3:1 large; 3:1 UI components) | https://webaim.org/resources/contrastchecker/ |
| R5 | WCAG 2.0/2.1 contrast success criteria details | https://webaim.org/articles/contrast/ |
| R6 | Qt QJSEngine class reference (exposing objects, evaluate, sandbox limits semantic only) | https://doc.qt.io/qt-6/qjsengine.html |
| R7 | Qt scripting integration & security caveats (QJSEngine globalObject exposure) | https://qt.developpez.com/doc/6.0/qtjavascript/ |
| R8 | Qt QSettings persistence of application settings across sessions | https://doc.qt.io/qt-6/qsettings.html |

### Reference Application Notes
- Scripting (Decisions 4 & 6): Security considerations (semantic sandbox only) informed by Qt docs [R6][R7]; justifies restricted whitelist + watchdog.
- Persistence (ThresholdConfig & Theme) leverages QSettings portability [R8].
- Contrast enforcement thresholds directly sourced from WCAG resources [R4][R5].
- Test framework selection grounded in official documentation emphasizing portability & ecosystem (R1–R3).

### Traceability Matrix (Decision ↔ Requirements / SC)
| Decision | Linked FR / SC | Impact |
|----------|----------------|--------|
| Unit Test Framework | FR-021 (flakiness discipline) | Enables expressive BDD sections |
| Alert Identity | FR-015 (ordering) | Deterministic severity + arrival ordering |
| Observability | SC-001..006 | Metrics feed validation & regression detection |
| Autopilot Lifecycle | FR-009..011, FR-026 | Clear error/timeouts handling |
| Replay Lifecycle | FR-013..014, FR-020 | Mutual exclusion with autopilot |
| Sync Strategy | FR-021 | Eliminates arbitrary sleeps |
| Theme Contrast Audit | FR-027, SC-007 | Automated accessibility enforcement |

### Risks & Mitigations
| Risk | Likelihood | Impact | Mitigation |
|------|------------|--------|------------|
| GUI test flakiness on CI hardware variance | Medium | Medium | Signal waits + metrics logging + <1% budget guard |
| Script sandbox exploit (exposed unwanted Qt APIs) | Low | Medium | Explicit whitelist + negative tests |
| Theme performance regression | Low | Low | Measure `theme.switch.ms` & budget <300ms |
| Replay buffer memory creep | Low | Low | Fixed 60s cap + ring buffer tests |

---
All decisions ratified 2025-10-14. Proceed to Phase 1.

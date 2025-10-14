<!--
Sync Impact Report
Version change: 1.0.1 -> 1.1.0 (MINOR: Expanded Principle III to behavior-focused, feature-level blackbox TDD; added Behavior Specification requirements)
Modified principles:
  Principle III: "Test‑First Multi‑Layer Verification" -> "Behavior‑First Test‑Driven Development (Feature‑Level, Non‑Negotiable)" (semantic expansion toward unit-of-behavior focus)
Added sections / content:
  Added explicit Behavior Specification workflow under Delivery Workflow & Quality Gates
  Added references to docs/testing_strategy.md
Removed sections: None
Templates requiring updates: plan-template.md ✅, spec-template.md ✅, tasks-template.md ✅ (behavior spec hooks added)
Outstanding TODOs: None
-->

# Qt UI Test Kitchen Constitution

## Core Principles

### I. Simplicity & Value Flow
The codebase MUST minimize accidental complexity: prefer pure functions for domain logic, isolate side effects at clear module boundaries, remove duplication eagerly, and reject features lacking a concrete learning or demonstrative value for Qt UI, testing, or tooling. Each addition MUST answer: "What new Qt pattern, testing strategy, or Copilot usage does this teach?" If unclear, it is not merged.

### II. Deep Modules & Information Hiding
Modules MUST be deep: large internal payoff / small public surface. Shallow abstractions (wrappers that merely rename Qt API calls) are rejected unless they introduce an invariant or cross‑cutting concern (e.g., synchronization helper). Internal details (object lookup strategies, helper macros, test hooks) stay private; only stable interfaces and object names required for tests are exposed. Any new public API MUST state its core invariant in code comments or docs.

### III. Behavior‑First Test‑Driven Development (Feature‑Level, NON‑NEGOTIABLE)
The primary "unit" is a *unit of behavior* (a user‑visible feature slice or externally observable contract), not an individual class.
Workflow per feature change:
1. Behavior Specification: A short, implementation‑agnostic description (inputs, triggers, observable outcomes, invariants, edge cases). Stored adjacent to feature plan or in `specs/<feature>/behavior.md`.
2. Blackbox Test First: Write a failing feature‑level test (could span multiple classes) asserting only externally observable effects (state, emitted signals, UI text, model deltas). Avoid asserting private structure.
3. Supporting Tests: Add unit or integration tests only when they provide incremental confidence or pin tricky invariants—not by default for every class.
4. Implement minimally to satisfy behavior. Refactor after green.
5. Guard against regression by keeping behavior spec + test in sync.
New UI elements MUST expose stable `objectName` values if referenced by tests.
Flakiness threshold: >1% over last 50 executions => mandatory stabilization within 48h.
Prohibited: Tests that over‑specify internal call sequences, class counts, or transient implementation details.

### IV. Deterministic Testability & Observability
Design MUST support deterministic synchronization: prefer signals, explicit state properties, and condition waits over arbitrary sleeps. Diagnostic hooks guarded by `#ifdef SQUISH_TEST_HOOKS` MAY expose internal metrics; they MUST not alter release behavior. Each module emitting asynchronous behavior MUST provide at least one of: (a) signal to await, (b) pollable property, (c) progress/event model. Structured logging (log level + category) SHOULD accompany complex flows; logs MUST NOT replace assertions.

### V. Explorability & Reproducible Examples
The repository acts as a "kitchen sink" of Qt + testing patterns. Every example MUST be: (a) buildable in isolation (document required subset), (b) documented with purpose and principle mapping, (c) accompanied by at least one automated test (unit or GUI) demonstrating usage. Copilot‑assisted code MUST undergo human review for correctness, clarity, and alignment with Principles I–IV before merge.

## Additional Constraints & Standards

Technology Stack: C++23 (fallback C++20 only if specific Qt 6.8 incompatibility documented), Qt 6.8 LTS, Squish (GUI tests; scripts in Python/JavaScript recommended), optional conditional compilation for test hooks (`SQUISH_TEST_HOOKS`). Code style & formatting: see `docs/code_style.md` (clang-format enforced in CI).
Naming: `objectName` MUST be stable, descriptive (`OrderDialog.addButton`, `ordersTable_vScroll`). Avoid volatile data in names.
Test Layers: Unit tests cover pure logic; integration tests cover model + controller interactions; GUI tests validate end‑to‑end user flows and synchronization robustness.
Performance Baseline: Simple UI actions (< 200ms) and initial application startup (< 3s cold start) SHOULD be maintained; exceeding requires justification and optimization issue.
Flakiness Budget: Any GUI test exceeding 2 transient failures in CI per rolling 50 run window triggers mandatory stabilization task.
Documentation: Every new example adds a short rationale paragraph linking it to at least one principle.
Security & Safety: No network calls or filesystem writes in GUI tests unless explicitly part of the example’s teaching goal.
Generated Code: AI/Copilot generated segments MUST include a review commit noting adjustments (if any) for correctness or style.

## Delivery Workflow & Quality Gates

Constitution Check (applied in plans/specs):
1. Simplicity: Evidence of avoiding unnecessary state / duplication.
2. Deep Module: Public API surface reviewed for minimalism & documented invariants.
3. Behavior‑First TDD: Behavior Specification present (inputs, triggers, outcomes), failing feature‑level test authored first.
4. Determinism: Synchronization strategy enumerated (signals/properties/waits) – no arbitrary sleeps.
5. Example Integrity: Added example build + test command documented; does not break prior examples.
6. Blackbox Discipline: Tests avoid asserting internal implementation details.

Pull Request Requirements:
- Checklist confirming all five principles evaluated.
- CI: All unit, integration, GUI tests pass; no quarantined test regressions.
- Code review explicitly signs off invariants & test meaningfulness.

Release / Tagging:
- Each significant cluster of new examples or patterns MAY trigger MINOR version if public guidance changes.
- Breaking governance or principle removal triggers MAJOR.

## Governance

Authority: This constitution supersedes ad hoc conventions; conflicting prior patterns are deprecated upon amendment.
Amendments: Proposed via PR titled "constitution: amendment <summary>" including: (a) redlines, (b) rationale, (c) version bump justification (MAJOR/MINOR/PATCH), (d) migration steps if behavior/gates change.
Versioning Policy: Semantic: MAJOR (principle removal/redefinition, governance breaking), MINOR (new principle or major expansion), PATCH (clarifications, typos, non‑semantic wording). Current version starts at 1.0.0.
Compliance Reviews: Each feature plan includes a Constitution Check gate; reviewers MUST block merge if any mandatory gate lacks evidence.
Enforcement: CI job aggregates flakiness + principle gate annotations; failures block merge. Emergency exceptions require follow‑up issue within 24h.
Deferred Items: Any TODO(...) left in this document MUST appear in Sync Impact Report until resolved.

**Version**: 1.1.0 | **Ratified**: 2025-10-14 | **Last Amended**: 2025-10-14
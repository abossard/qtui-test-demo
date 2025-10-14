# Behavior-First TDD & Testing Strategy

This repository practices **Behavior-First Test-Driven Development** (Principle III). The smallest testable unit is a **unit of behavior**: an externally observable feature slice, not a single class.

## 1. Definitions
- Unit of Behavior: A coherent feature outcome triggered by user input, signal, or API call with observable effects (UI mutation, signal emission, model change, file output within allowed scope).
- Behavior Specification: Implementation-agnostic description (inputs, triggers, expected outputs, invariants, edge cases, non-goals) stored per feature at `specs/<feature>/behavior.md` or embedded in `spec.md`.
- Blackbox Test: Verifies only observable results and public contracts; avoids assertions on internal classes, private methods, call sequences, or ephemeral data structure choices.

## 2. Workflow
1. Draft or update Behavior Specification.
2. Write/modify a failing feature-level blackbox test.
3. (Optionally) Add supporting unit/integration tests for tricky invariants.
4. Implement minimal code to pass the feature test.
5. Refactor (preserving green state) ensuring simplicity & deep module characteristics.
6. Review: Confirm spec ↔ test alignment; reject over-specified assertions.

## 3. Behavior Specification Template
```markdown
# Behavior: <concise name>
Purpose: <value it delivers>
Inputs/Triggers:
- <event 1>
- <event 2>
Observable Outcomes:
- <state/UI/signal change>
Invariants:
- <must always hold>
Edge Cases:
- <case + expected outcome>
Non-Goals:
- <explicitly out of scope>
Open Questions:
- <clarifications needed>
```

## 4. Test Layer Guidance
| Layer | Purpose | When to Add | Avoid |
|-------|---------|-------------|-------|
| Feature (Blackbox) | Validate end-to-end behavior | Always first | Internal structure assertions |
| Integration | Pin module interactions (model/controller) | Complex collaboration or race risk | Duplicating feature test coverage |
| Unit | Constrain pure logic or edge algorithms | Algorithmic complexity, corner math | Testing trivial forwarding |
| GUI (Squish) | Validate user-visible flow & synchronization | UI state change or timing | Re-asserting model logic already covered |

## 5. Blackbox Assertion Patterns
Prefer:
- Final UI text / model row count / emitted signal payload
- Public API return values / error codes
- Persistent state artifacts (if part of feature)
Avoid:
- Number of internal helper calls
- Private member values (accessed via test hooks)
- Concrete container types (e.g. asserting `std::vector` vs content semantics)

## 6. Flakiness Prevention
- Replace sleeps with waits on signals/properties.
- Provide a poll helper with timeout for conditions lacking a direct signal.
- Capture baseline timing metrics; regressions raise an optimization issue.

## 7. Test File Conventions
- Feature tests: `tests/feature/test_<feature>_behavior.cpp` or under integration if using mixed style.
- GUI tests (Squish): `tests/gui/<feature>/test_<flow>.py`.
- Behavior spec file name matches primary test file stem.

## 8. Reviewing Tests
Checklist:
- Behavior spec present & current.
- Test fails without implementation (verified by initial commit ordering or CI evidence).
- Assertions cover all invariants & key edge cases.
- No over-specification.
- Synchronization explicit (no magic sleeps over 50ms clusters).

## 9. Adding Supporting Unit Tests
Use when:
- Edge arithmetic or parsing logic benefits from fast micro feedback.
- Multiple boundary cases would bloat feature test readability.
- Race conditions need deterministic isolation.

## 10. Test Data Strategy
- Prefer inline minimal fixtures; escalate to factory helpers only after duplication >2.
- Use named builders for readability if constructing complex models.

## 11. Copilot Usage Notes
- Generate *draft* tests; prune assertions that reference internals.
- Always rewrite AI-generated comments to ensure accuracy.

## 12. Example Minimal Feature Test Skeleton (GoogleTest C++)
```cpp
TEST(OrdersTest, AddsOrderVisibleInTable) {
    // Arrange (Behavior spec: Adds new order, visible with correct fields)
    OrdersController ctl; // high-level facade
    auto initialCount = ctl.orderCount();

    // Act
    ctl.addOrder({42, "Acme", "Pending"});

    // Assert
    EXPECT_EQ(initialCount + 1, ctl.orderCount());
    auto o = ctl.findOrder(42);
    ASSERT_TRUE(o.has_value());
    EXPECT_EQ("Acme", o->customer);
    EXPECT_EQ("Pending", o->status);
}
```

## 13. Framework Notes
GoogleTest/GoogleMock chosen for: widespread adoption, rich matcher ecosystem, native mocking support, strong CI tooling examples. Sections-style structuring formerly planned (Catch2) replaced by explicit test fixture patterns if needed. No migration cost (decision changed pre-implementation).

## 14. Migration Strategy for Legacy Tests
1. Identify class-focused tests that duplicate feature coverage.
2. Merge assertions into feature test where missing.
3. Remove redundant unit tests.
4. Keep a thin unit layer for algorithmic invariants only.

## 15. Metrics (Future)
- Feature Test to Unit Test Ratio target: ~1:1 or lean toward feature tests.
- Flakiness threshold auto-reported; sustained breach blocks merge.

---
Amendments to this strategy follow the Constitution governance process.

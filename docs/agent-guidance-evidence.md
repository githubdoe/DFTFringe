# Agent Guidance Evidence

## Survey Scope

- Repository: [githubdoe/DFTFringe](https://github.com/githubdoe/DFTFringe).
- Creation window: 2022-01-01 through 2026-11-04, inclusive. Open and closed items are included. The earliest matching items in the enumerated corpus were created in October 2022.
- Dated searches: [issues](https://github.com/githubdoe/DFTFringe/issues?q=is%3Aissue+created%3A2022-01-01..2026-11-04), [pull requests](https://github.com/githubdoe/DFTFringe/issues?q=is%3Apr+created%3A2022-01-01..2026-11-04), and [discussions](https://github.com/githubdoe/DFTFringe/discussions?discussions_q=is%3Adiscussion+created%3A2022-01-01..2026-11-04).
- Search results contained 174 issues and 222 pull requests. The discussion filter surfaced seven threads: [#170](https://github.com/githubdoe/DFTFringe/discussions/170), [#269](https://github.com/githubdoe/DFTFringe/discussions/269), [#286](https://github.com/githubdoe/DFTFringe/discussions/286), [#298](https://github.com/githubdoe/DFTFringe/discussions/298), [#303](https://github.com/githubdoe/DFTFringe/discussions/303), [#308](https://github.com/githubdoe/DFTFringe/discussions/308), and [#404](https://github.com/githubdoe/DFTFringe/discussions/404). The discussion search's second page was empty.

## Coverage And Method

All 396 issue/PR search records were enumerated across paginated results. Of those, 372 had non-empty descriptions (all 174 issues and 198 of 222 PRs); title and description text was scanned for recurring behavior/reproduction, UI/state, wavefront/numerics, build/release, and validation topics. Those keyword buckets overlap and are directional, not mutually exclusive statistical classifications. Twenty-four PRs had no description.

| Year created | Issues | Pull requests |
|---|---:|---:|
| 2022 (from Jan 1) | 2 | 2 |
| 2023 | 46 | 66 |
| 2024 | 31 | 21 |
| 2025 | 56 | 79 |
| 2026 (through Nov 4) | 39 | 54 |
| **Total** | **174** | **222** |

Current search states: 131 issues closed and 43 open; 219 pull requests closed and 3 open. GitHub search reports PR state as open/closed; this table does not equate every closed PR with a merged PR.

The seven surfaced discussion pages and selected high-signal issues/PRs from 2022-2026 were manually inspected for requirements, review feedback, regression reports, and explicit test/build evidence. Key examples include early reports and process changes #4, #6, #9, #10, #15, #22, #37/#39, then #298, #303, #286, #389, #394/#395, #356, #370, #381/#382, #366, and #405.

**Conversation coverage is not exhaustive.** GitHub's unauthenticated page rendering exposed collapsed or hidden replies in long threads (notably #298 and #366), and this environment has no authenticated `gh` CLI or GraphQL session. The issue/PR inventory and description scan completed for the dated search results, but this report does not claim that every comment, nested discussion reply, inline review thread, or changed-file diff across all 222 PRs was read. A subsequent REST verification request was blocked by the anonymous API rate limit. The per-thread links let a maintainer inspect any item directly.

## Evidence To Guidance

### State the behavior before changing it

- [Discussion #298](https://github.com/githubdoe/DFTFringe/discussions/298) contains a long disagreement about when inversion prompts should appear, including different novice, experienced-user, loaded-file, simulated-wavefront, and batch-processing cases. The thread itself proposed writing explicit constraints, requirements, and solution options before further implementation. It does not establish a single universal inversion policy; the lesson is to clarify the owner-approved behavior when requirements conflict.
- [Issue #303](https://github.com/githubdoe/DFTFringe/discussions/303) reports idle CPU use that another contributor could not reproduce in the described versions/workflows. The discussion distinguishes observation from cause and asks for a concrete reproduction before pursuing threading or busy-loop changes.
- [Issue #260](https://github.com/githubdoe/DFTFringe/issues/260) and [discussion #286](https://github.com/githubdoe/DFTFringe/discussions/286) describe UI problems at high DPI and specific display scales. This supports recording the user's actual environment and visible failure, not assuming one developer's screen represents all configurations.

**Guidance:** For behavior changes, identify the user-visible reproduction and expected result. If the discussion shows competing interpretations, resolve the requirement and affected use cases before coding; do not convert speculation into a product rule.

### Trace every state and action path

- [Issue #394](https://github.com/githubdoe/DFTFringe/issues/394) says QA exercised “recompute” but missed the separate “update selected wavefronts” action; the latter updated data without clearing the Zernike status markers. [PR #395](https://github.com/githubdoe/DFTFringe/pull/395) fixed the state update at the shared surface-generation path.
- [Issue #389](https://github.com/githubdoe/DFTFringe/issues/389) captures a conflict between a global Gaussian-blur control and per-wavefront blur state. Clicking a wavefront changed a control value the user expected to remain their chosen input. The discussion required checking both user intent and displayed per-item state, including mixed selections.
- [Issue #121](https://github.com/githubdoe/DFTFringe/issues/121) and [PR #370](https://github.com/githubdoe/DFTFringe/pull/370) cover mirror-dialog edits leaking through Cancel, load/apply behavior, persistence, and outline-state adoption. PR #370 remains a work-in-progress in the inspected page; its review is evidence of risk areas, not a claim that the proposed refactor is an accepted project pattern.
- [Issue #37](https://github.com/githubdoe/DFTFringe/issues/37) and [PR #39](https://github.com/githubdoe/DFTFringe/pull/39) trace edge-mask state across app restart and outline-file load/save. Review found additional config-versus-outline persistence and wording questions before approval, illustrating that fixing the immediate computed value was not enough to validate the complete persisted-state workflow.

**Guidance:** Find the state owner and all user/programmatic mutation paths. For persisted settings, trace startup/load, edits, accept/cancel, save/reload, and subsequent processing or restart; clarify which file/store is the source of truth. For selection-aware controls, test each equivalent command and both uniform and mixed selections.

### Preserve copy and numeric semantics

- [Issue #62](https://github.com/githubdoe/DFTFringe/issues/62) and [PR #356](https://github.com/githubdoe/DFTFringe/pull/356) discuss wavefront copy behavior, `cv::Mat` shallow/deep copies, regions, and affected call sites. Reviewers explicitly warned that changing semantics could affect performance and shared data, so call sites were inspected and behavior-specific cloning was kept explicit.
- [Discussion #269](https://github.com/githubdoe/DFTFringe/discussions/269) documents why PV metrics can be subjective or misleading without a defined method and context. Participants asked for the use case and measurement method rather than deriving or displaying an apparently precise value from insufficient information.

**Guidance:** Treat data ownership and domain metrics as contracts. Inspect affected consumers and document assumptions before changing copy depth, numerical interpretation, or reporting.

### Validate geometry and platform variants

- [Issue #380](https://github.com/githubdoe/DFTFringe/issues/380), [PR #381](https://github.com/githubdoe/DFTFringe/pull/381), and follow-up [PR #382](https://github.com/githubdoe/DFTFringe/pull/382) show that 3D mouse interaction needed refinement after users tested rotated and near-axis-aligned views. PR #381 also had to retain Qt 5/Qt 6 event compatibility; the PR page shows 17 checks passing after changes and review.
- The dated corpus includes Qt 6 migration and cross-platform build issues, including [PR #214](https://github.com/githubdoe/DFTFringe/pull/214), [issue #225](https://github.com/githubdoe/DFTFringe/issues/225), and the build/release discussion in [#404](https://github.com/githubdoe/DFTFringe/discussions/404).
- Crash reports recur in [issue #112 / PR #114](https://github.com/githubdoe/DFTFringe/pull/114), [issue #246 / PR #251](https://github.com/githubdoe/DFTFringe/pull/251), and other issue/PR pairs. Build failures and packaging errors also recur across Windows, Linux, and macOS workflows.

**Guidance:** Validate interaction at the orientations and selection states that can change its meaning. Build the intended Qt target and inspect CI for the other supported target/platform whenever shared code or project files are affected.

### Keep scope controlled and checks deterministic

- [PR #356](https://github.com/githubdoe/DFTFringe/pull/356) includes discussion of limiting a risky semantic change to the requested copy issue and identifying C++14/C++17 compatibility before settling the implementation.
- [Discussion #405](https://github.com/githubdoe/DFTFringe/issues/405) includes a contributor's view that lint, tests, and other deterministic fast feedback are stronger enforcement than skill prose. This is participant feedback, not a repository policy vote; it aligns with the existing [`.clang-tidy`](../.clang-tidy) and [CI workflows](../.github/workflows/).
- Release tracking in [issue #366](https://github.com/githubdoe/DFTFringe/issues/366) cross-links fixes and test status, while [discussion #404](https://github.com/githubdoe/DFTFringe/discussions/404) compares release timing with platform-specific changes and user demand.

**Guidance:** Keep a fix as small as practical, avoid unrelated refactors, and use CI/lint/build checks as the authority for pass/fail. Written agent rules provide context and reminders; they do not replace executable validation.

### Keep generated files out of source changes

- [PR #9](https://github.com/githubdoe/DFTFringe/pull/9) removed autogenerated include files after confirming they were not required to build on Linux/WSL.
- [PR #10](https://github.com/githubdoe/DFTFringe/pull/10) introduced Linux PR builds in part to catch platform-only breaks and missing files; [PR #15](https://github.com/githubdoe/DFTFringe/pull/15) added Windows CI and tested the produced artifact on a clean environment. Review also caught that required help images/resources were missing from an early package.

**Guidance:** Keep generated qmake outputs and transient build artifacts out of source changes unless the project explicitly tracks them. Register hand-maintained sources/forms/resources in the qmake project and rely on the CI build for missing-file and platform checks.

### Preserve useful issue and review workflow

- [Issue #6](https://github.com/githubdoe/DFTFringe/issues/6) gives a short, concrete crash path, app version, and OS. Its discussion also clarifies that GitHub issues are acceptable for reports even though maintainers may be less active there.
- [Issue #22](https://github.com/githubdoe/DFTFringe/issues/22) starts with crashes that could not be reproduced reliably and asks for logs; the discussion separates debug output from user-facing logging and distinguishes logging from crash dumps.
- [PR #39](https://github.com/githubdoe/DFTFringe/pull/39) demonstrates peer review discovering adjacent load/save behavior and terminology issues after the initial defect appeared fixed.

**Guidance:** Preserve the reproduction path, version/platform, and evidence when available. Keep adjacent behaviors in scope only when they are part of the same state/data contract; otherwise record and split them. Use peer review and target CI to catch omissions rather than relying on a successful local path alone.

## Resulting Agent Changes

The evidence supports additions to root [AGENTS.md](../AGENTS.md): require an observable behavior statement; clarify disputed requirements; trace UI and persisted-state ownership; validate selection variants, geometry, and affected Qt targets; keep generated build outputs out of commits; and avoid scope expansion. Existing qmake/build and `cv::Mat` notes were retained. No separate skill or file-scoped instruction was added because the evidence supports concise always-on cautions, while repository build/lint checks remain the deterministic enforcement mechanism.

## Reproducibility

The initial and paginated GitHub issue/PR search totals matched: 174 issues and 222 PRs. The filtered Discussions page displayed seven results, with no second-page entries. Public links above are the source record. The follow-up REST verification was rate-limited, so the recorded counts are from the successful expanded search and its pagination, not a later independent API check. A future refresh should use the date-filtered searches, record current counts, and re-check open or recently active threads because statuses and discussions can change after this snapshot.
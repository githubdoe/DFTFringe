# DFTFringe Agent Guidance Survey Plan

## Goal

Use DFTFringe's GitHub collaboration history to improve project-specific agent guidance. Keep the guidance concise and actionable; preserve traceability in a separate evidence report instead of copying conversations into agent context.

## Status

Updated for the expanded survey. The dated issue/PR inventory contains 174 issues and 222 PRs; all 396 records were enumerated, and 372 non-empty descriptions were scanned. Seven filtered discussions were surfaced. Selected conversations/reviews informed concise additions to [AGENTS.md](../AGENTS.md); [agent-guidance-evidence.md](agent-guidance-evidence.md) records sources, findings, and limits. Some long-thread replies and inline reviews were hidden, and the later API quota check was rate-limited, so comment-level coverage is explicitly partial.

## Scope

- Repository: [githubdoe/DFTFringe](https://github.com/githubdoe/DFTFringe).
- Creation window: 2022-01-01 through 2026-11-04, inclusive; include open and closed items. The earliest matching repository activity currently surfaced is from October 2022.
- Sources: GitHub issues, pull requests, comments/reviews, and discussions, following cross-links between them.
- Expanded search totals: 174 issues, 222 pull requests, and 7 discussions. The issue/PR results were enumerated across pages; further anonymous API checks later hit the rate limit.
- Do not survey Groups.io independently; follow its links only when directly needed to interpret a GitHub item.

## Work

1. Reconcile the current [AGENTS.md](../AGENTS.md), issue templates, [CI workflows](../.github/workflows/), [clang-tidy configuration](../.clang-tidy), and [README](../README.md) with the history. Prefer links to existing references over duplicating build details.
2. Inventory every matching issue and pull request. Record identifiers, dates, status, labels, comment counts, and links; scan descriptions and read source conversations/PR review threads where accessible. Trace issue-to-PR relationships and note explicit validation evidence.
3. Read every matching discussion and its comments. Identify recurring, durable practices rather than turning one-off preferences into global rules.
4. Update root [AGENTS.md](../AGENTS.md) only with broadly useful guidance not already covered. Add selectively loaded instructions or a skill only if repeated evidence supports a distinct workflow. Keep enforceable rules in tests, linters, or CI.
5. Maintain `docs/agent-guidance-evidence.md` to document the collection method, annual coverage, limitations, and a linked evidence-to-recommendation mapping.

## Validation

- Match inventory totals and pages to GitHub's dated search results; track any inaccessible items, rate limits, or missing review/comment content explicitly.
- Spot-check representative issue discussions, PR reviews/files, issue-to-PR pairs, and all recommendations in the evidence report.
- Confirm guidance is concise, non-duplicative, and consistent with current CI and docs; verify local Markdown links.
- Keep product code and build configuration out of scope.

## Access Note

Public GitHub REST search is available, but this environment has no `gh` CLI and uses unauthenticated API access. Search quotas, pagination, and discussion/review access may constrain exhaustive conversation retrieval. Do not claim complete review of content that could not be fetched and verified; record such gaps in the evidence report.
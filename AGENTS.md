# Repository guidance for coding assistants

This file supplies project context, not tool permissions or environment isolation.
No particular editor, assistant, or agent workflow is required to contribute.

## Project and build

- DFTFringe is a Qt Widgets C++ application built with qmake.
  [mainwindow.cpp](mainwindow.cpp) composes the UI;
  [surfacemanager.cpp](surfacemanager.cpp) coordinates wavefront operations and
  view updates; [wavefront.h](wavefront.h) holds per-wavefront data and state.
- [DFTFringe.pro](DFTFringe.pro) targets Qt 6/C++17;
  [DFTFringe_QT5.pro](DFTFringe_QT5.pro) targets Qt 5/C++14.
  Check both variants when changing shared code. Register new sources, headers,
  forms, and resources in the relevant explicit qmake lists.
- Use [README.md](README.md) for setup and the existing [CI workflows](.github/workflows)
  for platform-specific dependencies and build commands. Use a matching Qt and
  compiler toolchain; do not assume the first qmake or make on PATH is correct.
- Keep generated qmake outputs and transient build artifacts out of commits.
  Runtime assets are also copied by packaging workflows; check those workflows
  when changing asset lookup or packaging.

## Change safely

- Establish the affected user path and expected result before changing behavior.
  Keep the fix focused; discuss consequential scientific, saved-format,
  dependency, or compatibility changes with maintainers rather than choosing
  policy silently.
- Trace state owners, callers, and both user-driven and programmatic updates.
  For settings, include load, accept/cancel, save/reload, and restart paths.
  For selection-aware controls, include single, multiple, and mixed selections,
  and equivalent actions such as recompute and update-selected.
- OpenCV `cv::Mat` assignment shares storage. Inspect affected copy and mutation
  paths before changing wavefront ownership; do not assume a copied matrix is
  independent.
- For 3D geometry or interaction changes, check axis-aligned and oblique views.

## Validate and report

- Use relevant existing tests, builds, and [.clang-tidy](.clang-tidy) checks.
  Add focused regression coverage where practical. Define reference values,
  units, and tolerances for numerical changes.
- A build alone does not verify UI or scientific behavior. Check the affected
  workflow and report checks actually run, their environment and results, and
  remaining limitations. Do not present planned or unobserved checks as passed.

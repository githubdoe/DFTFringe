# Agent Instructions

## Project Shape
- DFTFringe is a Qt Widgets C++ application built with qmake. `mainwindow.*` composes the workflows and views; `surfacemanager.*` coordinates wavefront operations and view updates; `wavefront.*` owns per-wavefront data and processing state.
- The Qt 6 build uses `DFTFringe.pro` and C++17. The separate Qt 5 build uses `DFTFringe_QT5.pro` and C++14. Keep a change compatible with the intended target; check whether both variants need updates.
- The qmake project files explicitly list sources, headers, forms, and resources. Register new files in the appropriate list and follow its alphabetical ordering.
- Do not add generated qmake outputs or transient build artifacts unless the project explicitly tracks them; add the hand-maintained source, header, form, or resource and update the `.pro` file instead.

## Change Safely
- Keep changes narrowly scoped and preserve existing behavior unless the request explicitly changes it. For stateful UI or analysis changes, trace the owning code path and relevant callers before editing.
- Before changing user-visible behavior, state a reproducible user path and expected outcome. If maintainers or users disagree about the behavior, clarify the requirement and affected use cases before implementing a solution.
- Qt widget state can be set both by user interaction and by programmatic load/initialization. Check both paths when changing enablement, selection, or derived display state.
- For persisted settings, trace startup/load, edits, accept/cancel, save/reload, and subsequent processing or restart paths; clarify which file or settings store is the source of truth.
- For controls that reflect per-wavefront state or act on selections, identify the source of truth and verify every equivalent action path (for example, recompute versus update-selected), including mixed and multi-selection cases.
- `cv::Mat` copies can share storage. Before changing `wavefront` copy, move, or lifetime behavior, inspect every affected call site and preserve the intended shallow/deep semantics; do not assume a copy is independent.
- For 3D interaction or geometry changes, verify behavior at axis-aligned as well as oblique camera orientations; a fix that works only at one angle can reverse or distort other movements.
- Avoid expanding a focused fix into a broad refactor without a demonstrated need; when a change does widen scope, validate the extra behavior and each supported Qt target it affects.
- Do not clean generated files or remove untracked work as part of an unrelated task.

## Build And Validation
- Use the matching qmake project and toolchain. With dependencies provisioned, the Qt 6 Windows build is `qmake.exe` followed by `mingw32-make -j4`; the Linux build is `/usr/lib/qt6/bin/qmake DFTFringe.pro` followed by `make -j4`.
- Use the CI workflows as the source of truth for dependency versions, setup, and platform-specific build details: [Windows Qt 6](.github/workflows/build-windows.yml), [Linux Qt 6](.github/workflows/build-linux.yml), [macOS](.github/workflows/build-macos.yml), and [Windows Qt 5](.github/workflows/build-windows-QT5.yml). The developer setup notes are in [README.md](README.md).
- No dedicated unit-test target was found. Run the relevant build and available static checks; for behavior changes, add or perform a focused check of the affected workflow when practical.
- Packaging copies runtime data such as `ColorMaps`, help files, and revision history separately from the qmake resource list. Check the platform workflow when changing runtime asset lookup or packaging.

## Focused References
- For the GitHub-history basis and limits of these agent rules, see [agent-guidance-evidence.md](docs/agent-guidance-evidence.md).
- Keep project changes consistent with the local `.clang-tidy` checks; CI lints changed lines and excludes vendored/generated code.
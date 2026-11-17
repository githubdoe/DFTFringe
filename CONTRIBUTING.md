# Contributing to DFTFringe

For usage questions and support, visit the
[Interferometry group](https://groups.io/g/Interferometry). GitHub issues are
welcome for bug reports and feature suggestions.

## Describe the change

Explain the problem and the user-visible outcome you want. For a bug, include
the steps and inputs that reproduce it, what you expected, and what actually
happened. Screenshots or a small sample can help; remove private data before
sharing. For numerical differences, include reference values, their source,
units, settings, and tolerances when known.

Keep the explanation proportionate: a small fix may need only a few sentences
in the PR. For larger changes, discussing alternatives and compatibility early
can save rework, especially when scientific conventions, saved data, or
dependencies are affected.

## Make a focused change

Use the [architecture map](docs/architecture.md) to find relevant code, then
trace the implementation and callers. For stateful changes, check both user
actions and loading or initialization paths. Keep fixes narrow, preserve
unrelated behavior, and avoid mixing in cleanup or broad refactoring.
Update directly related documentation and add focused regression coverage
where practical.

See the [README](README.md) for setup. Match the Qt version, compiler, and
dependency toolchain to the project you are building:

- [DFTFringe.pro](DFTFringe.pro): Qt 6, C++17.
- [DFTFringe_QT5.pro](DFTFringe_QT5.pro): Qt 5, C++14.

The existing CI configurations show platform-specific setup, dependency
versions, and build commands:
[Windows Qt 6](.github/workflows/build-windows.yml),
[Linux Qt 6](.github/workflows/build-linux.yml),
[macOS](.github/workflows/build-macos.yml),
[Windows Qt 5](.github/workflows/build-windows-QT5.yml), and
[Linux Qt 5](.github/workflows/build-linux-QT5.yml).
Consider both Qt variants when shared code is affected. The qmake project files
explicitly list sources, headers, forms, and resources; update the relevant
lists when adding files. Leave generated build output, personal configuration,
and unrelated logs or research files out of the contribution.

## Check and submit

Choose checks that exercise the change: relevant tests, builds, static checks,
or a focused manual workflow. For a UI fix, repeat the reproduction steps and
check nearby behavior. For numerical changes, compare with stated references
and tolerances. A successful build alone does not establish UI or scientific
correctness.

Review the complete diff before submitting. In the PR, explain the problem and
scope, link a related issue if any, and report validation **actually performed**:
commands or manual steps, environment and Qt variant, inputs, and results.
Separately note checks not run, why, and any remaining limitations or risks.
Do not present planned checks or unobserved CI results as completed validation.

Contributors are responsible for understanding and reviewing their changes,
checking results, and respecting licenses and data privacy, whether they use
AI assistance or not. Generated code and tool output need the same scrutiny as
any other contribution; no particular editor or assistant is required.

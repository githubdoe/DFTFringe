# Testing and validation

## Focused numerical tests

[tests.pro](../tests/tests.pro) builds a standalone Qt Test executable against the
actual [circle-fit implementation](../circlefit.cpp), without the application or
OpenCV/Qwt dependencies. Use a matching Qt/compiler pair. Qt Test is part of the
Qt base development packages already used by CI.

The data-driven checks cover the Hyper fit of exact unit, translated,
small-radius, and partial-arc circles. They check finite center/radius/residual,
absolute parameter tolerance of `1e-9 * max(1, radius)`, a non-negative residual
within that tolerance, and unchanged input coordinates and point count. These
are deterministic baseline tests, not evidence that noisy, degenerate, GUI, or
complete optical-analysis workflows are validated.

From the repository root, using an out-of-source build:

### Windows PowerShell (Qt 6)

Put the matching MinGW and Qt `bin` directories first on PATH, rather than a
different installed make/compiler. For example, with the paths from the
[Windows setup instructions](../README.md#how-to-build-dftfringe-on-windows):

```powershell
$env:Path = "C:\buildingDFTFringe\Tools\mingw1310_64\bin;C:\buildingDFTFringe\6.8.3\mingw_64\bin;$env:Path"
New-Item -ItemType Directory -Force build\tests | Out-Null
Push-Location build\tests
qmake.exe ..\..\tests\tests.pro CONFIG+=release
mingw32-make -j4
.\release\tst_circlefit.exe
Pop-Location
```

### Linux (Qt 6)

```sh
mkdir -p build/tests
cd build/tests
/usr/lib/qt6/bin/qmake ../../tests/tests.pro CONFIG+=release
make -j4
./tst_circlefit
```

For Qt 5, use the Qt 5 qmake instead; the test project selects C++14 rather than
C++17. On macOS use the matching Qt qmake and `make`. The Linux Qt 5 and Qt 6
build workflows include this target; Windows and macOS CI do not run it.
The tests do not create a GUI or need a display server.

Add focused cases for fixes and known numerical results with explicit units and
tolerances. Avoid random fixtures, unsupported degenerate-input expectations,
and copying implementation logic to manufacture expected results.

## Application checks

Use [README](../README.md) and the existing [CI workflows](../.github/workflows)
for the relevant full application build. Passing the standalone tests does not
replace a build of changed application code.

For affected behavior, record input, environment, steps, expected result, and
observed result for relevant checks:

- Startup and loading an existing input.
- User edits and programmatic initialization.
- Accept versus cancel.
- Save/reload and restart, using temporary copies of user data.
- Single, multiple, and mixed selections.
- Equivalent commands such as recompute and update-selected.
- Numerical output with defined tolerances.
- Axis-aligned and oblique views for geometry changes.
- Qt variants, supported platforms, and packaged asset lookup when affected.

Run only checks relevant to the change, explaining omissions. Never report this
checklist as executed merely because it exists.

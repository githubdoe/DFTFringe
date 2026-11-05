# Architecture map

This is a navigation aid describing the current implementation, not a required
design or a refactoring proposal.

## Main entry points

| Area | Where to look |
| --- | --- |
| Window composition and signal/slot wiring | [mainwindow.cpp](../mainwindow.cpp) and [mainwindow.h](../mainwindow.h): `MainWindow` sets up the interferogram, analysis, and result views, controls, and surface manager connections. |
| Phase-map generation | [dftarea.cpp](../dftarea.cpp): `DFTArea::makeSurface` calculates and unwraps phase, applies configured flips and fringe spacing, and emits `newWavefront`. The PSI path also emits `newWavefront`. |
| Surface operations and file I/O | [surfacemanager.cpp](../surfacemanager.cpp) and [surfacemanager.h](../surfacemanager.h): `SurfaceManager` keeps `m_wavefronts` and the current index, coordinates processing and surface operations, and implements `readWaveFront` and `writeWavefront`. |
| Per-wavefront data | [wavefront.h](../wavefront.h) and [wavefront.cpp](../wavefront.cpp): arrays, masks, outlines, Zernike values, metadata, and processing flags. |
| Zernike processing | [zernikeprocess.cpp](../zernikeprocess.cpp): routines called by the surface manager for fitting Zernike terms, filling voids, and nulling terms. |

## From analysis to results

`MainWindow` connects `DFTArea::newWavefront` to
`SurfaceManager::createWaveFrontFromPhaseMap`, which populates a wavefront,
sets up its mask, and starts processing.

`SurfaceManager::generateSurfacefromWavefront` coordinates Zernike fitting,
void filling, masking, nulling, and optional blur. Its branches depend on
wavefront state and mirror geometry; for example, elliptical surfaces take a
separate path.

`SurfaceManager::sendSurface` computes metrics and passes the wavefront to the
contour, profile, 3D, simulation, and Foucault views. Selection and control
updates also travel through Qt signals and slots.

## State and build navigation

- Wavefront arrays are OpenCV matrices. The `wavefront` copy constructor and
  `cloneMatricesFrom` clone the matrices, while default copy assignment is
  memberwise and can share matrix storage. `createWaveFrontFromPhaseMap`
  directly assigns its phase matrix to `wf->data`.
- Preferences are not centralized in one class. `MainWindow` restores window
  geometry and state using `QSettings`; `SurfaceManager` also reads and writes
  processing preferences. [mirrordlg.cpp](../mirrordlg.cpp) handles mirror
  settings, including fringe spacing and flips.
- [DFTFringe.pro](../DFTFringe.pro) is the Qt 6/C++17 qmake project;
  [DFTFringe_QT5.pro](../DFTFringe_QT5.pro) is the Qt 5/C++14 project.
  Both explicitly list sources, headers, forms, and resources and configure
  platform-specific external libraries. See the [README](../README.md) for
  setup and [contribution guidance](../CONTRIBUTING.md) for validation and
  platform build links.

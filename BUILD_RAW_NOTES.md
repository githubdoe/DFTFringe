# Компилиране на DFTFringe с RAW поддръжка (камерни файлове)

Промените за четене на камерни RAW файлове (CR2/CR3/NEF/ARW/DNG/RW2 и др.) вече са
вградени в сорса. Този файл описва какво е нужно, за да се компилира на Windows.

## Какво е променено
- `rawimage.h` / `rawimage.cpp` — нов модул, чете RAW чрез LibRaw → QImage.
- `igramarea.cpp` — `openImage()` разпознава RAW и го чете (вкл. distortion клона).
- `mainwindow.cpp` — RAW разширенията са добавени във файловите диалози.
- `DFTFringe.pro` — добавени са LibRaw include/lib и новите файлове.

## Нужен toolchain (същият, с който е построен инсталираният релийз в C:\DFTFringe)
Инсталираният exe е Qt6 + MinGW-w64 (GCC, SEH) + OpenCV 4.12.0.

1. **Qt6 с MinGW комплект** (Qt Online Installer) + модули: Qt Charts, Qt Data Visualization.
   Дава `qmake`, `mingw32-make`, `g++`, Qt Creator.

2. **Предварително построените зависимости**, които `DFTFringe.pro` (win32 секцията)
   очаква като СЪСЕДНИ папки на сорса, т.е. в `D:\Pictures\Mirrors\`:
   - `qwt-6.3.0`
   - `build_armadillo`
   - `build_openCV`   (OpenCV 4.12.0, MinGW — libopencv_*4120)
   - `build_lapack`
   Тези липсват в момента. Взимат се от билд-настройката на DFTFringe проекта
   (upstream repo-то документира/сваля точните версии). Без тях qmake няма да намери
   пътищата в win32 секцията.

3. **LibRaw** (НОВАТА зависимост), построена със същия MinGW, инсталирана в:
   `D:\Pictures\Mirrors\build_libraw\install\`
   - хедъри в `install\include\libraw\libraw.h`
   - `install\bin\libraw.dll` + `libraw.dll.a` (import lib)
   Ако я сложиш другаде — коригирай двата реда `libraw` в `DFTFringe.pro` (win32).
   Най-лесно набавяне: MSYS2 `pacman -S mingw-w64-x86_64-libraw` (същия ABI) и копиране
   на include/lib/dll, или билд от източник.

## Компилиране
От Qt Creator: отвори `DFTFringe.pro`, избери MinGW kit-а, Build → Release.

Или от команден ред (в среда с qmake/mingw в PATH):
```
qmake DFTFringe.pro
mingw32-make release
```

## Пускане / разпространение
Копирай `libraw.dll` (и евентуалните ѝ зависимости) до новополучения `DFTFringe.exe`,
до вече наличните Qt6/OpenCV DLL-ове. Най-просто: сложи новия exe и `libraw.dll` в
`C:\DFTFringe\` (където са всички останали DLL-ове) и го пусни оттам.

## Алтернатива без локална инсталация
DFTFringe upstream има GitHub Actions CI, който сам сглобява целия toolchain и
произвежда готов exe/инсталатор. Fork → приложи тези промени → добави LibRaw към CI
стъпката → push → свали построения артефакт.

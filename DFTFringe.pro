#    ____  _____ _____ _____     _
#   |    \|   __|_   _|   __|___|_|___ ___ ___
#   |  |  |   __| | | |   __|  _| |   | . | -_|
#   |____/|__|    |_| |__|  |_| |_|_|_|_  |___|
#                                     |___|


TARGET = DFTFringe

VERSION = MY_AUTOMATED_VERSION_STRING

DEFINES += APP_VERSION=\\\"$$VERSION\\\"
DEFINES += QAPPLICATION_CLASS=QApplication

TEMPLATE = app

QT += charts concurrent core core5compat datavisualization gui network opengl widgets xml

qtHaveModule(printsupport): QT += printsupport

QMAKE_CXXFLAGS += -std=c++17

# disable qDebug() in release
CONFIG( release, debug|release ) {
    message("Release build")
    DEFINES += QT_NO_DEBUG_OUTPUT
}

# Below are the three platform specific project configurations for WINDOWS, LINUX and MAC

# WINDOWS ##########
win32 {
    message("Using WINDOWS project configuration.")

    CONFIG( debug, debug|release ) {
        LIBS += -L..\qwt-6.3.0\lib -lqwtd # debug
    } else {
        LIBS += -L..\qwt-6.3.0\lib -lqwt # release
        CONFIG+=force_debug_info # keep debug infos (even in release build) to be able to link stacktrace address to actual function
        CONFIG+=separate_debug_info # separate debug infos into a .exe.debug to not grow the .exe
    }

    # NOTE: RC_FILE is Windows only, breaks Mac (and Linux?) builds if it in their scope.
    RC_FILE = DFTFringe.rc

    INCLUDEPATH += ..\qwt-6.3.0\src
    INCLUDEPATH += ..\build_armadillo\tmp\include
    INCLUDEPATH += ..\build_openCV\install\include

    LIBS += -L..\build_lapack\bin -llibblas
    LIBS += -L..\build_lapack\bin -lliblapack
    LIBS += -L..\build_openCV\install\x64\mingw\bin -llibopencv_calib3d4120
    LIBS += -L..\build_openCV\install\x64\mingw\bin -llibopencv_core4120
    LIBS += -L..\build_openCV\install\x64\mingw\bin -llibopencv_features2d4120
    LIBS += -L..\build_openCV\install\x64\mingw\bin -llibopencv_highgui4120
    LIBS += -L..\build_openCV\install\x64\mingw\bin -llibopencv_imgcodecs4120
    LIBS += -L..\build_openCV\install\x64\mingw\bin -llibopencv_imgproc4120
    LIBS += -ldbghelp # for SetUnhandledExceptionFilter


    # This is for armadillo to not use wrapper. See https://gitlab.com/conradsnicta/armadillo-code#6-linux-and-macos-compiling-and-linking
    DEFINES += ARMA_DONT_USE_WRAPPER
}

# LINUX ############
unix: !mac {
    message("Using LINUX project configuration.")
    contains( CONFIG,debug ) { message("no extra debug libraries") }

    INCLUDEPATH += /usr/include/opencv4
    INCLUDEPATH += /usr/local/qwt-6.3.0/include

    QMAKE_RPATHDIR += /usr/local/qwt-6.3.0/lib

    LIBS += -larmadillo
    LIBS += -lGLU
    LIBS += -lopencv_calib3d
    LIBS += -lopencv_core
    LIBS += -lopencv_features2d
    LIBS += -lopencv_highgui
    LIBS += -lopencv_imgcodecs
    LIBS += -lopencv_imgproc
    LIBS += -L/usr/local/qwt-6.3.0/lib -lqwt
}

# MAC ##############
macx {
    message("Using MACOS project configuration.")
    message( ................................ )
    message("..........PRO FILE: $$_PRO_FILE_")
    message("......PRO FILE PWD: $$_PRO_FILE_PWD_")
    message( ................... )

    CONFIG += app_bundle
    CONFIG += sdk_no_version_check
    CONFIG += link_pkgconfig
    CONFIG += silent

    QMAKE_FULL_VERSION=APP_VERSION
    QMAKE_MACOSX_DEPLOYMENT_TARGET = 11.0
    QMAKE_APPLE_DEVICE_ARCHS = x86_64 arm64

    CONFIG( debug, debug|release )   { DESTDIR = build/debug }
    CONFIG( release, debug|release ) { DESTDIR = build/release }

    MOC_DIR = $$DESTDIR/.moc
    OBJECTS_DIR = $$DESTDIR/.obj #these change between build and release.
    RCC_DIR = $$DESTDIR/.qrc
    UI_DIR = $$DESTDIR/.ui
    QMAKE_MKDIR = /usr/local/bin/mkdir # This tells QMAKE which mkdir command to use.
    QMAKE_PKG_CONFIG = /opt/homebrew/bin/pkg-config # This tells QMAKE which pkg-config executable to use.
    PKG_CONFIG_PATH = $$[QT_INSTALL_LIBS]/pkgconfig
    INCLUDEPATH += -I$$[QT_INSTALL_PLUGINS]
    LIBS += -L$$[QT_INSTALL_PLUGINS]
    PKGCONFIG += armadillo opencv Qt5Qwt6

    message(........QT_VERSION: $$[QT_VERSION])
    message(.QT_INSTALL_PREFIX: $$[QT_INSTALL_PREFIX])
    message(QT_INSTALL_HEADERS: $$[QT_INSTALL_HEADERS])
    message(...QT_INSTALL_LIBS: $$[QT_INSTALL_LIBS])
    message(QT_INSTALL_PLUGINS: $$[QT_INSTALL_PLUGINS])
    message(...................)
    message(...........DESTDIR: $$DESTDIR)
    message(...........MOC_DIR: $$MOC_DIR)
    message(.......OBJECTS_DIR: $$OBJECTS_DIR)
    message(...........RCC_DIR: $$RCC_DIR)
    message(............UI_DIR: $$UI_DIR)
    message(...................)
    message(.......QMAKE_MKDIR: $$QMAKE_MKDIR)
    message(..QMAKE_PKG_CONFIG: $$QMAKE_PKG_CONFIG)
    message(...PKG_CONFIG_PATH: $$PKG_CONFIG_PATH)
    message(.......INCLUDEPATH: $$INCLUDEPATH)
    message(..............LIBS: $$LIBS)
    message(.........PKGCONFIG: $$PKGCONFIG)
    message(............CONFIG: $$CONFIG)
}

# Below are the includes for source files and other resources, sorted alphabetically. ##################################
# If a filename contains spaces it will need quoting.


RESOURCES += DFTResources.qrc

TRANSLATIONS += dftfringe_fr.ts

INCLUDEPATH += ./bezier ./SingleApplication ./zernike

SOURCES += SingleApplication/singleapplication.cpp \
    SingleApplication/singleapplication_p.cpp \
    zernike/zapm.cpp \
    annulushelpdlg.cpp \
    arbitrarywavefronthelp.cpp \
    arbitrarywavwidget.cpp \
    astigpolargraph.cpp \
    astigscatterplot.cpp \
    astigstatsdlg.cpp \
    averagewavefrontfilesdlg.cpp \
    batchigramwizard.cpp \
    bathastigdlg.cpp \
    camcalibrationreviewdlg.cpp \
    cameracalibwizard.cpp \
    camwizardpage1.cpp \
    ccswappeddlg.cpp \
    circlefit.cpp \
    circleoutline.cpp \
    colorchannel.cpp \
    colorchanneldisplay.cpp \
    colormapviewerdlg.cpp \
    contourplot.cpp \
    contourrulerparams.cpp \
    contourtools.cpp \
    contourview.cpp \
    counterrotationdlg.cpp \
    cpoint.cpp \
    defocusdlg.cpp \
    dftarea.cpp \
    dftcolormap.cpp \
    dftthumb.cpp \
    dfttools.cpp \
    edgeplot.cpp \
    filteroutlinesdlg.cpp \
    foucaultview.cpp \
    generatetargetdlg.cpp \
    graphicsutilities.cpp \
    helpdlg.cpp \
    hotkeysdlg.cpp \
    igramarea.cpp \
    igramintensity.cpp \
    imagehisto.cpp \
    intensityplot.cpp \
    jitteroutlinedlg.cpp \
    lensetablemodel.cpp \
    main.cpp \
    mainwindow.cpp \
    messagereceiver.cpp \
    metricsdisplay.cpp \
    mirrordlg.cpp \
    myplotpicker.cpp \
    myutils.cpp \
    nullmarginhelpdlg.cpp \
    nullvariationdlg.cpp \
    oglrendered.cpp \
    oglview.cpp \
    outlinedialog.cpp \
    outlinehelpdocwidget.cpp \
    outlineplots.cpp \
    outlinestatsdlg.cpp \
    pdfcalibrationdlg.cpp \
    percentcorrectiondlg.cpp \
    pixelstats.cpp \
    plotcolor.cpp \
    profileplot.cpp \
    profileplotpicker.cpp \
    psfplot.cpp \
    psi_dlg.cpp \
    psiphasedisplay.cpp \
    psitiltoptions.cpp \
    punwrap.cpp \
    regionedittools.cpp \
    rejectedwavefrontsdlg.cpp \
    renamewavefrontdlg.cpp \
    reportdlg.cpp \
    reviewwindow.cpp \
    rmsplot.cpp \
    rotationdlg.cpp \
    savewavedlg.cpp \
    settings2.cpp \
    settingsdebug.cpp \
    settingsdft.cpp \
    settingsGeneral2.cpp \
    settingsigram.cpp \
    settingsigramimportconfig.cpp \
    settingsprofile.cpp \
    showaliasdlg.cpp \
    showallcontoursdlg.cpp \
    simigramdlg.cpp \
    simulationsview.cpp \
    standastigwizard.cpp \
    statsview.cpp \
    subtractwavefronatsdlg.cpp \
    surface3dcontrolsdlg.cpp \
    surfaceanalysistools.cpp \
    surfacegraph.cpp \
    surfacelightingproxy.cpp \
    surfacemanager.cpp \
    transformwavefrontdlg.cpp \
    unwraperrorsview.cpp \
    usercolormapdlg.cpp \
    userdrawnprofiledlg.cpp \
    utilil.cpp \
    vortexdebug.cpp \
    wavefront.cpp \
    wavefrontaveragefilterdlg.cpp \
    wavefrontfilterdlg.cpp \
    wftexaminer.cpp \
    wftstats.cpp \
    zernikedlg.cpp \
    zernikeeditdlg.cpp \
    zernikepolar.cpp \
    zernikeprocess.cpp \
    zernikes.cpp \
    zernikesmoothingdlg.cpp

HEADERS += bezier/bezier.h \
    SingleApplication/singleapplication_p.h \
    SingleApplication/singleapplication.h \
    zernike/zapm_interface.h \
    annulushelpdlg.h \
    arbitrarywavefronthelp.h \
    astigpolargraph.h \
    arbitrarywavwidget.h \
    astigscatterplot.h \
    astigstatsdlg.h \
    averagewavefrontfilesdlg.h \
    batchigramwizard.h \
    bathastigdlg.h \
    camcalibrationreviewdlg.h \
    cameracalibwizard.h \
    camwizardpage1.h \
    ccswappeddlg.h \
    circle.h \
    circleoutline.h \
    circleutils.h \
    colorchannel.h \
    colorchanneldisplay.h \
    colormapviewerdlg.h \
    contourplot.h \
    contourrulerparams.h \
    contourtools.h \
    contourview.h \
    counterrotationdlg.h \
    cpoint.h \
    defocusdlg.h \
    dftarea.h \
    dftcolormap.h \
    dftthumb.h \
    dfttools.h \
    edgeplot.h \
    filteroutlinesdlg.h \
    foucaultview.h \
    generatetargetdlg.h \
    graphicsutilities.h \
    helpdlg.h \
    hotkeysdlg.h \
    IgramArea.h \
    igramintensity.h \
    imagehisto.h \
    intensityplot.h \
    jitteroutlinedlg.h \
    lensetablemodel.h \
    mainwindow.h \
    messagereceiver.h \
    metricsdisplay.h \
    mirrordlg.h \
    myplotpicker.h \
    myutils.h \
    nullmarginhelpdlg.h \
    nullvariationdlg.h \
    oglrendered.h \
    oglview.h \
    outlinedialog.h \
    outlinehelpdocwidget.h \
    outlineplots.h \
    outlinestatsdlg.h \
    pdfcalibrationdlg.h \
    percentcorrectiondlg.h \
    percentCorrectionSurface.h \
    pixelstats.h \
    plotcolor.h \
    profileplot.h \
    profileplotpicker.h \
    psfplot.h \
    psi_dlg.h \
    psiphasedisplay.h \
    psitiltoptions.h \
    punwrap.h \
    regionedittools.h \
    rejectedwavefrontsdlg.h \
    renamewavefrontdlg.h \
    reportdlg.h \
    reviewwindow.h \
    rmsplot.h \
    rotationdlg.h \
    savewavedlg.h \
    settings2.h \
    settingsdebug.h \
    settingsdft.h \
    settingsGeneral2.h \
    settingsigram.h \
    settingsigramimportconfig.h \
    settingsprofile.h \
    showaliasdlg.h \
    showallcontoursdlg.h \
    simigramdlg.h \
    simulationsview.h \
    standastigwizard.h \
    statsview.h \
    subtractwavefronatsdlg.h \
    surface3dcontrolsdlg.h \
    surfaceanalysistools.h \
    surfacegraph.h \
    surfacelightingproxy.h \
    surfacemanager.h \
    transformwavefrontdlg.h \
    unwraperrorsview.h \
    usercolormapdlg.h \
    userdrawnprofiledlg.h \
    utils.h \
    vortex.h \
    vortexdebug.h \
    wavefront.h \
    wavefrontaveragefilterdlg.h \
    wavefrontfilterdlg.h \
    wftexaminer.h \
    wftstats.h \
    zernikedlg.h \
    zernikeeditdlg.h \
    zernikepolar.h \
    zernikeprocess.h \
    zernikes.h \
    zernikesmoothingdlg.h

FORMS += arbitrarywavefronthelp.ui \
    annulushelpdlg.ui \
    astigpolargraph.ui \
    astigstatsdlg.ui \
    averagewavefrontfilesdlg.ui \
    batchigramwizard.ui \
    bathastigdlg.ui \
    camcalibrationreviewdlg.ui \
    cameracalibwizard.ui \
    camwizardpage1.ui \
    ccswappeddlg.ui \
    colorchanneldisplay.ui \
    colormapviewerdlg.ui \
    contourrulerparams.ui \
    contourtools.ui \
    contourview.ui \
    counterrotationdlg.ui \
    defocusdlg.ui \
    dftarea.ui \
    dftthumb.ui \
    dfttools.ui \
    edgeplot.ui \
    filteroutlinesdlg.ui \
    foucaultview.ui \
    generatetargetdlg.ui \
    helpdlg.ui \
    hotkeysdlg.ui \
    igramintensity.ui \
    jitteroutlinedlg.ui \
    mainwindow.ui \
    metricsdisplay.ui \
    mirrordlg.ui \
    nullmarginhelpdlg.ui \
    nullvariationdlg.ui \
    oglrendered.ui \
    outlinedialog.ui \
    outlinehelpdocwidget.ui \
    outlineplots.ui \
    outlinestatsdlg.ui \
    pdfcalibrationdlg.ui \
    percentcorrectiondlg.ui \
    pixelstats.ui \
    profilearea.ui \
    profileplot.ui \
    psfplot.ui \
    psi_dlg.ui \
    psiphasedisplay.ui \
    psitiltoptions.ui \
    regionedittools.ui \
    rejectedwavefrontsdlg.ui \
    renamewavefrontdlg.ui \
    reportdlg.ui \
    reviewwindow.ui \
    rotationdlg.ui \
    savewavedlg.ui \
    settings2.ui \
    settingsdebug.ui \
    settingsdft.ui \
    settingsGeneral2.ui \
    settingsigram.ui \
    settingsigramimportconfig.ui \
    settingsprofile.ui \
    showaliasdlg.ui \
    showallcontoursdlg.ui \
    simigramdlg.ui \
    simulationsview.ui \
    standastigwizard.ui \
    statsview.ui \
    subtractwavefronatsdlg.ui \
    surface3dcontrolsdlg.ui \
    surfaceanalysistools.ui \
    transformwavefrontdlg.ui \
    unwraperrorsview.ui \
    usercolormapdlg.ui \
    userdrawnprofiledlg.ui \
    vortexdebug.ui \
    wavefrontaveragefilterdlg.ui \
    wavefrontfilterdlg.ui \
    wavefrontnulldlg.ui \
    wftexaminer.ui \
    zernikedlg.ui \
    zernikeeditdlg.ui \
    zernikesmoothingdlg.ui

DISTFILES += buildingDFTFringe64.txt \
    ColorMaps/Dale1.cmp \
    ColorMaps/Dale2.cmp \
    ColorMaps/Dale3.cmp \
    ColorMaps/Diverging_BrBG.cmp \
    ColorMaps/Diverging_bwr.cmp \
    ColorMaps/Diverging_coolwarm.cmp \
    ColorMaps/Diverging_PiYG.cmp \
    ColorMaps/Diverging_PRGn.cmp \
    ColorMaps/Diverging_PuOr.cmp \
    ColorMaps/Diverging_RdBu.cmp \
    ColorMaps/Diverging_RdGy.cmp \
    ColorMaps/Diverging_RdYlBu.cmp \
    ColorMaps/Diverging_RdYlGn.cmp \
    ColorMaps/Diverging_seismic.cmp \
    ColorMaps/Diverging_Spectral.cmp \
    ColorMaps/Miscellaneous_brg.cmp \
    ColorMaps/Miscellaneous_CMRmap.cmp \
    ColorMaps/Miscellaneous_cubehelix.cmp \
    ColorMaps/Miscellaneous_flag.cmp \
    ColorMaps/Miscellaneous_gist_earth.cmp \
    ColorMaps/Miscellaneous_gist_ncar.cmp \
    ColorMaps/Miscellaneous_gist_rainbow.cmp \
    ColorMaps/Miscellaneous_gist_stern.cmp \
    ColorMaps/Miscellaneous_gnuplot.cmp \
    ColorMaps/Miscellaneous_gnuplot2.cmp \
    ColorMaps/Miscellaneous_hsv.cmp \
    ColorMaps/Miscellaneous_jet.cmp \
    ColorMaps/Miscellaneous_nipy_spectral.cmp \
    ColorMaps/Miscellaneous_ocean.cmp \
    ColorMaps/Miscellaneous_prism.cmp \
    ColorMaps/Miscellaneous_rainbow.cmp \
    ColorMaps/Miscellaneous_terrain.cmp \
    "ColorMaps/Perceptually Uniform Sequential_inferno.cmp" \
    "ColorMaps/Perceptually Uniform Sequential_magma.cmp" \
    "ColorMaps/Perceptually Uniform Sequential_plasma.cmp" \
    "ColorMaps/Perceptually Uniform Sequential_viridis.cmp" \
    ColorMaps/Qualitative_Accent.cmp \
    ColorMaps/Qualitative_Dark2.cmp \
    ColorMaps/Qualitative_Paired.cmp \
    ColorMaps/Qualitative_Pastel1.cmp \
    ColorMaps/Qualitative_Pastel2.cmp \
    ColorMaps/Qualitative_Set1.cmp \
    ColorMaps/Qualitative_Set2.cmp \
    ColorMaps/Qualitative_Set3.cmp \
    "ColorMaps/Sequential (2)_afmhot.cmp" \
    "ColorMaps/Sequential (2)_autumn.cmp" \
    "ColorMaps/Sequential (2)_bone.cmp" \
    "ColorMaps/Sequential (2)_cool.cmp" \
    "ColorMaps/Sequential (2)_copper.cmp" \
    "ColorMaps/Sequential (2)_gist_heat.cmp" \
    "ColorMaps/Sequential (2)_gray.cmp" \
    "ColorMaps/Sequential (2)_hot.cmp" \
    "ColorMaps/Sequential (2)_pink.cmp" \
    "ColorMaps/Sequential (2)_spring.cmp" \
    "ColorMaps/Sequential (2)_summer.cmp" \
    "ColorMaps/Sequential (2)_winter.cmp" \
    ColorMaps/Sequential_Blues.cmp \
    ColorMaps/Sequential_BuGn.cmp \
    ColorMaps/Sequential_BuPu.cmp \
    ColorMaps/Sequential_GnBu.cmp \
    ColorMaps/Sequential_Greens.cmp \
    ColorMaps/Sequential_Greys.cmp \
    ColorMaps/Sequential_Oranges.cmp \
    ColorMaps/Sequential_OrRd.cmp \
    ColorMaps/Sequential_PuBu.cmp \
    ColorMaps/Sequential_PuBuGn.cmp \
    ColorMaps/Sequential_PuRd.cmp \
    ColorMaps/Sequential_Purples.cmp \
    ColorMaps/Sequential_RdPu.cmp \
    ColorMaps/Sequential_Reds.cmp \
    ColorMaps/Sequential_YlGn.cmp \
    ColorMaps/Sequential_YlGnBu.cmp \
    ColorMaps/Sequential_YlOrBr.cmp \
    ColorMaps/Sequential_YlOrRd.cmp \
    ColorMaps/spring.cmp \
    COPYING.LESSER.txt \
    COPYING.txt \
    helptext.txt \
    README.md \
    RevisionHistory.html





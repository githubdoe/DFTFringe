#-------------------------------------------------
#
# Project created by QtCreator 2014-08-11T00:35:19
#
#-------------------------------------------------

QT += network \
      xml \
      multimedia \
      multimediawidgets \
      widgets
QT += concurrent widgets
QT += gui webkitwidgets\

qtHaveModule(printsupport): QT += printsupport
QT       += core gui
QT       += opengl widgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
QT += opengl
TARGET = DFTFringe
TEMPLATE = app

CONFIG += ``
SOURCES += main.cpp\
    mainwindow.cpp \
    igramarea.cpp \
    circleoutline.cpp \
    gplus.cpp \
    Boundary.cpp \
    graphicsutilities.cpp \
    dfttools.cpp \
    dftarea.cpp \
    outlinetools.cpp \
    profileplot.cpp \
    wavefront.cpp \
    contourplot.cpp \
    contourtools.cpp \
    glwidget.cpp \
    dftcolormap.cpp \
    oglcontrols.cpp \
    surfaceanalysistools.cpp \
    prefsdlg.cpp \
    surfacemanager.cpp \
    zernikedlg.cpp \
    zernikeprocess.cpp \
    mirrordlg.cpp \
    zernikes.cpp \
    metricsdisplay.cpp \
    reviewwindow.cpp \
    statsdlg.cpp \
    wavefrontloader.cpp \
    rotationdlg.cpp \
    wftstats.cpp \
    surfacepropertiesdlg.cpp \
    imagehisto.cpp \
    colorchanneldisplay.cpp \
    intensityplot.cpp \
    igramintensity.cpp \
    dftthumb.cpp \
    vortexdebug.cpp \
    simigramdlg.cpp \
    punwrap.cpp \
    wftexaminer.cpp \
    savewavedlg.cpp \
    usercolormapdlg.cpp \
    colormapviewerdlg.cpp \
    oglview.cpp \
    settingsigram.cpp \
    settings2.cpp \
    settingsdft.cpp \
    settingsdebug.cpp \
    contourview.cpp \
    simulationsview.cpp \
    psfplot.cpp \
    standastigwizard.cpp \
    counterrotationdlg.cpp \
    renamewavefrontdlg.cpp \
    subtractwavefronatsdlg.cpp \
    helpdlg.cpp \
    settingsprofile.cpp \
    batchigramprocessingdlg.cpp \
    batchigramwizard.cpp
    punwrap.cpp

HEADERS  += mainwindow.h \
    igramarea.h \
    circleoutline.h \
    Boundary.h \
    gplus.h \
    graphicsutilities.h \
    dfttools.h \
    dftarea.h \
    outlinetools.h \
    profileplot.h \
    wavefront.h \
    contourplot.h \
    contourtools.h \
    glwidget.h \
    dftcolormap.h \
    oglcontrols.h \
    surfaceanalysistools.h \
    prefsdlg.h \
    surfacemanager.h \
    zernikedlg.h \
    zernikeprocess.h \
    mirrordlg.h \
    zernikes.h \
    metricsdisplay.h \
    reviewwindow.h \
    vortex.h \
    statsdlg.h \
    wavefrontstats.h \
    wavefrontloader.h \
    rotationdlg.h \
    wftstats.h \
    surfacepropertiesdlg.h \
    punwrap.h \
    imagehisto.h \
    colorchanneldisplay.h \
    intensityplot.h \
    igramintensity.h \
    dftthumb.h \
    vortexdebug.h \
    simigramdlg.h \
    wftexaminer.h \
    savewavedlg.h \
    usercolormapdlg.h \
    colormapviewerdlg.h \
    oglview.h \
    settingsigram.h \
    settings2.h \
    settingsdft.h \
    settingsdebug.h \
    contourview.h \
    simulationsview.h \
    psfplot.h \
    standastigwizard.h \
    counterrotationdlg.h \
    renamewavefrontdlg.h \
    subtractwavefronatsdlg.h \
    helpdlg.h \
    settingsprofile.h \
    batchigramprocessingdlg.h \
    batchigramwizard.h

FORMS    += mainwindow.ui \
    dfttools.ui \
    dftarea.ui \
    outlinetools.ui \
    profilearea.ui \
    profileplot.ui \
    contourtools.ui \
    oglcontrols.ui \
    surfaceanalysistools.ui \
    prefsdlg.ui \
    zernikedlg.ui \
    mirrordlg.ui \
    wavefrontnulldlg.ui \
    metricsdisplay.ui \
    reviewwindow.ui \
    statsdlg.ui \
    rotationdlg.ui \
    surfacepropertiesdlg.ui \
    vortextools.ui \
    colorchanneldisplay.ui \
    igramintensity.ui \
    dftthumb.ui \
    vortexdebug.ui \
    simigramdlg.ui \
    wftexaminer.ui \
    savewavedlg.ui \
    usercolormapdlg.ui \
    colormapviewerdlg.ui \
    settingsigram.ui \
    settings2.ui \
    settingsdft.ui \
    settingsdebug.ui \
    contourview.ui \
    simulationsview.ui \
    psfplot.ui \
    standastigwizard.ui \
    counterrotationdlg.ui \
    renamewavefrontdlg.ui \
    subtractwavefronatsdlg.ui \
    helpdlg.ui \
    settingsprofile.ui \
    batchigramprocessingdlg.ui \
    batchigramwizard.ui

CONFIG( debug, debug|release ) {
    # debug
    LIBS += C:/qwt-6.1.2/lib/qwtd.dll
} else {
    # release
    LIBS += C:/build-qwt-Desktop_Qt_5_3_MinGW_32bit-Release/lib/qwt.dll
}
INCLUDEPATH += C:\\qwt-6.1.2\\src



INCLUDEPATH += c:\opencv\build\include
LIBS += C:/opencv/build-mingw/bin/*.dll



OTHER_FILES += \
    todo.txt

RESOURCES += \
    DFTResources.qrc
RC_FILE = DFTFringe.rc


# The application version
VERSION = 1.2

# Define the preprocessor macro to get the application version in our application.
DEFINES += APP_VERSION=\\\"$$VERSION\\\"

DISTFILES += \
    helptext.txt \
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
    ColorMaps/Perceptually Uniform Sequential_inferno.cmp \
    ColorMaps/Perceptually Uniform Sequential_magma.cmp \
    ColorMaps/Perceptually Uniform Sequential_plasma.cmp \
    ColorMaps/Perceptually Uniform Sequential_viridis.cmp \
    ColorMaps/Qualitative_Accent.cmp \
    ColorMaps/Qualitative_Dark2.cmp \
    ColorMaps/Qualitative_Paired.cmp \
    ColorMaps/Qualitative_Pastel1.cmp \
    ColorMaps/Qualitative_Pastel2.cmp \
    ColorMaps/Qualitative_Set1.cmp \
    ColorMaps/Qualitative_Set2.cmp \
    ColorMaps/Qualitative_Set3.cmp \
    ColorMaps/Sequential (2)_afmhot.cmp \
    ColorMaps/Sequential (2)_autumn.cmp \
    ColorMaps/Sequential (2)_bone.cmp \
    ColorMaps/Sequential (2)_cool.cmp \
    ColorMaps/Sequential (2)_copper.cmp \
    ColorMaps/Sequential (2)_gist_heat.cmp \
    ColorMaps/Sequential (2)_gray.cmp \
    ColorMaps/Sequential (2)_hot.cmp \
    ColorMaps/Sequential (2)_pink.cmp \
    ColorMaps/Sequential (2)_spring.cmp \
    ColorMaps/Sequential (2)_summer.cmp \
    ColorMaps/Sequential (2)_winter.cmp \
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
    RevisionHistory.html

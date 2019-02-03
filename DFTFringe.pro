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
    batchigramwizard.cpp \
    outlinehelpdocwidget.cpp \
    statsview.cpp \
    jitteroutlinedlg.cpp \
    nullvariationdlg.cpp \
    ccswappeddlg.cpp \
    foucaultview.cpp \
    squareimage.cpp \
    bathastigdlg.cpp \
    zernikeeditdlg.cpp \
    settingsgeneral2.cpp \
    nullmarginhelpdlg.cpp \
    plotcolor.cpp \
    cameracalibwizard.cpp \
    camwizardpage1.cpp \
    camcalibrationreviewdlg.cpp \
    generatetargetdlg.cpp \
    lensetablemodel.cpp \
    unwraperrorsview.cpp \
    lensdialog.cpp \
    singleapplication.cpp \
    messagereceiver.cpp \
    myutils.cpp \
    pixelstats.cpp \
    utilil.cpp \
    circlefit.cpp \
    astigstatsdlg.cpp \
    averagewavefrontfilesdlg.cpp \
    astigzoomer.cpp \
    astigscatterplot.cpp \
    wavefrontfilterdlg.cpp \
    myplotpicker.cpp \
    testplotclass.cpp \
    rmsplot.cpp \
    regionedittools.cpp \
    reportdlg.cpp \
    videosetupdlg.cpp \
    showaliasdlg.cpp \
    colorchannel.cpp \
    wavefrontaveragefilterdlg.cpp \
    rejectedwavefrontsdlg.cpp \
    outlinestatsdlg.cpp \
    filteroutlinesdlg.cpp \
    outlineplots.cpp \
    transformwavefrontdlg.cpp \
    showallcontoursdlg.cpp \
    psi_dlg.cpp
    punwrap.cpp

HEADERS  += mainwindow.h \
    igramarea.h \
    circleoutline.h \
    gplus.h \
    graphicsutilities.h \
    dfttools.h \
    dftarea.h \
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
    batchigramwizard.h \
    outlinehelpdocwidget.h \
    statsview.h \
    jitteroutlinedlg.h \
    nullvariationdlg.h \
    ccswappeddlg.h \
    foucaultview.h \
    squareimage.h \
    bathastigdlg.h \
    zernikeeditdlg.h \
    settingsgeneral2.h \
    nullmarginhelpdlg.h \
    plotcolor.h \
    cameracalibwizard.h \
    camwizardpage1.h \
    camcalibrationreviewdlg.h \
    generatetargetdlg.h \
    lensetablemodel.h \
    unwraperrorsview.h \
    lensdialog.h \
    singleapplication.h \
    singleapplication_p.h \
    messagereceiver.h \
    boundary.h \
    myutils.h \
    pixelstats.h \
    utils.h \
    circleutils.h \
    circle.h \
    astigstatsdlg.h \
    averagewavefrontfilesdlg.h \
    astigzoomer.h \
    astigscatterplot.h \
    wavefrontfilterdlg.h \
    myplotpicker.h \
    testplotclass.h \
    rmsplot.h \
    regionedittools.h \
    reportdlg.h \
    videosetupdlg.h \
    showaliasdlg.h \
    colorchannel.h \
    wavefrontaveragefilterdlg.h \
    rejectedwavefrontsdlg.h \
    outlinestatsdlg.h \
    filteroutlinesdlg.h \
    outlineplots.h \
    transformwavefrontdlg.h \
    showallcontoursdlg.h \
    psi_dlg.h
FORMS    += mainwindow.ui \
    dfttools.ui \
    dftarea.ui \
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
    rotationdlg.ui \
    surfacepropertiesdlg.ui \
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
    batchigramwizard.ui \
    outlinehelpdocwidget.ui \
    statsview.ui \
    jitteroutlinedlg.ui \
    nullvariationdlg.ui \
    ccswappeddlg.ui \
    foucaultview.ui \
    bathastigdlg.ui \
    zernikeeditdlg.ui \
    settingsgeneral2.ui \
    nullmarginhelpdlg.ui \
    cameracalibwizard.ui \
    camwizardpage1.ui \
    camcalibrationreviewdlg.ui \
    generatetargetdlg.ui \
    unwraperrorsview.ui \
    lensdialog.ui \
    pixelstats.ui \
    astigstatsdlg.ui \
    averagewavefrontfilesdlg.ui \
    wavefrontfilterdlg.ui \
    regionedittools.ui \
    reportdlg.ui \
    videosetupdlg.ui \
    showaliasdlg.ui \
    wavefrontaveragefilterdlg.ui \
    rejectedwavefrontsdlg.ui \
    outlinestatsdlg.ui \
    filteroutlinesdlg.ui \
    outlineplots.ui \
    transformwavefrontdlg.ui \
    showallcontoursdlg.ui \
    psi_dlg.ui

win32 {
      CONFIG( debug, debug|release ) {
        # debug
        LIBS += C:/qwt-6.1.2/lib/qwtd.dll
      } else {
        # release
        LIBS += C:/build-qwt-Desktop_Qt_5_3_MinGW_32bit-Release/lib/qwt.dll
      }
      INCLUDEPATH += C:\\qwt-6.1.2\\src
      INCLUDEPATH += c:\\opencv\\build\\include
      LIBS += C:/opencv/build-mingw/bin/*.dll
      message("using win32")
      }

unix {
     INCLUDEPATH += /usr/include/qwt
     iLIBS += -lqwt-qt5
     LIBS += -lopencv_core
     LIBS += -lopencv_imgproc
     LIBS += -lopencv_highgui
     LIBS += -lGLU
     LIBS += -lopencv_calib3d
     LIBS += -lopencv_features2d
     LIBS += -lopencv_videoio
     LIBS += -lopencv_imgcodecs
     LIBS += -lqwt-qt5
     message("using linux")
     contains(CONFIG,debug) { message("no extra debug libraries") }
}

OTHER_FILES += \
    todo.txt

RESOURCES += \
    DFTResources.qrc
RC_FILE = DFTFringe.rc
QMAKE_CXXFLAGS += -std=c++11

# The application version
VERSION = 4.0

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
    RevisionHistory.html \
    README.md \
    res/Help/igram.png \
    res/Help/igramScreen.png \
    res/Help/manyrotations.png \
    res/Help/MirrorConfig.png \
    res/Help/help.html \
    res/Help/mirrorConfig.html \
    res/Help/smoothing.htm \
    res/Help/Gaussian10.png \
    res/Help/oneIgramanalysis.png \
    res/Help/BathInterferometry.odt \
    res/Help/analysis.png \
    res/Help/dft.png \
    res/Help/igramSettingsLens.png \
    res/Help/lineGrid.png \
    res/Help/artificialNull.htm \
    res/Help/fringecontrol.htm \
    res/Help/lensDistort.html \
    res/Help/unwrap_errors.html \
    res/Help/chessGrid.png \
    res/Help/circles.png \
    res/Help/PSIHelp.html \
    res/Help/astigsummary.html \
    res/Help/astigSummary.png \
    res/Help/badStandSummary.png

    TRANSLATIONS    = dftfringe_fr.ts

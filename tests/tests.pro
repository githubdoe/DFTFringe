TEMPLATE = app
TARGET = tst_circlefit
QT = core testlib
CONFIG += console testcase
CONFIG -= app_bundle
greaterThan(QT_MAJOR_VERSION, 5) {
    CONFIG += c++17
} else {
    CONFIG += c++14
}

INCLUDEPATH += ..
HEADERS += ../circlefit.h
SOURCES += ../circlefit.cpp \
           tst_circlefit.cpp

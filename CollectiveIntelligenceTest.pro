QT     += core gui printsupport
CONFIG += c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CollectiveIntelligenceTest

TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += main.cpp \
    core/Actor.cpp \
    core/ActorsBase.cpp \
    core/Certification.cpp \
    core/GroupActor.cpp \
    core/Random.cpp \
    core/Utils.cpp \
    gui/QCustomPlot.cc \
    gui/TableActorsBase.cpp \
    gui/GraphPlotter.cpp \
    gui/MainWindow.cpp

HEADERS += \
    core/Actor.h \
    core/ActorsBase.h \
    core/Certification.h \
    core/Defines.h \
    core/GroupActor.h \
    core/Random.h \
    core/Utils.h \
    gui/QCustomPlot.h \
    gui/TableActorsBase.h \
    gui/GraphPlotter.h \
    gui/MainWindow.h

FORMS += \
    gui/MainWindow.ui

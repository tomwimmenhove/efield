#-------------------------------------------------
#
# Project created by QtCreator 2020-03-17T19:35:56
#
#-------------------------------------------------

QT       += core gui xml
CONFIG += c++14

QMAKE_CXXFLAGS += -fopenmp
QMAKE_LFLAGS += -fopenmp -lstdc++

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = efield
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwindow.cpp \
    model/simulator.cpp \
    heatmap.cpp \
    model/floatsurface.cpp \
    model/gradientsurface.cpp \
    mouseeventlabel.cpp \
    visualizer/visualizer.cpp \
    visualizer/heatlegendwidget.cpp \
    viewmodel/mainvm.cpp \
    model/simulatorworker.cpp \
    util/geometry.cpp \
    pointinputdialog.cpp \
    model/project.cpp

HEADERS += \
        mainwindow.h \
    model/simulator.h \
    model/surface.h \
    model/floatsurfacedrawer.h \
    model/floatsurface.h \
    model/gradientsurface.h \
    mouseeventlabel.h \
    heatlegendwidget.h \
    util/ivaluestepper.h \
    util/simplevaluestepper.h \
    visualizer/visualizer.h \
    visualizer/heatmap.h \
    viewmodel/mainvm.h \
    model/simulatorworker.h \
    graphics/lineelement.h \
    graphics/sharednode.h \
    graphics/sharedint.h \
    graphics/drawingelement.h \
    graphics/sceneelement.h \
    graphics/drawing.h \
    graphics/idrawer.h \
    graphics/nodeelement.h \
    util/geometry.h \
    util/refcounted.h \
    pointinputdialog.h \
    graphics/sceneserializevisitor.h \
    graphics/drawingelementvisitor.h \
    graphics/scenedeserializevisitor.h \
    model/project.h

FORMS += \
        mainwindow.ui \
    pointinputdialog.ui

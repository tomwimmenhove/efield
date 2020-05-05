#-------------------------------------------------
#
# Project created by QtCreator 2020-03-17T19:35:56
#
#-------------------------------------------------

QT       += core gui xml
CONFIG += c++14

QMAKE_CXXFLAGS += -fopenmp -Wall -Werror
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
    model/simulator.cpp \
    model/floatsurface.cpp \
    model/gradientsurface.cpp \
    visualizer/visualizer.cpp \
    viewmodel/mainvm.cpp \
    model/simulatorworker.cpp \
    util/geometry.cpp \
    model/project.cpp \
    viewmodel/mouseoperations/mouseoperation.cpp \
    viewmodel/mouseoperations/newnodemouseoperation.cpp \
    viewmodel/mouseoperations/normalmouseoperation.cpp \
    viewmodel/editdrawingelementvisitor.cpp \
    viewmodel/mouseoperations/dragmouseoperation.cpp \
    util/undo/undostack.cpp \
    util/undo/moveundoitem.cpp \
    util/undo/deletenodeundoitem.cpp \
    util/undo/deletelineundoitem.cpp \
    viewmodel/deletedrawingelementvisitor.cpp \
    util/undo/newnodeundoitem.cpp \
    viewmodel/mouseoperations/selectionmouseoperation.cpp \
    util/undo/compositundoitem.cpp \
    util/undo/compositundonamegenerator.cpp \
    view/mainwindow.cpp \
    view/heatmap.cpp \
    view/mouseeventlabel.cpp \
    view/pointinputdialog.cpp \
    view/heatlegendwidget.cpp \
    viewmodel/copydrawingelementvisitor.cpp \
    viewmodel/elementdependencyvisitor.cpp \
    viewmodel/elementmanipulators.cpp \
    util/undo/deletecircleundoitem.cpp \
    util/undo/circlevalueundoitem.cpp

HEADERS += \
    model/simulator.h \
    model/surface.h \
    model/floatsurfacedrawer.h \
    model/floatsurface.h \
    model/gradientsurface.h \
    util/ivaluestepper.h \
    util/simplevaluestepper.h \
    visualizer/visualizer.h \
    viewmodel/mainvm.h \
    model/simulatorworker.h \
    graphics/lineelement.h \
    graphics/sharednode.h \
    graphics/drawingelement.h \
    graphics/sceneelement.h \
    graphics/drawing.h \
    graphics/idrawer.h \
    graphics/nodeelement.h \
    util/geometry.h \
    util/refcounted.h \
    graphics/sceneserializevisitor.h \
    graphics/drawingelementvisitor.h \
    graphics/scenedeserializevisitor.h \
    model/project.h \
    util/derefiterator.h \
    viewmodel/mouseoperations/mouseoperation.h \
    viewmodel/mouseoperations/newnodemouseoperation.h \
    viewmodel/mouseoperations/newlinemouseoperation.h \
    viewmodel/mouseoperations/normalmouseoperation.h \
    viewmodel/mouseoperations/mouseoperations.h \
    viewmodel/editdrawingelementvisitor.h \
    viewmodel/mouseoperations/dragmouseoperation.h \
    util/undo/undostack.h \
    util/undo/moveundoitem.h \
    util/undo/linevalueundoitem.h \
    util/undo/deletenodeundoitem.h \
    util/undo/deletelineundoitem.h \
    viewmodel/deletedrawingelementvisitor.h \
    util/undo/newlineundoitem.h \
    util/undo/newnodeundoitem.h \
    util/undo/undoitem.h \
    viewmodel/mouseoperations/selectionmouseoperation.h \
    util/undo/compositundoitem.h \
    util/undo/compositundonamegenerator.h \
    view/mainwindow.h \
    view/heatlegendwidget.h \
    view/mouseeventlabel.h \
    view/pointinputdialog.h \
    view/heatmap.h \
    viewmodel/copydrawingelementvisitor.h \
    viewmodel/elementdependencyvisitor.h \
    viewmodel/elementmanipulators.h \
    graphics/circleelement.h \
    util/undo/deletecircleundoitem.h \
    util/undo/valueundoitem.h \
    util/undo/circlevalueundoitem.h \
    util/undo/newtwonodeundoitem.h \
    util/undo/newcircleundoitem.h \
    viewmodel/mouseoperations/newtwonodemousepperation.h \
    viewmodel/mouseoperations/newcirclemouseoperation.h

FORMS += \
    view/mainwindow.ui \
    view/pointinputdialog.ui

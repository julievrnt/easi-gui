QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    src/Connectors/connectorbase.cpp \
    src/Connectors/connectorline.cpp \
    src/Connectors/connectorlineparentwidget.cpp \
    src/Connectors/inputs/inputconnector.cpp \
    src/Connectors/outputs/outputconnector.cpp \
    src/Nodes/Builders/includenode.cpp \
    src/Nodes/Builders/layeredmodelnode.cpp \
    src/Nodes/Extra/functionnode.cpp \
    src/Nodes/Extra/layeredmodelnodenode.cpp \
    src/Nodes/Filters/anynode.cpp \
    src/Nodes/Filters/axisalignedcuboidaldomainfilternode.cpp \
    src/Nodes/Filters/groupfilternode.cpp \
    src/Nodes/Filters/sphericaldomainfilternode.cpp \
    src/Nodes/Filters/switchnode.cpp \
    src/Nodes/Maps/affinemapnode.cpp \
    src/Nodes/Maps/andersonianstressnode.cpp \
    src/Nodes/Maps/asaginode.cpp \
    src/Nodes/Maps/constantmapnode.cpp \
    src/Nodes/Maps/evalmodelnode.cpp \
    src/Nodes/Maps/functionmapnode.cpp \
    src/Nodes/Maps/identitymapnode.cpp \
    src/Nodes/Maps/luamapnode.cpp \
    src/Nodes/Maps/optimalstressnode.cpp \
    src/Nodes/Maps/polynomialmapnode.cpp \
    src/Nodes/Maps/scecfilenode.cpp \
    src/Nodes/Extra/affinematrixnode.cpp \
    src/Nodes/Extra/polynomialmatrixnode.cpp \
    src/Nodes/Extra/translationnode.cpp \
    src/Nodes/Extra/switchcomponentnode.cpp \
    src/easigraphicsview.cpp \
    src/main.cpp \
    src/mainwindow.cpp \
    src/Nodes/nodebase.cpp \
    src/Nodes/nodeparentwidget.cpp \
    src/Nodes/rootnode.cpp \
    src/widgetshandler.cpp

HEADERS += \
    src/Connectors/connectorbase.h \
    src/Connectors/connectorline.h \
    src/Connectors/connectorlineparentwidget.h \
    src/Connectors/inputs/inputconnector.h \
    src/Connectors/outputs/outputconnector.h \
    src/Nodes/Builders/includenode.h \
    src/Nodes/Builders/layeredmodelnode.h \
    src/Nodes/Extra/functionnode.h \
    src/Nodes/Extra/layeredmodelnodenode.h \
    src/Nodes/Filters/anynode.h \
    src/Nodes/Filters/axisalignedcuboidaldomainfilternode.h \
    src/Nodes/Filters/groupfilternode.h \
    src/Nodes/Filters/sphericaldomainfilternode.h \
    src/Nodes/Filters/switchnode.h \
    src/Nodes/Maps/affinemapnode.h \
    src/Nodes/Maps/andersonianstressnode.h \
    src/Nodes/Maps/asaginode.h \
    src/Nodes/Maps/constantmapnode.h \
    src/Nodes/Maps/evalmodelnode.h \
    src/Nodes/Maps/functionmapnode.h \
    src/Nodes/Maps/identitymapnode.h \
    src/Nodes/Maps/luamapnode.h \
    src/Nodes/Maps/optimalstressnode.h \
    src/Nodes/Maps/polynomialmapnode.h \
    src/Nodes/Maps/scecfilenode.h \
    src/Nodes/Extra/affinematrixnode.h \
    src/Nodes/Extra/polynomialmatrixnode.h \
    src/Nodes/Extra/translationnode.h \
    src/Nodes/Extra/switchcomponentnode.h \
    src/easigraphicsview.h \
    src/mainwindow.h \
    src/Nodes/nodebase.h \
    src/Nodes/nodeparentwidget.h \
    src/Nodes/rootnode.h \
    src/widgetshandler.h

FORMS += \
    src/mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

unix:!macx: LIBS += -L$$PWD/../../../../usr/local/lib/ -lyaml-cpp

INCLUDEPATH += $$PWD/../../../../usr/local/include
DEPENDPATH += $$PWD/../../../../usr/local/include

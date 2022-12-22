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
    src/Connectors/outputs/mathsoutputconnector.cpp \
    src/Connectors/outputs/outputconnector.cpp \
    src/Nodes/Maps/affinemapnode.cpp \
    src/Nodes/Maps/constantmapnode.cpp \
    src/main.cpp \
    src/mainwindow.cpp \
    src/Nodes/nodebase.cpp \
    src/Nodes/nodeparentwidget.cpp \
    src/Nodes/rootnode.cpp \
    src/Nodes/rootnodedialog.cpp

HEADERS += \
    src/Connectors/connectorbase.h \
    src/Connectors/connectorline.h \
    src/Connectors/connectorlineparentwidget.h \
    src/Connectors/inputs/inputconnector.h \
    src/Connectors/outputs/mathsoutputconnector.h \
    src/Connectors/outputs/outputconnector.h \
    src/Nodes/Maps/affinemapnode.h \
    src/Nodes/Maps/constantmapnode.h \
    src/mainwindow.h \
    src/Nodes/nodebase.h \
    src/Nodes/nodeparentwidget.h \
    src/Nodes/rootnode.h \
    src/Nodes/rootnodedialog.h \
    src/helpers.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../usr/local/lib/release/ -lyaml-cpp
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../usr/local/lib/debug/ -lyaml-cpp
else:unix: LIBS += -L$$PWD/../../../../usr/local/lib/ -lyaml-cpp

INCLUDEPATH += $$PWD/../../../../usr/local/include
DEPENDPATH += $$PWD/../../../../usr/local/include

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../../../usr/local/lib/release/libyaml-cpp.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../../../usr/local/lib/debug/libyaml-cpp.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../../../usr/local/lib/release/yaml-cpp.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../../../usr/local/lib/debug/yaml-cpp.lib
else:unix: PRE_TARGETDEPS += $$PWD/../../../../usr/local/lib/libyaml-cpp.a

#win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../YAMLcpp/ -llibyaml-cpp.dll
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../YAMLcpp/ -llibyaml-cpp.dll
#else:unix: LIBS += -L$$PWD/../../../../YAMLcpp/ -llibyaml-cpp.dll

#INCLUDEPATH += $$PWD/../../Downloads/yaml-cpp-yaml-cpp-0.7.0/include
#DEPENDPATH += $$PWD/../../Downloads/yaml-cpp-yaml-cpp-0.7.0/include

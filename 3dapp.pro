QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

DEFINES += QT_DEPRECATED_WARNINGS
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000

SOURCES += \
    src/attributebuffer.cpp \
    src/main.cpp \
    src/mainwindow.cpp \
    src/mesh.cpp \
    src/meshmodel.cpp \
    src/meshrenderer.cpp \
    src/meshview.cpp \
    src/pythonmesh.cpp \
    src/pythonplugin.cpp \
    src/shader.cpp \
    src/vao.cpp

HEADERS += \
    src/attributebuffer.h \
    src/linear.h \
    src/mainwindow.h \
    src/mesh.h \
    src/meshmodel.h \
    src/meshrenderer.h \
    src/meshview.h \
    src/pythonmesh.h \
    src/pythonplugin.h \
    src/shader.h \
    src/vao.h

FORMS += \
    src/mainwindow.ui

CONFIG += no_keywords
QMAKE_CXXFLAGS += -fsplit-stack
QMAKE_CXXFLAGS += $$system(python3-config --cflags)
QMAKE_LFLAGS += $$system(python3-config --ldflags --embed)
INCLUDEPATH += $$system(python3-config --includes)

qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc

DISTFILES += \
    plugins/catmullclark.py \
    src/api.py \
    src/fragment.glsl \
    src/vertex.glsl

# Copia i plugin dalla cartella sorgente a quella di build.
copyplugins.commands = $(COPY_DIR) $$PWD/plugins/* $$OUT_PWD/plugins/
first.depends = $(first) copyplugins
export(first.depends)
export(copyplugins.commands)
QMAKE_EXTRA_TARGETS += first copyplugins

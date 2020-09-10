QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    attributebuffer.cpp \
    main.cpp \
    mainwindow.cpp \
    mesh.cpp \
    meshmodel.cpp \
    meshrenderer.cpp \
    meshview.cpp \
    pythonmesh.cpp \
    pythonplugin.cpp \
    shader.cpp \
    vao.cpp

HEADERS += \
    attributebuffer.h \
    linear.h \
    mainwindow.h \
    mesh.h \
    meshmodel.h \
    meshrenderer.h \
    meshview.h \
    pythonmesh.h \
    pythonplugin.h \
    shader.h \
    vao.h

FORMS += \
    mainwindow.ui

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
    api.py \
    fragment.glsl \
    plugins/catmullclark.py \
    vertex.glsl

# Copia i plugin dalla cartella sorgente a quella di build.
copyplugins.commands = $(COPY_DIR) $$PWD/plugins/* $$OUT_PWD/plugins/
first.depends = $(first) copyplugins
export(first.depends)
export(copyplugins.commands)
QMAKE_EXTRA_TARGETS += first copyplugins

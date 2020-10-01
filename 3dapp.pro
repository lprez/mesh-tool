QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

DEFINES += QT_DEPRECATED_WARNINGS
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000

SOURCES += \
    src/attributebuffer.cpp \
    src/framebuffer.cpp \
    src/main.cpp \
    src/mainwindow.cpp \
    src/mesh.cpp \
    src/meshmodel.cpp \
    src/meshrenderer.cpp \
    src/meshview.cpp \
    src/pythonexception.cpp \
    src/pythonmesh.cpp \
    src/pythonplugin.cpp \
    src/shader.cpp \
    src/vao.cpp

HEADERS += \
    src/attributebuffer.h \
    src/framebuffer.h \
    src/linear.h \
    src/mainwindow.h \
    src/mesh.h \
    src/meshmodel.h \
    src/meshrenderer.h \
    src/meshview.h \
    src/pythonexception.h \
    src/pythonmesh.h \
    src/pythonplugin.h \
    src/shader.h \
    src/vao.h

FORMS += \
    src/mainwindow.ui

CONFIG += no_keywords

win32 {
    QMAKE_CXXFLAGS += -I$$(PYTHONPATH)\include -D_USE_MATH_DEFINES
    QMAKE_LFLAGS += -L$$(PYTHONPATH) -lpython38
}

unix {
    QMAKE_CXXFLAGS += $$system(python3-config --cflags)
    QMAKE_CXXFLAGS += -Wno-deprecated-copy
    QMAKE_LFLAGS += $$system(python3-config --ldflags --embed)
    INCLUDEPATH += $$system(python3-config --includes)
}

#debug {
#    QMAKE_CXXFLAGS -= -O3
#    QMAKE_CXXFLAGS += -O0
#}

qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc

DISTFILES += \
    README.txt \
    plugins/catmullclark.py \
    plugins/exportobj.py \
    plugins/importobj.py \
    plugins/loopsubdivision.py \
    plugins/subdivision_utils.py \
    src/api.py \
    src/fragment.glsl \
    src/fragment_pick.glsl \
    src/vertex.glsl \
    src/vertex_pick.glsl

UI_DIR = src

# Copia i plugin dalla cartella sorgente a quella di build.
unix {
    copyplugins.commands = $(MKDIR) -p $$OUT_PWD/plugins/ ; $(COPY_DIR) $$PWD/plugins/* $$OUT_PWD/plugins/
}

first.depends = $(first) copyplugins
export(first.depends)
export(copyplugins.commands)
QMAKE_EXTRA_TARGETS += first copyplugins


win32 {
    CONFIG(debug,debug|release) {
        dlls.path = $${OUT_PWD}/debug
    } else {
        dlls.path = $${OUT_PWD}/release
    }
    dlls.files += $$[QT_INSTALL_BINS]/Qt5Core.dll
    dlls.files += $$[QT_INSTALL_BINS]/Qt5Gui.dll
    dlls.files += $$[QT_INSTALL_BINS]/Qt5Widgets.dll
    dlls.files += $$[QT_INSTALL_BINS]/libEGL.dll
    INSTALLS += dlls
}

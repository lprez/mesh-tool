#include <GL/gl.h>
#include <QOpenGLContext>
#include <QOpenGLFunctions>

#include "meshview.h"
#include "shader.h"
#include "mesh.h"
#include "meshrenderer.h"

// TODO togliere
#include <QTextStream>
#include <QFile>

MeshView::MeshView(QWidget *parent) : QOpenGLWidget(parent)
{
}

MeshView::~MeshView() {
    delete test;
    delete renderer;
}

void MeshView::setModel(MeshModel *model)
{
    this->model = model;
    connect(model, &MeshModel::meshChanged, this, &MeshView::updateMesh);
}

void MeshView::updateMesh()
{
    renderer->update(model->getMesh());
    update();
}

void MeshView::initializeGL()
{
    QSurfaceFormat format;

    format.setDepthBufferSize(24);
    //format.setStencilBufferSize(8);
    format.setVersion(4, 5);
    format.setProfile(QSurfaceFormat::CoreProfile);

    setFormat(format);


    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();

    f->glClearColor(0.2, 0.2, 0.2, 1);
    f->glEnable(GL_DEPTH_TEST);

    QFile vsfile(":/glsl/src/vertex.glsl");
    vsfile.open(QFile::ReadOnly);
    QTextStream vsstream(&vsfile);
    QString vsstring = vsstream.readAll();
    vsfile.close();

    QFile fsfile(":/glsl/src/fragment.glsl");
    fsfile.open(QFile::ReadOnly);
    QTextStream fsstream(&fsfile);
    QString fsstring = fsstream.readAll();
    fsfile.close();


    test = new ShaderProgram(
                vsstring.toLocal8Bit().constData(),
                fsstring.toLocal8Bit().constData()
                );


    renderer = new MeshRenderer(model->getMesh(), *test);
}

void MeshView::resizeGL(int width, int height)
{
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();

    f->glViewport(0, 0, width, height);
}

void MeshView::paintGL()
{
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();

    f->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    renderer->render();
}


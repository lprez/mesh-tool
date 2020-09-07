#include <GL/gl.h>
#include <QOpenGLContext>
#include <QOpenGLFunctions>

#include "renderwidget.h"
#include "shader.h"
#include "mesh.h"
#include "meshrenderer.h"

// TODO togliere
#include <Python.h>
#include <QTextStream>
#include <QFile>
#include <iostream>
#include "pythonplugin.h"
#include "pythonmesh.h"

RenderWidget::RenderWidget(QWidget *parent) : QOpenGLWidget(parent)
{

}

RenderWidget::~RenderWidget() {
    delete test;
    delete test_mesh;
}

void RenderWidget::updateMesh()
{
    test_mesh->update(model->getMesh());
}

void RenderWidget::initializeGL()
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

    QFile vsfile(":/glsl/vertex.glsl");
    vsfile.open(QFile::ReadOnly);
    QTextStream vsstream(&vsfile);
    QString vsstring = vsstream.readAll();
    vsfile.close();

    QFile fsfile(":/glsl/fragment.glsl");
    fsfile.open(QFile::ReadOnly);
    QTextStream fsstream(&fsfile);
    QString fsstring = fsstream.readAll();
    fsfile.close();


    test = new ShaderProgram(
                vsstring.toLocal8Bit().constData(),
                fsstring.toLocal8Bit().constData()
                );


    std::map<VertexID, Vertex> vertices = {
        {0, Vertex({-1, -1, 0}, {0, 0})},
        {1, Vertex({1, -1, 0}, {0, 0})},
        {2, Vertex({0, 1, 0}, {0, 0})}
    };
    std::map<FaceID, Face> faces = {
        {0, Face(0, 1, 2)}
    };
    Mesh tmesh(vertices, faces);

    this->setModel(new MeshModel(this, tmesh));

    test_mesh = new MeshRenderer(tmesh, *test);

    connect(model, &MeshModel::meshChanged, this, &RenderWidget::updateMesh);
}

void RenderWidget::resizeGL(int width, int height)
{
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();

    f->glViewport(0, 0, width, height);



    if (width > 1000) {
        PythonPlugin plugin("testplugin.py");
        std::wcout << plugin.name() << std::endl;
        std::function<Mesh(const Mesh&)> transformer = [&plugin](const Mesh& mesh) {return plugin.run(mesh);};
        model->transform(transformer);
    }
}

void RenderWidget::paintGL()
{
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();

    f->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    test_mesh->render();
}


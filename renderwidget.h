#ifndef RENDERWIDGET_H
#define RENDERWIDGET_H

#include <QWidget>
#include <QOpenGLWidget>
#include "shader.h"
#include "meshmodel.h"
#include "meshrenderer.h"


class RenderWidget : public QOpenGLWidget
{
public:
    RenderWidget(QWidget *parent = 0);
    ~RenderWidget();
    void setModel(MeshModel *model) { this->model = model; }

public Q_SLOTS:
    void updateMesh();

protected:
    void initializeGL();
    void resizeGL(int width, int height);
    void paintGL();

private:
    MeshModel *model;

    // TODO togliere
    ShaderProgram *test;
    MeshRenderer *test_mesh;
};

#endif // RENDERWIDGET_H

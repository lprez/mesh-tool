#ifndef RENDERWIDGET_H
#define RENDERWIDGET_H

#include <QWidget>
#include <QOpenGLWidget>
#include "shader.h"
#include "meshmodel.h"
#include "meshrenderer.h"

// Un Widget che rappresenta lo spazio 3D in cui viene visualizzata la mesh.

class MeshView : public QOpenGLWidget
{
public:
    MeshView(QWidget *parent = 0);
    ~MeshView();
    void setModel(MeshModel *model);

public Q_SLOTS:
    void updateMesh();

protected:
    void initializeGL();
    void resizeGL(int width, int height);
    void paintGL();

private:
    MeshModel *model;
    MeshRenderer *renderer;

    // TODO togliere
    ShaderProgram *test;
};

#endif // RENDERWIDGET_H

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
    void set_model(MeshModel *model);

public Q_SLOTS:
    void update_mesh();

protected:
    void initializeGL() override;
    void resizeGL(int width, int height) override;
    void paintGL() override;

    void wheelEvent(QWheelEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    MeshModel *model;
    MeshRenderer *renderer;
    BasicShaderProgram *shader;

    // Parametri della prospettiva
    float near_plane = 0.25, far_plane = 10000, fov = 90, aspect_ratio = 1;
    // Parametri della vista
    Vec3 eye{{0, 0, 0}}, target{{0, 0, 0}}, up{{0, 1, 0}};

    float distance = 1;
    bool rotating = false;
    QPointF camera_rotation{M_PI / 2, M_PI / 2};
    QPoint rotation_start;
    QPointF camera_rotation_start;

    // Aggiorna le matrici di trasformazione, vista e prospettiva
    void update_matrices();
};

#endif // RENDERWIDGET_H

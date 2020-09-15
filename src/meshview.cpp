#include <GL/gl.h>
#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QWheelEvent>

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
    delete shader;
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
    format.setSwapBehavior(QSurfaceFormat::SwapBehavior::DoubleBuffer);

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


    shader = new BasicShaderProgram(
                vsstring.toLocal8Bit().constData(),
                fsstring.toLocal8Bit().constData(),
                "model_matrix",
                "view_matrix",
                "proj_matrix"
                );

    renderer = new MeshRenderer(model->getMesh(), *shader);
}

void MeshView::resizeGL(int width, int height)
{
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();

    aspect_ratio = ((float) width) / ((float) height);

    f->glViewport(0, 0, width, height);
}

void MeshView::paintGL()
{
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();

    update_matrices();
    f->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    renderer->render();
}

void MeshView::wheelEvent(QWheelEvent *event)
{
    QPoint pixels = event->pixelDelta();
    QPoint degrees = event->angleDelta();

    if (!pixels.isNull()) {
        distance -= ((float) pixels.y()) / 10;
    } else if (!degrees.isNull()) {
        distance -= ((float) degrees.y()) / 100;
    }

    //distance = std::max<float>(0, distance);
    printf("s%f", distance);
    update();

    event->accept();
}

void MeshView::mousePressEvent(QMouseEvent *event)
{
    // TODO: cambiare con right
    if (event->button() == Qt::MouseButton::LeftButton) {
        rotating = true;
        rotation_start = event->pos();
        camera_rotation_start = camera_rotation;
        event->accept();
    } else {
        event->ignore();
    }
}

void MeshView::mouseMoveEvent(QMouseEvent *event)
{
    if (rotating) {
        QPoint delta = rotation_start - event->pos();

        camera_rotation = camera_rotation_start - ((QPointF) delta) / 500;
        update();
    } else {
        event->ignore();
    }
}

void MeshView::mouseReleaseEvent(QMouseEvent *event)
{
    // TODO: cambiare con right
    if (event->button() == Qt::MouseButton::LeftButton) {
        rotating = false;
        event->accept();
    } else {
        event->ignore();
    }
}

void MeshView::update_matrices()
{
    float phi = camera_rotation.x(),
          theta = camera_rotation.y(),
          dx = sin(theta) * cos(phi),
          dz = sin(theta) * sin(phi),
          dy = cos(theta),
          len = exp(distance);
    Vec3 rot_eye({dx * len, dy * len, dz * len});
    printf("%f, %f, %f\n", dx, dy, dz);

    shader->set_model_matrix(Matrix<float, 4, 4>::translation({0, 0, 0}) * Matrix<float, 4, 4>::scale(1));
    shader->set_view_matrix(look_at4(eye + rot_eye, target, up));
    shader->set_proj_matrix(perspective4(near, far, fov, aspect_ratio));
}


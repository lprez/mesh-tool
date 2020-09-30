
#include <GL/gl.h>
#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QWheelEvent>

#include "meshview.h"
#include "shader.h"
#include "mesh.h"
#include "meshrenderer.h"

#include <QTextStream>
#include <QFile>

MeshView::MeshView(QWidget *parent) : QOpenGLWidget(parent)
{
}

MeshView::~MeshView() {
    delete shader;
    delete renderer;
}

void MeshView::set_model(MeshModel *model)
{
    this->model = model;
    connect(model, &MeshModel::mesh_changed, this, &MeshView::update_mesh);
}

void MeshView::update_mesh()
{
    renderer->update(model->get_mesh());
    update();
}

void MeshView::initializeGL()
{
    // Seleziona la configurazione del framebuffer e della versione di OpenGL
    QSurfaceFormat format;

    format.setDepthBufferSize(24);
    //format.setStencilBufferSize(8);
    format.setVersion(4, 5);
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setSwapBehavior(QSurfaceFormat::SwapBehavior::DoubleBuffer);

    setFormat(format);


    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();

    // Colore di sfondo
    f->glClearColor(0.2, 0.2, 0.2, 1);
    // Depth test
    f->glEnable(GL_DEPTH_TEST);

    // Carica il sorgente del vertex shader
    QFile vsfile(":/glsl/src/vertex.glsl");
    vsfile.open(QFile::ReadOnly);
    QTextStream vsstream(&vsfile);
    QString vsstring = vsstream.readAll();
    vsfile.close();

    // Carica il sorgente del fragment shader
    QFile fsfile(":/glsl/src/fragment.glsl");
    fsfile.open(QFile::ReadOnly);
    QTextStream fsstream(&fsfile);
    QString fsstring = fsstream.readAll();
    fsfile.close();

    // Compila gli shader
    shader = new BasicShaderProgram(
                vsstring.toLocal8Bit().constData(),
                fsstring.toLocal8Bit().constData(),
                "model_matrix",
                "view_matrix",
                "proj_matrix"
                );

    renderer = new MeshRenderer(model->get_mesh(), *shader);
}

void MeshView::resizeGL(int width, int height)
{
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();

    // Aggiorna la viewport e l'aspect ratio (necessario per la prospettiva)
    aspect_ratio = ((float) width) / ((float) height);

    f->glViewport(0, 0, width, height);
}

void MeshView::paintGL()
{
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();

    update_matrices();
    // Svuota il framebuffer prima di renderizzare di nuovo
    f->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    renderer->render();
}

void MeshView::wheelEvent(QWheelEvent *event)
{
    QPoint pixels = event->pixelDelta();
    QPoint degrees = event->angleDelta();

    // Implementa lo zoom con la rotellina
    if (!pixels.isNull()) {
        distance -= ((float) pixels.y()) / 10;
    } else if (!degrees.isNull()) {
        distance -= ((float) degrees.y()) / 300;
    }

    update();

    event->accept();
}

void MeshView::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::MouseButton::LeftButton) {
        // Inizia la rotazione quando si clicca, conservando la posizione iniziale
        // del mouse e la rotazione iniziale
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
        // Differenza della posizione del mouse rispetto all'inizio
        QPoint delta = rotation_start - event->pos();

        // Aggiorna la rotazione della vista
        camera_rotation = camera_rotation_start - ((QPointF) delta) / 500;


        // Sposta il cursore dal lato opposto quando si raggiungono i bordi

        if (event->x() < 0) {
            rotation_start = QPoint(width() + event->x(), event->y());
            camera_rotation_start = camera_rotation;
            QCursor::setPos(mapToGlobal(QPoint(width(), event->y())));
        }

        if (event->x() > width()) {
            rotation_start = QPoint(event->x() - width(), event->y());
            camera_rotation_start = camera_rotation;
            QCursor::setPos(mapToGlobal(QPoint(0, event->y())));
        }

        if (event->y() < 0) {
            rotation_start = QPoint(event->x(), height() + event->y());
            camera_rotation_start = camera_rotation;
            QCursor::setPos(mapToGlobal(QPoint(event->x(), height())));
        }

        if (event->y() > height()) {
            rotation_start = QPoint(event->x(), event->y() - height());
            camera_rotation_start = camera_rotation;
            QCursor::setPos(mapToGlobal(QPoint(event->x(), 0)));
        }

        update();
    } else {
        event->ignore();
    }
}

void MeshView::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::MouseButton::LeftButton) {
        // Termina la rotazione al rilascio
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
          // Coordinate sferiche del punto di osservazione
          dx = sin(theta) * cos(phi),
          dz = sin(theta) * sin(phi),
          dy = cos(theta),
          len = exp(distance);
    Vec3 rot_eye({dx * len, dy * len, dz * len});

    shader->set_model_matrix(Matrix<float, 4, 4>::translation({0, 0, 0}) * Matrix<float, 4, 4>::scale(1));
    shader->set_view_matrix(look_at4(eye + rot_eye, target, up));
    shader->set_proj_matrix(perspective4(near_plane, far_plane, fov, aspect_ratio));
}


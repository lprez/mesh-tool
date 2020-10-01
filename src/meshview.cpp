
#include <GL/gl.h>
#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QOpenGLFunctions_4_5_Core>
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
    delete view_shader;
    delete pick_shader;
    delete buffers;
    delete view_renderer;
    delete pick_renderer;
}

void MeshView::set_model(MeshModel *model)
{
    this->model = model;
    connect(model, &MeshModel::mesh_changed, this, &MeshView::update_mesh);
}

void MeshView::update_mesh()
{
    buffers->update(model->get_mesh());
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

    QOpenGLFunctions_4_5_Core *f = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_4_5_Core>();

    // Colore di sfondo
    f->glClearColor(0.2, 0.2, 0.2, 1);
    // Depth test
    f->glEnable(GL_DEPTH_TEST);
    f->glPointSize(1);

    view_shader = load_shaders(":/glsl/src/vertex.glsl", ":/glsl/src/fragment.glsl");
    pick_shader = load_shaders(":/glsl/src/vertex_pick.glsl", ":/glsl/src/fragment_pick.glsl");

    buffers = new MeshBuffers(model->get_mesh());
    view_renderer = new MeshRendererView(*buffers, *view_shader);
    pick_renderer = new MeshRendererPick(*buffers, *pick_shader);
}

void MeshView::resizeGL(int width, int height)
{
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();

    // Aggiorna la viewport e l'aspect ratio (necessario per la prospettiva)
    aspect_ratio = ((float) width) / ((float) height);

    f->glViewport(0, 0, width, height);

    // Il framebuffer non è più valido perché ha le dimensioni sbagliate
    if (pick_framebuffer != nullptr) {
        delete pick_framebuffer;
        pick_framebuffer = nullptr;
        pick_vertex = false;
        moving = false;
    }
}

void MeshView::paintGL()
{
    QOpenGLFunctions_4_5_Core *f = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_4_5_Core>();

    update_matrices();
    // Svuota il framebuffer prima di renderizzare di nuovo
    f->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    view_renderer->render();

    if (pick_vertex) {
        moving = render_pick();
        pick_vertex = false;
    }
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
    if (event->button() == Qt::MouseButton::RightButton) {
        pick_vertex = true;
        move_start = event->pos();
        update();
        event->accept();
    } else if (event->button() == Qt::MouseButton::LeftButton && !moving && !pick_vertex) {
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
    if (moving) {
        model->set_vertex(picked_vertex,
                          Vertex(widget_coords_to_object_space(event->x(), event->y(), window_z_start)));
        model->recalculate_normals();
        event->accept();
    } else if (rotating && !pick_vertex) {
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
    if (moving && event->button() == Qt::MouseButton::RightButton) {
        // Termina lo spostamento al rilascio
        moving = false;
        model->recalculate_normals();
        event->accept();
    } else if (rotating && event->button() == Qt::MouseButton::LeftButton) {
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

    Matrix<float, 4, 4> model_matrix = Matrix<float, 4, 4>::scale(1),
                        view_matrix = look_at4(eye + rot_eye, target, up),
                        proj_matrix = perspective4(near_plane, far_plane, fov, aspect_ratio);

    view_shader->set_model_matrix(model_matrix);
    view_shader->set_view_matrix(view_matrix);
    view_shader->set_proj_matrix(proj_matrix);

    pick_shader->set_model_matrix(model_matrix);
    pick_shader->set_view_matrix(view_matrix);
    pick_shader->set_proj_matrix(proj_matrix);

    mvp_matrix = proj_matrix * view_matrix * model_matrix;
}

// https://www.khronos.org/opengl/wiki/Compute_eye_space_from_window_space#From_XYZ_of_gl_FragCoord
Vec3 MeshView::widget_coords_to_object_space(int x, int y, float window_z)
{
    Matrix<float, 4, 4> inv_mvp = invert4(mvp_matrix);

    float window_x = ((float) x) + 0.5,
          window_y = ((float) height() - y) + 0.5,
          // Coordinate NDC
          Nx = (2 * window_x) / width() - 1,
          Ny = (2 * window_y) / height() - 1,
          Nz = 2 * window_z - 1,
          // Parametri della prospettiva
          T1 = (far_plane + near_plane) / (near_plane - far_plane),
          T2 = (2 * far_plane * near_plane) / (near_plane - far_plane),
          //E1 = -1,
          // Coordinata W nel clip space
          Cw = T2 / (Nz + T1);

    // Coordinate nel clip space
    Vec4 C = { Nx * Cw, Ny * Cw, Nz * Cw, Cw },
         P = {inv_mvp * C};

    return {P[0], P[1], P[2]};
}

BasicShaderProgram *MeshView::load_shaders(const char *vert, const char *frag)
{
    // Carica il sorgente del vertex shader
    QFile vsfile(vert);
    vsfile.open(QFile::ReadOnly);
    QTextStream vsstream(&vsfile);
    QString vsstring = vsstream.readAll();
    vsfile.close();

    // Carica il sorgente del fragment shader
    QFile fsfile(frag);
    fsfile.open(QFile::ReadOnly);
    QTextStream fsstream(&fsfile);
    QString fsstring = fsstream.readAll();
    fsfile.close();

    // Compila gli shader
    return new BasicShaderProgram(
                vsstring.toLocal8Bit().constData(),
                fsstring.toLocal8Bit().constData(),
                "model_matrix",
                "view_matrix",
                "proj_matrix"
                );
}


bool MeshView::render_pick()
{
    // Raggio di selezione dei vertici, dev'essere dispari
    const size_t pick_diameter = 41;
    unsigned int vertex_id_buffer[pick_diameter][pick_diameter];
    const VertexID clear_vertex_id[] = {0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF};
    QOpenGLFunctions_4_5_Core *f = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_4_5_Core>();

    if (move_start.x() < (int) pick_diameter / 2 || move_start.x() > width() - (int) pick_diameter / 2 ||
        move_start.y() < (int) pick_diameter / 2 || move_start.y() > height() - (int) pick_diameter / 2) {
        return false;
    }

    // Crea il framebuffer se non esiste già
    if (pick_framebuffer == nullptr) {
        pick_framebuffer = new Framebuffer(width(), height(), GL_DEPTH_COMPONENT24, {GL_R32UI});
    }

    // Abilita il framebuffer in scrittura
    pick_framebuffer->enable(false);

    f->glClearBufferuiv(GL_COLOR, 0, clear_vertex_id);
    // Il buffer del render pass precedente viene riutilizzato per nascondere i punti che verrebbero
    // nascosti dalle facce. Ciò impedisce di spostare punti non visibili.
    f->glBlitFramebuffer(0, 0, width(), height(), 0, 0, width(), height(), GL_DEPTH_BUFFER_BIT, GL_NEAREST);
    f->glDepthFunc(GL_LEQUAL);
    pick_renderer->render(true);
    f->glDepthFunc(GL_LESS);

    pick_framebuffer->disable(false);

    // Abilita il framebuffer in lettura
    pick_framebuffer->enable(true);
    // Legge il "colore" rosso (ovvero il VertexID)
    pick_framebuffer->read_pixels_red(move_start.x() - pick_diameter / 2,
                                      height() - move_start.y() - pick_diameter / 2,
                                      pick_diameter,
                                      pick_diameter,
                                      (unsigned int *) vertex_id_buffer);

    // Distanza dal vertice più vicino
    int closest_distance = pick_diameter * pick_diameter;
    // Offset del vertice più vicino rispetto al cursore
    int offset_x, offset_y;

    picked_vertex = 0xFFFFFFFF;

    // Cerca il vertice più vicino
    for (int x = - (int) pick_diameter / 2; x <= (int) pick_diameter / 2; x++) {
        for (int y = - (int) pick_diameter / 2; y <= (int) pick_diameter / 2; y++) {
            int distance = x * x + y * y;

            if (vertex_id_buffer[y + pick_diameter / 2][x + pick_diameter / 2] != 0xFFFFFFFF &&
                distance < closest_distance) {
                picked_vertex = vertex_id_buffer[y + pick_diameter / 2][x + pick_diameter / 2];
                offset_x = x;
                offset_y = y;
            }
        }
    }

    if (picked_vertex == 0xFFFFFFFF) {
        // Non c'è nessun vertice vicino
        pick_framebuffer->disable(true);
        return false;
    } else {
        // Legge la profondità (la Z nel window space)
        pick_framebuffer->read_pixels_depth(move_start.x() + offset_x,
                                            height() - move_start.y() + offset_y,
                                            1, 1, &window_z_start);
        pick_framebuffer->disable(true);

        return true;
    }
}


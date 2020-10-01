#ifndef RENDERWIDGET_H
#define RENDERWIDGET_H

#include <QWidget>
#include <QOpenGLWidget>

#include "shader.h"
#include "meshmodel.h"
#include "meshrenderer.h"
#include "framebuffer.h"

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
    MeshBuffers *buffers;
    MeshRendererView *view_renderer;
    BasicShaderProgram *view_shader;
    MeshRendererPick *pick_renderer;
    BasicShaderProgram *pick_shader;

    // Parametri della prospettiva
    float near_plane = 0.25, far_plane = 10000, fov = 90, aspect_ratio = 1;
    // Parametri della vista
    Vec3 eye{{0, 0, 0}}, target{{0, 0, 0}}, up{{0, 1, 0}};

    // Distanza della vista dalla mesh
    float distance = 1.5;

    // Variabili dello stato della rotazione
    bool rotating = false; // Se true, la rotazione è in corso
    QPointF camera_rotation{M_PI / 4, M_PI * 3 / 8};
    QPoint rotation_start; // Coordinate iniziali del puntatore
    QPointF camera_rotation_start; // Rotazione iniziale della vista

    // Variabili dello stato dello spostamento dei vertici
    bool pick_vertex = false; // Se true, al prossimo rendering verrà fatto il 3D picking
    bool moving = false; // Se true, lo spostamento è in corso
    QPoint move_start; // Coordinate iniziali del puntatore
    VertexID picked_vertex; // Vertice scelto per lo spostamento
    float window_z_start; // z iniziale del vertice nel window space
    Framebuffer *pick_framebuffer = nullptr;

    // Moltiplicazione delle matrici di trasformazione e proiezione
    Matrix<float, 4, 4> mvp_matrix;

    // Aggiorna le matrici di trasformazione, vista e prospettiva
    void update_matrices();

    // Converte delle coordinate relative al widget in coordinate
    // locali della mesh.
    Vec3 widget_coords_to_object_space(int x, int y, float window_z);

    // Carica vertex e fragment shader da due file
    BasicShaderProgram *load_shaders(const char *vert, const char *frag);

    // Renderizza dei punti nelle posizioni dei vertici con un valore intero uguale al VertexID. Ciò viene
    // fatto fuori dallo schermo in un altro framebuffer per trovare il VertexID del vertice cliccato
    bool render_pick();
};

#endif // RENDERWIDGET_H

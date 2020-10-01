#ifndef MESHMODEL_H
#define MESHMODEL_H

#include <QObject>
#include "mesh.h"

// Un modello della Mesh che si sta modificando nell'applicazione.
class MeshModel : public QObject
{
    Q_OBJECT
public:
    MeshModel(QObject *parent) : QObject(parent) {}
    MeshModel(QObject *parent, Mesh &initial_mesh) : QObject(parent), mesh(initial_mesh) {}

    // Sostituisce con una nuova mesh
    void replace(const Mesh& new_mesh);

    void set_vertex(VertexID id, Vertex vertex) { mesh.set_vertex(id, vertex); Q_EMIT mesh_changed(); }
    void remove_vertex(VertexID id) { mesh.remove_vertex(id); Q_EMIT mesh_changed(); }

    void set_smooth(bool smooth, bool recalculate_normals = true);
    void recalculate_normals() { mesh.recalculate_normals(); Q_EMIT mesh_changed(); }
    const Mesh& get_mesh() { return mesh; }

Q_SIGNALS:
    void mesh_changed();

private:
    Mesh mesh;
};

#endif // MESHMODEL_H

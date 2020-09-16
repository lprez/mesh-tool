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

    void recalculate_normals() { mesh.recalculate_normals(); }
    const Mesh& getMesh() {return mesh;}

Q_SIGNALS:
    void meshChanged();

private:
    Mesh mesh;
};

#endif // MESHMODEL_H

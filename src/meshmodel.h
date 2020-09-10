#ifndef MESHMODEL_H
#define MESHMODEL_H

#include <QObject>
#include "mesh.h"

// Un modello della Mesh per l'interazione con Qt.

class MeshModel : public QObject
{
    Q_OBJECT
public:
    MeshModel(QObject *parent) : QObject(parent) {}
    MeshModel(QObject *parent, Mesh &initial_mesh) : QObject(parent), mesh(initial_mesh) {}
    void transform(std::function<Mesh (const Mesh&)>& transformer);
    const Mesh& getMesh() {return mesh;}

Q_SIGNALS:
    void meshChanged();

private:
    Mesh mesh;
};

#endif // MESHMODEL_H

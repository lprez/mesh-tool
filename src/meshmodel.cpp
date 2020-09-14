#include "meshmodel.h"

void MeshModel::replace(const Mesh& new_mesh)
{
    this->mesh = new_mesh;
    Q_EMIT meshChanged();
}

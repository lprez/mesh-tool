#include "meshmodel.h"

void MeshModel::replace(const Mesh& new_mesh)
{
    this->mesh = new_mesh;
    Q_EMIT mesh_changed();
}


void MeshModel::set_smooth(bool smooth, bool recalculate_normals)
{
    this->mesh.set_smooth(smooth, recalculate_normals);
    Q_EMIT mesh_changed();
}

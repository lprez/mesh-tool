#include "meshmodel.h"

void MeshModel::transform(std::function<Mesh (const Mesh&)>& transformer)
{
    this->mesh = transformer(this->mesh);
    Q_EMIT meshChanged();
}

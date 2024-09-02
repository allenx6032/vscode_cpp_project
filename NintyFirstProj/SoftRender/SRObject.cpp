#include "SRObject.h"

#include "SRmesh.h"
#include "SRMaterial.h"
#include "SRShader.h"


void Object::transform(const glm::mat4& m) {
	material->shader->setModelMatrix(m);
}
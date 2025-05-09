#include "skity_hw_mesh.hpp"

#include "skity_hw_renderer.hpp"

namespace skity {

size_t HWMesh::AppendVertex(float x, float y, float mix, float u, float v) {
  size_t base = VertexBase();

  raw_vertex_buffer_.emplace_back(x, y, mix, u, v);

  return base;
}

size_t HWMesh::AppendVertex(const HWVertex &vertex) {
  size_t base = VertexBase();

  raw_vertex_buffer_.emplace_back(vertex);

  return base;
}

size_t HWMesh::AppendIndices(const std::vector<uint32_t> &indices) {
  size_t base = IndexBase();

  raw_index_buffer_.insert(raw_index_buffer_.end(), indices.begin(),
                           indices.end());

  return base;
}

void HWMesh::UploadMesh(HWRenderer *renderer) {
  renderer->UploadVertexBuffer(raw_vertex_buffer_.data(),
                               sizeof(HWVertex) * raw_vertex_buffer_.size());

  renderer->UploadIndexBuffer(raw_index_buffer_.data(),
                              sizeof(uint32_t) * raw_index_buffer_.size());
}

void HWMesh::ResetMesh() {
  raw_index_buffer_.clear();
  raw_vertex_buffer_.clear();
}

}  // namespace skity
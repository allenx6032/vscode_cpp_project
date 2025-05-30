#ifndef SKITY_SRC_RENDER_HW_HW_PATH_VISITOR_HPP
#define SKITY_SRC_RENDER_HW_HW_PATH_VISITOR_HPP

#include <skity_paint.hpp>
#include <skity_path.hpp>

#include "skity_hw_geometry_raster.hpp"

namespace skity {

class HWDrawRange;
class HWMesh;

class HWPathVisitor : public HWGeometryRaster {
 public:
  HWPathVisitor(HWMesh* mesh, Paint const& paint, bool use_gs)
      : HWGeometryRaster(mesh, paint, use_gs) {}
  virtual ~HWPathVisitor() = default;

  void VisitPath(Path const& path, bool force_close);

 private:
  void HandleMoveTo(glm::vec2 const& p);
  void HandleLineTo(glm::vec2 const& p1, glm::vec2 const& p2);
  void HandleQuadTo(glm::vec2 const& p1, glm::vec2 const& p2,
                    glm::vec2 const& p3);
  void HandleConicTo(glm::vec2 const& p1, glm::vec2 const& p2,
                     glm::vec2 const& p3, float weight);
  void HandleCubicTo(glm::vec2 const& p1, glm::vec2 const& p2,
                     glm::vec2 const& p3, glm::vec2 const& p4);
  void HandleClose();

 protected:
  glm::vec2 FirstPoint() const { return first_pt_; }

  glm::vec2 PrevDir() const { return prev_dir_; }

  virtual void OnBeginPath() = 0;

  virtual void OnEndPath() = 0;

  virtual void OnMoveTo(glm::vec2 const& p) = 0;

  virtual void OnLineTo(glm::vec2 const& p1, glm::vec2 const& p2) = 0;

  virtual void OnQuadTo(glm::vec2 const& p1, glm::vec2 const& p2,
                        glm::vec2 const& p3) = 0;

 private:
  glm::vec2 first_pt_ = {};
  glm::vec2 prev_dir_ = {};
  glm::vec2 prev_pt_ = {};
};

}  // namespace skity

#endif  // SKITY_SRC_RENDER_HW_HW_PATH_VISITOR_HPP
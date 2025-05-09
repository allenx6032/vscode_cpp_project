#pragma once

#include "apollonia.h"
#include "apollonia_math.h"
#include "apollonia_body.h"
#include "apollonia_collision.h"
#include "apollonia_joint.h"

#include <vector>
#include <map>
#include <mutex>

namespace apollonia {

class World {
 public:
  using BodyList = std::vector<Body*>;
  using JointList = std::vector<Joint*>;
  using ArbiterList = std::map<ArbiterKey, Arbiter*>;

  World(const Vec2& gravity) : gravity_(gravity) {}
  ~World();
  static PolygonBody* NewBox(Float mass, Float width, Float height,
                             const Vec2& position={0, 0});
  static PolygonBody* NewPolygonBody(Float mass, const PolygonBody::VertexList& vertices,
                                     const Vec2& position={0, 0});
  static Arbiter* NewArbiter(Body& a, Body& b, const Vec2& normal,
      const Arbiter::ContactList& contacts=Arbiter::ContactList());
  static RevoluteJoint* NewRevoluteJoint(Body& a, Body& b, const Vec2& anchor);

  void Add(Body* body) { bodies_.push_back(body); }
  void Add(Joint* joint) { joints_.push_back(joint); }
  const Vec2& gravity() const { return gravity_; }
  const BodyList& bodies() const { return bodies_; }
  const JointList& joints() const { return joints_; }

  void Step(Float dt);
  void Clear();
  void Lock() { mutex_.lock(); }
  void Unlock() { mutex_.unlock(); }

 private:
  // TODO(wgtdkp):
  void BroadPhase();
  DISABLE_COPY_AND_ASSIGN(World)

  std::mutex mutex_;

  Vec2 gravity_ {0, 0};
  size_t iterations_ {10};
  BodyList bodies_;
  JointList joints_;
  ArbiterList arbiters_;
};

}

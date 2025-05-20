#ifndef JOINT_H_
#define JOINT_H_
#include "mc_lua_stack.h"
#include <memory>
#include "mc_body.h"
#include "mc_world.h"
namespace dynamics
{
class Joint
{
public:
  Joint() = default;
  Joint(lua::Stack& lua, Body& body_a, Body& body_b, World& world);
  explicit operator bool() const;
private:
  std::shared_ptr<class JointImpl> impl_;
};
}
#endif
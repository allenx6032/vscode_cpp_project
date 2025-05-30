#include "mc_world_impl.h"
#include "mc_body_impl.h"
#include <vector>
#include "mc_exception.h"
#include "mc_log.h"
#include "SDL_stdinc.h"
namespace 
{
auto Radians(double degrees) -> float32
{
  static const double scale = M_PI / 180.;
  return static_cast<float32>(degrees * scale);
}

typedef std::unique_ptr<b2Shape> Shape;
typedef std::pair<Shape, float32> ShapePair;

auto Box(dynamics::WorldImpl const& world, lua::Stack& lua) -> ShapePair
{
  double angle = 0.;
  {
    lua::Guard guard = lua.Field("angle");
    if(lua.Check())
    {
      lua.Pop(angle);
    }
  }

  float32 width = world.Metres(lua.Field<double>(3));
  float32 height = world.Metres(lua.Field<double>(4));

  ShapePair shape;
  shape.first = Shape(new b2PolygonShape);
  b2PolygonShape& box = *dynamic_cast<b2PolygonShape*>(shape.first.get());
  box.SetAsBox(.5f * width, .5f * height, b2Vec2(world.Metres(lua.Field<double>(1)), world.Metres(lua.Field<double>(2))), Radians(angle));

  shape.second = width * height;
  return shape;
}

auto Circle(dynamics::WorldImpl const& world, lua::Stack& lua) -> ShapePair
{
  ShapePair shape;
  shape.first = Shape(new b2CircleShape);
  b2CircleShape& circle = *dynamic_cast<b2CircleShape*>(shape.first.get());
  circle.m_p.Set(world.Metres(lua.Field<double>(1)), world.Metres(lua.Field<double>(2)));
  circle.m_radius = world.Metres(lua.Field<double>(3));
  shape.second = circle.m_radius * circle.m_radius * b2_pi;
  return shape;
}

auto Chain(dynamics::WorldImpl const& world, lua::Stack& lua) -> ShapePair
{
  std::vector<b2Vec2> vertices;
  for(int index = 1, end = lua.Size(); index <= end; ++index)
  {
    lua::Guard guard = lua.Field(index);
    vertices.emplace_back(world.Metres(lua.Field<double>(1)), world.Metres(lua.Field<double>(2)));
  }

  ShapePair shape;
  shape.first = Shape(new b2ChainShape);
  b2ChainShape& chain = *dynamic_cast<b2ChainShape*>(shape.first.get());

  if(vertices.front() == vertices.back())
  {
    vertices.pop_back();
    chain.CreateLoop(vertices.data(), vertices.size());
  }
  else
  {
    chain.CreateChain(vertices.data(), vertices.size());
  }
  shape.second = 0.f;
  return shape;
}

auto BodyDefinition(dynamics::WorldImpl const& world, lua::Stack& lua, float32 damping, float32 x, float32 y, bool allow_rotation, float32 angle) -> b2BodyDef
{
  b2BodyDef body_def;
 
  lua::Guard guard = lua.Field("velocity");
  if(lua.Check())
  {
    body_def.type = b2_dynamicBody;
    body_def.linearVelocity.Set(world.Metres(lua.Field<double>(1)), world.Metres(lua.Field<double>(2)));
  }
  else
  {
    body_def.type = b2_staticBody;
    body_def.linearVelocity.SetZero();
  }

  body_def.position.Set(x, y);
  body_def.angle = angle;
  body_def.angularVelocity = 0.f;
  body_def.linearDamping = damping;
  body_def.angularDamping = 0.f;
  body_def.fixedRotation = !allow_rotation;
  return body_def;
}

auto FixtureDefinition(float32 area, float32 mass, float32 friction, float32 restitution, bool sensor) -> b2FixtureDef
{
  b2FixtureDef fixture;
  float32 density = 0.f;
  if(area > 0.f)
  {
    density = mass / area;
  }
  fixture.density = density;
  if(mass == 0.f)
  {
    fixture.isSensor = true;
    fixture.friction = 0.f;
    fixture.restitution = 0.f;
  }
  else
  {
    fixture.isSensor = sensor;
    fixture.friction = friction;
    fixture.restitution = restitution;
  }
  return fixture;
}

auto Position(b2Body const& body) -> b2Vec3
{
  b2Vec2 xy = body.GetPosition();
  return b2Vec3(xy.x, xy.y, body.GetAngle());
}

auto Velocity(b2Body const& body) -> b2Vec3
{
  b2Vec2 xy = body.GetLinearVelocity();
  return b2Vec3(xy.x, xy.y, body.GetAngularVelocity());
}

auto Degrees(float32 radians) -> double
{
  static const double scale = 180. / M_PI;
  return static_cast<double>(radians) * scale;
}
}

namespace dynamics
{
BodyImpl::BodyImpl(lua::Stack& lua, World& world)
{
  float32 area = 0.f;
  std::vector<Shape> shapes;

  {
    lua::Guard guard = lua.Field("shapes");
    int size = lua.Size();
    shapes.reserve(size);

    for(int index = 1; index <= size; ++index)
    {
      lua::Guard guard = lua.Field(index);

      std::string type = lua.Field<std::string>("type");
    
      Shape shape;
      if(type == "box")
      {
        lua::Guard guard = lua.Field("shape");
        float32 temp;
        std::tie(shape, temp) = Box(*world.impl_, lua);
        area += temp;
      }
      else if(type == "circle")
      {
        lua::Guard guard = lua.Field("shape");
        float32 temp;
        std::tie(shape, temp) = Circle(*world.impl_, lua);
        area += temp;
      }
      else if(type == "chain")
      {
        lua::Guard guard = lua.Field("shape");
        float32 temp;
        std::tie(shape, temp) = Chain(*world.impl_, lua);
        area += temp;
      }
      else
      {
        BOOST_THROW_EXCEPTION(exception::Exception());
      }
      
      shapes.emplace_back(std::move(shape));
    }
  }

  float32 drag = lua.Field<float>("drag");

  double x, y;
  {
    lua::Guard guard = lua.Field("position");
    x = lua.Field<double>(1);
    y = lua.Field<double>(2);
  }

  bool rotation = false;
  {
    lua::Guard guard = lua.Field("allow_rotation");
    if(lua.Check())
    {
      lua.Pop(rotation);
    }
  }

  double angle = 0.;
  {
    lua::Guard guard = lua.Field("angle");
    if(lua.Check())
    {
      lua.Pop(angle);
    }
  }

  b2BodyDef body_def = BodyDefinition(*world.impl_, lua, drag, world.impl_->Metres(x), world.impl_->Metres(y), rotation, Radians(angle));

  body_ = world.impl_->world_.CreateBody(&body_def);
  body_->SetUserData(this);

  bool sensor = false;
  {
    lua::Guard guard = lua.Field("sensor");
    if(lua.Check())
    {
      lua.Pop(sensor);
    }
  }

  float mass = 0.f;
  {
    lua::Guard guard = lua.Field("mass");
    if(lua.Check())
    {
      lua.Pop(mass);
    }
  }

  float friction = 0.f;
  {
    lua::Guard guard = lua.Field("friction");
    if(lua.Check())
    {
      lua.Pop(friction);
    }
  }

  float restitution = 0.f;
  {
    lua::Guard guard = lua.Field("restitution");
    if(lua.Check())
    {
      lua.Pop(restitution);
    }
  }

  b2FixtureDef fixture = FixtureDefinition(area, mass, friction, restitution, sensor);

  for(auto& shape : shapes)
  {
    fixture.shape = shape.get();
    body_->CreateFixture(&fixture);
  }

  body_->ResetMassData();

  world_ = world;
  position_ = ::Position(*body_);
  velocity_ = ::Velocity(*body_);

  {
    lua::Guard guard = lua.Field("light");
    if(lua.Check())
    {
      light_ = Light(lua);
    }
  }
}

auto BodyImpl::MakeBody(b2Body* body_ptr) -> Body
{
  Body body;
  if(body_ptr)
  {
    BodyImpl* impl = static_cast<BodyImpl*>(body_ptr->GetUserData());
    if(impl)
    {
      body.impl_ = impl->shared_from_this();
    }
  }
  return body;
}

auto BodyImpl::Position() const -> game::Position 
{
  auto world = world_.Lock().impl_;
  return game::Position(world->Pixels(position_.x), world->Pixels(position_.y));
}

auto BodyImpl::Position(float x, float y) -> void
{
  auto world = world_.Lock().impl_;
  body_->SetTransform(b2Vec2(world->Metres(x), world->Metres(y)), body_->GetAngle());
}

auto BodyImpl::Rotation() const -> double
{
  return Degrees(position_.z);
}

auto BodyImpl::Rotation(double angle) -> void
{
  body_->SetTransform(body_->GetPosition(), Radians(angle));
}

auto BodyImpl::Velocity() const -> game::Position
{
  auto world = world_.Lock().impl_;
  return game::Position(world->Pixels(velocity_.x), world->Pixels(velocity_.y));
}

auto BodyImpl::Velocity(float x, float y) -> void
{
  auto world = world_.Lock().impl_;
  body_->SetLinearVelocity(b2Vec2(world->Metres(x), world->Metres(y)));
}

auto BodyImpl::Force(float x, float y) -> void
{
  auto world = world_.Lock().impl_;
  body_->ApplyForceToCenter(b2Vec2(world->Metres(x), world->Metres(y)), true);
}

auto BodyImpl::Impulse(float x, float y) -> void
{
  auto world = world_.Lock().impl_;
  body_->ApplyLinearImpulse(b2Vec2(world->Metres(x), world->Metres(y)), body_->GetWorldCenter(), true);
}

auto BodyImpl::Begin() -> void
{
  if(body_->GetType() != b2_staticBody)
  {
    position_ = ::Position(*body_);
    velocity_ = ::Velocity(*body_);
  }
}

auto BodyImpl::End(float32 dt) -> void
{
  if(body_->GetType() != b2_staticBody)
  {
    b2Vec3 v = ::Velocity(*body_) + velocity_;
    b2Vec3 dx = ::Position(*body_) - position_;
    cubic_[3] = dt * v - 2.f * dx;
    cubic_[2] = -dt * (v + velocity_) + 3.f * dx;
    cubic_[1] = dt * velocity_;
    cubic_[0] = position_;
  }
}

auto BodyImpl::Update(float32 ds) -> void
{
  if(body_->GetType() != b2_staticBody)
  {
    position_ = ds * (ds * (ds * cubic_[3] + cubic_[2]) + cubic_[1]) + cubic_[0];
    velocity_ = ds * (ds * 3.f * cubic_[3] + 2.f * cubic_[2]) + cubic_[1];
  }
}

auto BodyImpl::Modulation() const -> display::Modulation
{
  return display::Modulation(light_.illumination.x, light_.illumination.y, light_.illumination.z, 1.f);
}

auto BodyImpl::Emit(float r, float g, float b) -> void
{
  light_.emission.Set(r, g, b);
}

auto BodyImpl::Intrinsic(float r, float g, float b) -> void
{
  light_.intrinsic.Set(r, g, b);
}

BodyImpl::~BodyImpl()
{
  try
  { 
    if(world_.Lock())
    {
      body_->SetUserData(nullptr);
      body_->GetWorld()->DestroyBody(body_);
    }
  }
  catch(...)
  {
    exception::Log("Swallowed exception");
  }
}

auto Body::operator<(Body const& other) const -> bool
{
  return impl_.owner_before(other.impl_);
}

Body::operator bool() const
{
  return static_cast<bool>(impl_) && static_cast<bool>(impl_->world_.Lock());
}

auto Body::Position() const -> game::Position
{
  return impl_->Position();
}

auto Body::Position(float x, float y) -> void
{
  impl_->Position(x, y);
}

auto Body::Rotation() const -> double
{
  return impl_->Rotation();
}

auto Body::Rotation(double angle) -> void
{
  impl_->Rotation(angle);
}

auto Body::Velocity() const -> game::Position
{
  return impl_->Velocity();
}

auto Body::Velocity(float x, float y) -> void
{
  impl_->Velocity(x, y);
}

auto Body::Force(float x, float y) -> void
{
  impl_->Force(x, y);
}

auto Body::Impulse(float x, float y) -> void
{
  impl_->Impulse(x, y);
}

auto Body::Modulation() const -> display::Modulation
{
  return impl_->Modulation();
}

auto Body::Emit(float r, float g, float b) -> void
{
  impl_->Emit(r, g, b);
}

auto Body::Intrinsic(float r, float g, float b) -> void
{
  impl_->Intrinsic(r, g, b);
}

Body::Body(lua::Stack& lua, World& world) : impl_(std::make_shared<BodyImpl>(lua, world))
{
}
}
#include "deceptus_boxcollider.h"

#include "deceptus_tmxobject.h"
#include "deceptus_fixturenode.h"
#include "deceptus_gamemechanismdeserializerregistry.h"

namespace
{
const auto registered_boxcollider = []
{
   auto& registry = GameMechanismDeserializerRegistry::instance();
   registry.mapGroupToLayer("BoxCollider", "box_colliders");

   registry.registerLayerName(
      "box_colliders",
      [](GameNode* parent, const GameDeserializeData& data, auto& mechanisms)
      {
         auto mechanism = std::make_shared<BoxCollider>(parent);
         mechanism->setup(data);
         mechanisms["box_colliders"]->push_back(mechanism);
      }
   );
   registry.registerObjectGroup(
      "BoxCollider",
      [](GameNode* parent, const GameDeserializeData& data, auto& mechanisms)
      {
         auto mechanism = std::make_shared<BoxCollider>(parent);
         mechanism->setup(data);
         mechanisms["box_colliders"]->push_back(mechanism);
      }
   );
   return true;
}();
}  // namespace

BoxCollider::BoxCollider(GameNode* node) : GameNode(node)
{
   setClassName(typeid(BoxCollider).name());
}

void BoxCollider::setup(const GameDeserializeData& data)
{
   setObjectId(data._tmx_object->_name);

   _size.x = data._tmx_object->_width_px;
   _size.y = data._tmx_object->_height_px;
   _rect = sf::FloatRect{{data._tmx_object->_x_px, data._tmx_object->_y_px}, {data._tmx_object->_width_px, data._tmx_object->_height_px}};

   addChunks(_rect);

   setupBody(data._world);
}

std::optional<sf::FloatRect> BoxCollider::getBoundingBoxPx()
{
   return _rect;
}

void BoxCollider::setupBody(const std::shared_ptr<b2World>& world)
{
   const auto half_size_x = (_size.x / 2) / PPM;
   const auto half_size_y = (_size.y / 2) / PPM;

   b2PolygonShape shape;
   shape.SetAsBox(half_size_x, half_size_y);

   b2BodyDef body_def;
   body_def.type = b2_staticBody;
   _body = world->CreateBody(&body_def);
   _body->SetTransform(b2Vec2((_rect.position.x + _size.x / 2) / PPM, (_rect.position.y + _size.y / 2) / PPM), 0);

   b2FixtureDef fixture_def;
   fixture_def.shape = &shape;
   fixture_def.density = 1.0f;
   fixture_def.friction = 0.3f;
   fixture_def.filter.categoryBits = CategoryBoundary;                    // I am a
   fixture_def.filter.maskBits = CategoryMoveableBox | CategoryBoundary;  // I collide with

   auto* fixture = _body->CreateFixture(&fixture_def);
   auto* object_data = new FixtureNode(this);
   object_data->setType(ObjectTypeObstacle);
   fixture->SetUserData(static_cast<void*>(object_data));
}

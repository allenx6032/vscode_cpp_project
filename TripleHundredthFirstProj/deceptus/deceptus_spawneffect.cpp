#include "deceptus_spawneffect.h"

#include <iostream>
#include "deceptus_sfmlmath.h"
#include "deceptus_tmxproperties.h"
#include "deceptus_animationpool.h"
#include "deceptus_constants.h"
#include "deceptus_texturepool.h"
#include "deceptus_valuereader.h"

namespace
{

float frand(float min, float max)
{
   const auto val = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
   return min + val * (max - min);
};

}  // namespace

SpawnEffect::SpawnEffect(const sf::Vector2f pos_px)
{
   _particles = std::make_unique<ParticleEffect>(
      pos_px, _particle_count, _particle_radius, _show_duration_s, _particle_velocity_min, _particle_velocity_max
   );

   _orb = std::make_unique<Orb>(pos_px, _orb_idle_cycle_count);
}

void SpawnEffect::deserialize(const GameDeserializeData& data)
{
   if (!data._tmx_object->_properties)
   {
      return;
   }

   const auto& map = data._tmx_object->_properties->_map;

   _hide_duration_s = ValueReader::readValue<float>("hide_duration", map).value_or(_default_hide_duration_s);
   _show_duration_s = ValueReader::readValue<float>("show_duration_s", map).value_or(_default_show_duration_s);
   _particle_count = ValueReader::readValue<int32_t>("particle_count", map).value_or(_default_particle_count);
   _particle_radius = ValueReader::readValue<float>("particle_radius", map).value_or(_default_particle_radius);
   _particle_velocity_min = ValueReader::readValue<float>("particle_velocity_min", map).value_or(_default_particle_velocity_min);
   _particle_velocity_max = ValueReader::readValue<float>("particle_velocity_max", map).value_or(_default_particle_velocity_max);
   _orb_idle_cycle_count = ValueReader::readValue<int32_t>("orb_idle_cycle_count", map).value_or(_default_orb_idle_cycle_count);
}

void SpawnEffect::draw(sf::RenderTarget& target)
{
   _particles->draw(target);
   _orb->draw(target);
}

void SpawnEffect::update(const sf::Time& dt)
{
   if (_orb->_step == Orb::Step::Hide)
   {
      _elapsed_hide += dt;
      _particles->_alpha = 1.0f - std::min(_elapsed_hide.asSeconds(), _hide_duration_s) / _hide_duration_s;
      _particles->_respawn = false;
   }
   else
   {
      _elapsed_show += dt;
      _particles->_alpha = std::min(_elapsed_show.asSeconds(), _show_duration_s) / _show_duration_s;
   }

   _particles->update(dt);
   _orb->update(dt);
}

bool SpawnEffect::isFinished() const
{
   return _orb->_animation_hide->_finished && _particles->_alpha < 0.01f;
}

bool SpawnEffect::isShown() const
{
   return _orb->_animation_show->_finished;
}

void SpawnEffect::activate()
{
   _activate = true;
}

bool SpawnEffect::isActive() const
{
   return _activate;
}

SpawnEffect::ParticleEffect::ParticleEffect(
   const sf::Vector2f& offset_px,
   int32_t count,
   float radius_px,
   float show_duration_s,
   float particle_velocity_min,
   float particle_velocity_max
)
{
   // texture: 3000 x 300
   // row 1: 30 * 50 x 50 (show)
   // row 2: 60 * 50 x 50 (idle)
   // row 3: 12 * 100 x 100 (hide)
   _texture = TexturePool::getInstance().get("data/effects/spawn_particles.png");

   _particles.reserve(count);
   for (auto i = 0; i < count; i++)
   {
      Particle particle(*_texture);
      particle._sprite->setOrigin({5, 5});
      particle._offset_px = offset_px;
      particle._radius_px = radius_px;
      particle._show_duration_s = show_duration_s;
      particle._particle_velocity_min = particle_velocity_min;
      particle._particle_velocity_max = particle_velocity_max;
      particle.spawn();
      particle.setupPosition(frand(0.0f, 1.0f));  // at the start spawn from everywhere

      _particles.push_back(std::move(particle));
   }
}

void SpawnEffect::ParticleEffect::draw(sf::RenderTarget& target)
{
   static const sf::RenderStates render_states{sf::BlendAdd};
   std::ranges::for_each(
      _particles,
      [&target](const auto& particle)
      {
         if (particle._delay.asMilliseconds() <= 0 && !particle._dead)
         {
            target.draw(*particle._sprite, render_states);
         }
      }
   );
}

void SpawnEffect::ParticleEffect::update(const sf::Time& dt)
{
   std::ranges::for_each(
      _particles,
      [&dt, this](auto& particle)
      {
         particle._alpha_all_particles = _alpha;
         particle._respawn = _respawn;
         particle.update(dt);
      }
   );
}

bool SpawnEffect::ParticleEffect::allDead() const
{
   return std::ranges::all_of(_particles, [](const auto& particle) { return particle._dead; });
}

void SpawnEffect::Particle::setupPosition(float random_scale)
{
   const auto angle = static_cast<float>(std::rand() % 360) * FACTOR_DEG_TO_RAD;

   const auto x = std::cos(angle);
   const auto y = std::sin(angle);

   _pos_norm.x = x;
   _pos_norm.y = y;

   _scale_px = random_scale * _radius_px;
}

SpawnEffect::Particle::Particle(const sf::Texture& texture)
{
   _sprite = std::make_unique<sf::Sprite>(texture);
}

void SpawnEffect::Particle::spawn()
{
   setupPosition(frand(0.7f, 1.0f));
   _velocity = frand(_particle_velocity_min, _particle_velocity_max);
   _delay = sf::seconds(frand(0.0f, _show_duration_s));

   // each texture rect is 10x10px, 5 particles in 1 row
   _sprite->setTextureRect({{(std::rand() % 5) * 10, 0}, {10, 10}});
}

void SpawnEffect::Particle::update(const sf::Time& dt)
{
   _delay -= dt;
   if (_delay.asMilliseconds() > 50)
   {
      return;
   }

   // respawn condition
   if (_pos_px.x * _pos_px.x + _pos_px.y * _pos_px.y < 0.3f)
   {
      if (_respawn)
      {
         spawn();
      }
      else
      {
         _dead = true;
      }
   }

   // alpha should increase within the first second after spawn or so
   // also alpha should have a static alpha so the overall effect can be activated and deactivated
   const auto alpha_norm = std::min(SfmlMath::length(_pos_norm), 1.0f);
   const auto alpha_norm_squared = alpha_norm * alpha_norm;
   const auto alpha_squared_and_scaled = 255 - (alpha_norm_squared * 255);
   const auto alpha = static_cast<uint8_t>(alpha_squared_and_scaled * _alpha_all_particles);
   _sprite->setColor({255, 255, 255, alpha});

   // compute sprite position
   float speed_factor = 1.0f;
   if (!_respawn)
   {
      speed_factor = 1.0f - alpha_norm_squared;
   }

   _pos_norm = _pos_norm * (1.0f - speed_factor * _velocity * dt.asMilliseconds());
   _pos_px = _pos_norm * _scale_px;
   _sprite->setPosition(_pos_px + _offset_px);
}

SpawnEffect::Orb::Orb(const sf::Vector2f& pos_px, int32_t idle_cycle_count) : _idle_cycle_count(idle_cycle_count)
{
   _texture = TexturePool::getInstance().get("data/effects/spawn_orb.png");

   AnimationPool animation_pool{"data/effects/spawn_effect_animations.json"};

   _animation_show = animation_pool.create("show", pos_px.x, pos_px.y, false, false);
   _animation_idle = animation_pool.create("idle", pos_px.x, pos_px.y, false, false);
   _animation_hide = animation_pool.create("hide", pos_px.x, pos_px.y, false, false);

   _animation_show->seekToStart();
   _animation_idle->seekToStart();
   _animation_hide->seekToStart();

   _animation_idle->_looped = true;

   _animation_show->play();
}

void SpawnEffect::Orb::draw(sf::RenderTarget& target)
{
   if (!_animation_show->_paused)
   {
      _animation_show->draw(target);
   }

   if (!_animation_idle->_paused)
   {
      _animation_idle->draw(target);
   }

   if (!_animation_hide->_paused)
   {
      _animation_hide->draw(target);
   }
}

void SpawnEffect::Orb::update(const sf::Time& dt)
{
   switch (_step)
   {
      case Step::Show:
      {
         if (!_animation_show->_paused)
         {
            _animation_show->update(dt);
         }

         // transition show -> idle
         if (_animation_show->_finished)
         {
            _step = Step::Idle;
            _animation_idle->play();
         }

         break;
      }
      case Step::Idle:
      {
         if (_animation_idle->_loop_count == _idle_cycle_count)
         {
            _animation_idle->pause();
            _step = Step::Hide;
            _animation_hide->play();
         }

         if (!_animation_idle->_paused)
         {
            _animation_idle->update(dt);
         }

         break;
      }
      case Step::Hide:
      {
         if (!_animation_hide->_paused)
         {
            _animation_hide->update(dt);
         }
      }
      break;
   }
}

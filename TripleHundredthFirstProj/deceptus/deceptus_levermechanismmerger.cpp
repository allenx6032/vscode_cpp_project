#include "deceptus_levermechanismmerger.h"

#include "deceptus_tmxproperties.h"
#include "deceptus_tmxproperty.h"
#include "deceptus_conveyorbelt.h"
#include "deceptus_door.h"
#include "deceptus_fan.h"
#include "deceptus_laser.h"
#include "deceptus_lever.h"
#include "deceptus_movingplatform.h"
#include "deceptus_onoffblock.h"
#include "deceptus_rotatingblade.h"
#include "deceptus_spikeblock.h"
#include "deceptus_spikes.h"

namespace
{
std::vector<std::shared_ptr<TmxObject>> __rectangles;

}

void LeverMechanismMerger::addSearchRect(const std::shared_ptr<TmxObject>& rect)
{
   __rectangles.push_back(rect);
}

void LeverMechanismMerger::merge(
   const std::vector<std::shared_ptr<GameMechanism>>& levers,
   const std::vector<std::shared_ptr<GameMechanism>>& lasers,
   const std::vector<std::shared_ptr<GameMechanism>>& platforms,
   const std::vector<std::shared_ptr<GameMechanism>>& fans,
   const std::vector<std::shared_ptr<GameMechanism>>& belts,
   const std::vector<std::shared_ptr<GameMechanism>>& spikes,
   const std::vector<std::shared_ptr<GameMechanism>>& spike_blocks,
   const std::vector<std::shared_ptr<GameMechanism>>& on_off_blocks,
   const std::vector<std::shared_ptr<GameMechanism>>& rotating_blades,
   const std::vector<std::shared_ptr<GameMechanism>>& doors
)
{
   using Callback = std::function<void(int32_t)>;

   std::vector<std::shared_ptr<GameMechanism>> all_mechanism;
   std::copy(lasers.begin(), lasers.end(), std::back_inserter(all_mechanism));
   std::copy(platforms.begin(), platforms.end(), std::back_inserter(all_mechanism));
   std::copy(fans.begin(), fans.end(), std::back_inserter(all_mechanism));
   std::copy(belts.begin(), belts.end(), std::back_inserter(all_mechanism));
   std::copy(spikes.begin(), spikes.end(), std::back_inserter(all_mechanism));
   std::copy(spike_blocks.begin(), spike_blocks.end(), std::back_inserter(all_mechanism));
   std::copy(on_off_blocks.begin(), on_off_blocks.end(), std::back_inserter(all_mechanism));
   std::copy(rotating_blades.begin(), rotating_blades.end(), std::back_inserter(all_mechanism));
   std::copy(doors.begin(), doors.end(), std::back_inserter(all_mechanism));

   for (const auto& tmp : levers)
   {
      auto lever = std::dynamic_pointer_cast<Lever>(tmp);

      // don't go by search rectangle, go by target id
      for (const auto& target_id : lever->getTargetIds())
      {
         const auto target_it = std::find_if(
            all_mechanism.begin(),
            all_mechanism.end(),
            [target_id](const auto& mechanism)
            {
               auto node = std::dynamic_pointer_cast<GameNode>(mechanism);
               return node->getObjectId() == target_id;
            }
         );

         if (target_it != all_mechanism.end())
         {
            auto mechanism = (*target_it);
            auto callback = [mechanism](int32_t state)
            {
               auto enabled = (state != -1);
               mechanism->setEnabled(enabled);
            };
            lever->addCallback(callback);
            lever->updateReceivers();
         }
      }
   }

   // old rectangle approach
   for (const auto& rect : __rectangles)
   {
      sf::FloatRect search_rect;
      search_rect.position.x = rect->_x_px;
      search_rect.position.y = rect->_y_px;
      search_rect.size.x = rect->_width_px;
      search_rect.size.y = rect->_height_px;

      // Log::Info()
      //    << "x: " << searchRect.position.x << " "
      //    << "y: " << searchRect.position.y << " "
      //    << "w: " << searchRect.size.x << " "
      //    << "h: " << searchRect.size.y << " ";

      for (const auto& tmp : levers)
      {
         auto lever = std::dynamic_pointer_cast<Lever>(tmp);

         if (lever->getPixelRect().findIntersection(search_rect).has_value())
         {
            std::vector<Callback> callbacks;

            for (const auto& l : lasers)
            {
               auto mechanism = std::dynamic_pointer_cast<Laser>(l);

               if (mechanism->getPixelRect().findIntersection(search_rect).has_value())
               {
                  callbacks.emplace_back([mechanism](int32_t state) { mechanism->setEnabled(state == -1 ? false : true); });
               }
            }

            for (const auto& b : belts)
            {
               auto mechanism = std::dynamic_pointer_cast<ConveyorBelt>(b);

               if (mechanism->getPixelRect().findIntersection(search_rect).has_value())
               {
                  callbacks.emplace_back([mechanism](int32_t state) { mechanism->setEnabled(state == -1 ? false : true); });
               }
            }

            for (const auto& f : fans)
            {
               auto mechanism = std::dynamic_pointer_cast<Fan>(f);

               if (mechanism->getPixelRect().findIntersection(search_rect).has_value())
               {
                  callbacks.emplace_back([mechanism](int32_t state) { mechanism->setEnabled(state == -1 ? false : true); });
               }
            }

            for (const auto& p : platforms)
            {
               auto mechanism = std::dynamic_pointer_cast<MovingPlatform>(p);

               const auto& pixel_path = mechanism->getPixelPath();

               if (std::any_of(
                      pixel_path.begin(), pixel_path.end(), [&](const auto& pixel) { return (search_rect.contains({pixel.x, pixel.y})); }
                   ))
               {
                  callbacks.emplace_back([mechanism](int32_t state) { mechanism->setEnabled(state == -1 ? false : true); });
               }
            }

            for (const auto& s : spikes)
            {
               auto mechanism = std::dynamic_pointer_cast<Spikes>(s);

               if (mechanism->getPixelRect().findIntersection(search_rect).has_value())
               {
                  callbacks.emplace_back([mechanism](int32_t state) { mechanism->setEnabled(state == -1 ? false : true); });
               }
            }

            for (const auto& s : spike_blocks)
            {
               auto mechanism = std::dynamic_pointer_cast<SpikeBlock>(s);

               if (mechanism->getPixelRect().findIntersection(search_rect).has_value())
               {
                  callbacks.emplace_back([mechanism](int32_t state) { mechanism->setEnabled(state == -1 ? false : true); });
               }
            }

            for (const auto& instance : on_off_blocks)
            {
               auto mechanism = std::dynamic_pointer_cast<OnOffBlock>(instance);

               if (mechanism->getPixelRect().findIntersection(search_rect).has_value())
               {
                  callbacks.emplace_back([mechanism](int32_t state) { mechanism->setEnabled(state == -1 ? false : true); });
               }
            }

            for (const auto& instance : rotating_blades)
            {
               auto mechanism = std::dynamic_pointer_cast<RotatingBlade>(instance);

               if (mechanism->getPixelRect().findIntersection(search_rect).has_value())
               {
                  callbacks.emplace_back([mechanism](int32_t state) { mechanism->setEnabled(state == -1 ? false : true); });
               }
            }

            for (const auto& instance : doors)
            {
               auto mechanism = std::dynamic_pointer_cast<Door>(instance);

               if (mechanism->getPixelRect().findIntersection(search_rect).has_value())
               {
                  callbacks.emplace_back(
                     [mechanism](int32_t state)
                     {
                        if (state == -1)
                        {
                           mechanism->close();
                        }
                        else
                        {
                           mechanism->open();
                        }
                     }
                  );
               }
            }

            // the rect can be configured to enable a lever, too
            // this approach could be deprecated at a later point in time because
            // the standard way should be to configure everything via the lever
            // tmxobject properties
            if (rect->_properties)
            {
               auto enabled_it = rect->_properties->_map.find("enabled");
               if (enabled_it != rect->_properties->_map.end())
               {
                  const auto enabled = enabled_it->second->_value_bool.value();
                  lever->setEnabled(enabled);
               }
            }

            lever->setCallbacks(callbacks);
            lever->updateReceivers();

            break;
         }
      }
   }

   __rectangles.clear();
}

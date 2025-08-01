#include "deceptus_tmxpolygon.h"

#include "deceptus_log.h"
#include "deceptus_tmxtools.h"

#include <iostream>
#include <sstream>

void TmxPolygon::deserialize(tinyxml2::XMLElement* element, const std::shared_ptr<TmxParseData>&)
{
   auto* points = element->Attribute("points");
   const auto pairs = TmxTools::split(points, ' ');

   for (const auto& pair : pairs)
   {
      const auto two_points = TmxTools::splitPair(pair, ',');
      if (two_points.size() == 2)
      {
         const auto x = std::stof(two_points.at(0));
         const auto y = std::stof(two_points.at(1));

         _polyline.emplace_back(x, y);
      }
      else
      {
         Log::Error() << "bad polygon data";
         break;
      }
   }
}

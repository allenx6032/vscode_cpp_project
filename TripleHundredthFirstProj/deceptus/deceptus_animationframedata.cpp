#include "deceptus_animationframedata.h"

#include <cmath>

//   0   1   2   3   4   5   6   7
// +---+---+---+---+---+---+---+---+
// |   |   |   |   |   |###|###|###| 0
// +---+---+---+---+---+---+---+---+
// |###|###|###|   |   |   |   |   | 1
// +---+---+---+---+---+---+---+---+
// |   |   |   |   |   |   |   |   | 2
// +---+---+---+---+---+---+---+---+
//

AnimationFrameData::AnimationFrameData(
   const std::shared_ptr<sf::Texture>& texture,
   const sf::Vector2f& origin,
   int32_t frame_width,
   int32_t frame_height,
   int32_t frame_count,
   int32_t frames_per_row,
   const std::vector<sf::Time>& frame_times,
   int32_t start_frame
)
    : _texture(texture), _origin(origin), _frame_times(frame_times)
{
   for (auto i = start_frame; i < start_frame + frame_count; i++)
   {
      const auto x = (i % frames_per_row);
      const auto y = static_cast<int32_t>((floor(static_cast<float>(i) / frames_per_row)));

      // Log::Info() << this << " x: " << x << " y: " << y;

      _frames.emplace_back(sf::IntRect({x * frame_width, y * frame_height}, {frame_width, frame_height}));
   }
}

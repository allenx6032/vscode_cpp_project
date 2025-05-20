#ifndef FADE_H_
#define FADE_H_
#include <memory>
#include "mc_queue.h"
#include "mc_window.h"
namespace game
{
class Fade
{
public:
  Fade(event::Queue& queue, display::Window const& window);
  auto Down(float period) -> void;
  auto Up(float period) -> void;
  auto Render() const -> void;
  auto Pause() -> void;
  auto Resume() -> void;
  auto Add(event::Command const& command) -> void;
private:
  class Impl;
  std::shared_ptr<Impl> impl_;
};
}
#endif
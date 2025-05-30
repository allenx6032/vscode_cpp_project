#ifndef SCRIPT_H_
#define SCRIPT_H_
#include <memory>
#include <string>
#include "mc_window.h"
#include "mc_weak_ptr.h"
#include "mc_queue.h"
#include <filesystem>
namespace game
{
class Script
{
public:
  Script() = default;
  Script(std::filesystem::path const& file, display::Window& window, event::Queue& queue, std::filesystem::path const& path, float volume);
  auto Pause() -> void;
  auto Resume() -> void;
  auto Render() -> void;
  auto ChoiceUp(int player) -> void;
  auto ChoiceDown(int player) -> void;
  auto ChoiceLeft(int player) -> void;
  auto ChoiceRight(int player) -> void;
  auto ActionLeft(int player, bool state) -> void;
  auto ActionRight(int player, bool state) -> void;
  auto Move(int player, float x, float y) -> void;
  auto Look(int player, float x, float y) -> void;
  auto Add(event::Command const& command) -> void;
  auto Join(int player) -> void;
  auto Leave(int player) -> void;
  explicit operator bool() const;
private:
  class Impl;
  std::shared_ptr<Impl> impl_;
public:
  typedef memory::WeakPtr<Script> WeakPtr;
  friend WeakPtr;
};
}
#endif
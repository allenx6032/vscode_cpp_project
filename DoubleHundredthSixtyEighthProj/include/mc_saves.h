#ifndef SAVES_H_
#define SAVES_H_
#include <string>
#include <memory>
#include <optional>
#include <filesystem>
namespace game
{ 
class Saves
{
public:
  Saves() = default;
  Saves(std::filesystem::path const& file);
  typedef boost::optional<int> Slot;
  auto Playing() const -> bool;
  auto LastPlayed() const -> int;
  auto Size() const -> int;
  auto Progress(int slot) const -> int;
  auto Current(int slot) const -> int;
  auto LastPlayed(int slot) const -> std::string;
  auto Play(int slot) -> void;
  auto Stop() -> void;
  auto Save() -> void;
  auto Current(int slot, int current) -> void;
  auto Delete(int slot) -> void;
  explicit operator bool() const;
private:
  class Impl;
  std::shared_ptr<Impl> impl_;
};
}
#endif
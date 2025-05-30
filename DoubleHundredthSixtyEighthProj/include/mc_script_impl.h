#ifndef SCRIPT_IMPL_H_
#define SCRIPT_IMPL_H_
#include "mc_script.h"
#include "mc_actor.h"
#include "mc_timer.h"
#include "mc_lua_stack.h"
#include "mc_choice.h"
#include <memory>
#include <map>
#include <unordered_set>
#include <string>
#include <set>
#include <array>
#include "mc_subtitle.h"
#include "mc_fade.h"
#include "mc_signal.h"
#include <filesystem>
#include "mc_game_joint.h"
#include "mc_music.h"
#include "mc_sound.h"
#include "mc_timer.h"
#include "mc_lua_data.h"
#include "boost/bimap/bimap.hpp"
#include "boost/bimap/multiset_of.hpp" 
#include "boost/bimap/unordered_set_of.hpp" 
#include <map>
namespace game
{
typedef std::unordered_set<Joint> JointSet;
typedef std::unordered_set<Actor> ActorSet;
typedef std::unordered_set<event::Timer> TimerSet;
typedef std::unordered_set<audio::Sound> SoundSet;
typedef std::unordered_set<audio::Music> MusicSet;
typedef std::array<bool, 2> Paused;
typedef std::array<float, 3> Order;
typedef std::map<int, Choice> Choices;

typedef boost::bimaps::bimap<boost::bimaps::multiset_of<Order>, boost::bimaps::unordered_set_of<Actor>, boost::bimaps::unconstrained_set_of_relation, boost::bimaps::with_info<Position>> Scene;

struct Stage
{
  Paused paused_;
  JointSet joints_;
  ActorSet actors_;
  TimerSet timers_;
  SoundSet sounds_;
  MusicSet music_;
  audio::Music current_music_;
  ActorSet subjects_;
  ActorSet actives_;
  float zoom_;
  double angle_;

  Scene scene_;
  Choices choices_;
  Subtitle subtitle_;
  Subtitle title_;

  dynamics::World world_;
  collision::Group collision_;
};

typedef std::weak_ptr<Stage> WeakStagePtr;
typedef std::shared_ptr<Stage> StagePtr;
typedef std::unordered_set<StagePtr> StageSet;

class Script::Impl final : public std::enable_shared_from_this<Impl>
{
public:
  Impl(display::Window& window, event::Queue& queue, std::filesystem::path const& path, float volume);
  auto Init(std::filesystem::path const& file) -> void;
  
  auto Pause() -> void;
  auto Resume() -> void;
  auto Render() -> void;

  auto Player(int player, std::string const& call) -> void;
  auto Call(std::string const& call) -> void;
  auto View() -> void;
  auto Pause(StagePtr const& stage, bool& paused) -> void;
  auto Pause(StagePtr const& stage) -> bool;
  auto Resume(StagePtr const& stage, bool& paused) -> void;
  auto Add(event::Command const& command) -> void;
  
  auto ChoiceUp(int player) -> void;
  auto ChoiceDown(int player) -> void;
  auto ChoiceLeft(int player) -> void;
  auto ChoiceRight(int player) -> void;
  auto ActionLeft(int player, bool state) -> void;
  auto ActionRight(int player, bool state) -> void;
  auto Move(int player, float x, float y) -> void;
  auto Look(int player, float x, float y) -> void;

  auto Collect() -> void;

  auto ActorInit() -> void;
  auto ActorLoad() -> void;
  auto ActorFree() -> void;
  auto ActorBody() -> void;
  auto ActorEyes() -> void;
  auto ActorMouth() -> void;
  auto ActorPosition() -> void;
  auto ActorVelocity() -> void;
  auto ActorForce() -> void;
  auto ActorImpulse() -> void;
  auto ActorModulation() -> void;
  auto ActorDilation() -> void;
  auto ActorRotation() -> void;
  auto ActorPlane() -> void;
  auto ActorScale() -> void;
  auto ActorLink() -> void;
  auto ActorUnlink() -> void;
  auto ActorEmit() -> void;
  auto ActorIntrinsic() -> void;

  template<class Data> auto StageDataGet() -> std::pair<StagePtr, Data>
  {
    std::pair<WeakStagePtr, Data::WeakPtr> ptr;
    lua::Get(static_cast<lua_State*>(lua_), ptr);
    return std::make_pair(ptr.first.lock(), ptr.second.Lock());
  }

  template<class Data> auto DataGet() -> Data
  {
    std::pair<WeakStagePtr, Data::WeakPtr> ptr;
    lua::Get(static_cast<lua_State*>(lua_), ptr);
    return ptr.second.Lock();
  }

  auto StageInit() -> void;
  auto StageLoad() -> void;
  auto StageFree() -> void;
  auto StageAmbient() -> void;
  auto StageNominate() -> void;
  auto StagePause() -> void;
  auto StageResume() -> void;
  auto StageGet() -> StagePtr;

  auto ChoiceInit() -> void;
  auto ChoiceChoice() -> void;
  auto ChoiceUpModulation() -> void;
  auto ChoiceDownModulation() -> void;
  auto ChoiceLeftModulation() -> void;
  auto ChoiceRightModulation() -> void;

  auto SubtitleInit() -> void;
  auto SubtitleText() -> void;
  auto SubtitleModulation() -> void;
  auto TitleText() -> void;
  auto TitleModulation() -> void;

  auto TimerInit() -> void;
  auto TimerLoad() -> void;
  auto TimerFree() -> void;

  auto ViewInit() -> void;
  auto ViewAddActor() -> void;
  auto ViewActor() -> void;
  auto ViewZoom() -> void;
  auto ViewRotation() -> void;

  auto CollisionInit() -> void;
  auto CollisionBegin() -> void;
  auto CollisionEnd() -> void;
  auto CollisionLink() -> void;
  auto CollisionUnlink() -> void;

  auto FadeInit() -> void;
  auto FadeUp() -> void;
  auto FadeDown() -> void;
  auto FadeEnd() -> void;

  auto JointInit() -> void;
  auto JointLoad() -> void;
  auto JointFree() -> void;

  auto AudioInit() -> void;
  auto SoundLoad() -> void;
  auto SoundFree() -> void;
  auto SoundPlay() -> void;
  auto SoundEnd() -> void;
  auto MusicLoad() -> void;
  auto MusicFree() -> void;
  auto MusicPlay() -> void;
  auto MusicEnd() -> void;

  auto Scale() const -> float;

  auto Join(int player) -> void;
  auto Leave(int player) -> void;

  lua::Stack lua_;
  display::Window window_;
  event::Queue queue_;
  StageSet stages_;
  StagePtr stage_;
  bool paused_;
  Fade fade_;
  event::Signal signal_;
  bool begun_;
  std::filesystem::path path_;
  float volume_;
  event::Timer collect_;
};
}
#endif
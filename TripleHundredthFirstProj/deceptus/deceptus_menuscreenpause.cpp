#include "deceptus_menuscreenpause.h"

#include "deceptus_audio.h"
#include "deceptus_musicplayer.h"
#include "deceptus_gameaudio.h"
#include "deceptus_gamestate.h"
#include "deceptus_messagebox.h"
#include "deceptus_menu.h"
#include "deceptus_menuaudio.h"

MenuScreenPause::MenuScreenPause()
{
   setFilename("data/menus/pause.psd");
}

void MenuScreenPause::update(const sf::Time& /*dt*/)
{
}

void MenuScreenPause::keyboardKeyPressed(sf::Keyboard::Key key)
{
   if (key == sf::Keyboard::Key::Up)
   {
      up();
   }
   else if (key == sf::Keyboard::Key::Down)
   {
      down();
   }
   else if (key == sf::Keyboard::Key::Enter)
   {
      select();
   }
   else if (key == sf::Keyboard::Key::Escape)
   {
      resume();
   }
}

void MenuScreenPause::loadingFinished()
{
   updateLayers();
}

void MenuScreenPause::up()
{
   switch (_selection)
   {
      case Selection::Resume:
         _selection = Selection::Quit;
         break;
      case Selection::Options:
         _selection = Selection::Resume;
         break;
      case Selection::Quit:
         _selection = Selection::Options;
         break;
   }

   updateLayers();
   MenuAudio::play(MenuAudio::SoundEffect::ItemNavigate);
}

void MenuScreenPause::down()
{
   switch (_selection)
   {
      case Selection::Resume:
         _selection = Selection::Options;
         break;
      case Selection::Options:
         _selection = Selection::Quit;
         break;
      case Selection::Quit:
         _selection = Selection::Resume;
         break;
   }

   updateLayers();
   MenuAudio::play(MenuAudio::SoundEffect::ItemNavigate);
}

void MenuScreenPause::resume()
{
   Menu::getInstance()->hide();
   GameState::getInstance().enqueueResume();
   GameAudio::getInstance().play(GameAudio::SoundEffect::GameStateResume);

   MusicPlayer::getInstance().queueTrack(
      {.filename = "data/music/level_test_track_muffler_awakening.ogg",
       .transition = MusicPlayer::TransitionType::Crossfade,
       .duration = std::chrono::milliseconds(1000),
       .post_action = MusicPlayer::PostPlaybackAction::Loop}
   );
}

void MenuScreenPause::select()
{
   switch (_selection)
   {
      case Selection::Resume:
         resume();
         break;
      case Selection::Options:
         Menu::getInstance()->show(Menu::MenuType::Options);
         MenuAudio::play(MenuAudio::SoundEffect::Apply);
         break;
      case Selection::Quit:
         MessageBox::question(
            "Do you want to end the game?",
            [](MessageBox::Button button)
            {
               if (button == MessageBox::Button::Yes)
               {
                  GameState::getInstance().enqueuePause();
                  Menu::getInstance()->show(Menu::MenuType::Main);
               }
            }
         );
         break;
   }
}

void MenuScreenPause::showEvent()
{
   // initial selection after coming from pause state should always be 'resume'
   _selection = Selection::Resume;
   updateLayers();
}

void MenuScreenPause::updateLayers()
{
   _layers["resume_0"]->_visible = (_selection != Selection::Resume);
   _layers["resume_1"]->_visible = (_selection == Selection::Resume);
   _layers["options_0"]->_visible = (_selection != Selection::Options);
   _layers["options_1"]->_visible = (_selection == Selection::Options);
   _layers["quit_game_0"]->_visible = (_selection != Selection::Quit);
   _layers["quit_game_1"]->_visible = (_selection == Selection::Quit);

   _layers["back_xbox_0"]->_visible = isControllerUsed();
   _layers["back_xbox_1"]->_visible = false;
   _layers["accept_xbox_0"]->_visible = isControllerUsed();
   _layers["accept_xbox_1"]->_visible = false;

   _layers["back_pc_0"]->_visible = !isControllerUsed();
   _layers["back_pc_1"]->_visible = false;
   _layers["accept_pc_0"]->_visible = !isControllerUsed();
   _layers["accept_pc_1"]->_visible = false;
}

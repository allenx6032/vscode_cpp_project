#include "deceptus_menuscreenaudio.h"

#include "deceptus_audio.h"
#include "deceptus_musicplayer.h"
#include "deceptus_gameconfiguration.h"
#include "deceptus_menu.h"
#include "deceptus_menuaudio.h"

#include <format>

#define STEP_SIZE 10

MenuScreenAudio::MenuScreenAudio()
{
   setFilename("data/menus/audio.psd");
}

void MenuScreenAudio::up()
{
   auto next = static_cast<int32_t>(_selection);
   next--;
   if (next < 0)
   {
      next = static_cast<int32_t>(Selection::Count) - 1;
   }

   _selection = static_cast<Selection>(next);
   updateLayers();

   MenuAudio::play(MenuAudio::SoundEffect::ItemNavigate);
}

void MenuScreenAudio::down()
{
   auto next = static_cast<int32_t>(_selection);
   next++;
   if (next == static_cast<int32_t>(Selection::Count))
   {
      next = 0;
   }

   _selection = static_cast<Selection>(next);
   updateLayers();

   MenuAudio::play(MenuAudio::SoundEffect::ItemNavigate);
}

void MenuScreenAudio::select()
{
   MenuAudio::play(MenuAudio::SoundEffect::ItemSelect);
}

void MenuScreenAudio::back()
{
   Menu::getInstance()->show(Menu::MenuType::Options);
   MenuAudio::play(MenuAudio::SoundEffect::MenuBack);
}

void MenuScreenAudio::set(int32_t x)
{
   switch (_selection)
   {
      case Selection::Master:
      {
         GameConfiguration::getInstance()._audio_volume_master += x;
         break;
      }
      case Selection::Music:
      {
         GameConfiguration::getInstance()._audio_volume_music += x;
         break;
      }
      case Selection::SFX:
      {
         GameConfiguration::getInstance()._audio_volume_sfx += x;
         break;
      }
      case Selection::Count:
      {
         break;
      }
   }

   if (GameConfiguration::getInstance()._audio_volume_master > 100)
   {
      GameConfiguration::getInstance()._audio_volume_master = 100;
   }

   if (GameConfiguration::getInstance()._audio_volume_master < 0)
   {
      GameConfiguration::getInstance()._audio_volume_master = 0;
   }

   if (GameConfiguration::getInstance()._audio_volume_music > 100)
   {
      GameConfiguration::getInstance()._audio_volume_music = 100;
   }

   if (GameConfiguration::getInstance()._audio_volume_music < 0)
   {
      GameConfiguration::getInstance()._audio_volume_music = 0;
   }

   if (GameConfiguration::getInstance()._audio_volume_sfx > 100)
   {
      GameConfiguration::getInstance()._audio_volume_sfx = 100;
   }

   if (GameConfiguration::getInstance()._audio_volume_sfx < 0)
   {
      GameConfiguration::getInstance()._audio_volume_sfx = 0;
   }

   GameConfiguration::getInstance().serializeToFile();

   // update the volume of active threads
   Audio::getInstance().adjustActiveSampleVolume();
   MusicPlayer::getInstance().adjustActiveMusicVolume();

   updateLayers();

   MenuAudio::play(MenuAudio::SoundEffect::ItemTick);
}

void MenuScreenAudio::setDefaults()
{
   GameConfiguration::resetAudioDefaults();
   updateLayers();

   MenuAudio::play(MenuAudio::SoundEffect::Apply);
}

void MenuScreenAudio::keyboardKeyPressed(sf::Keyboard::Key key)
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

   else if (key == sf::Keyboard::Key::Left)
   {
      set(-STEP_SIZE);
   }

   else if (key == sf::Keyboard::Key::Right)
   {
      set(STEP_SIZE);
   }

   else if (key == sf::Keyboard::Key::Escape)
   {
      back();
   }

   else if (key == sf::Keyboard::Key::D)
   {
      setDefaults();
   }
}

void MenuScreenAudio::loadingFinished()
{
   for (auto i = 0; i < 11; i++)
   {
      const auto master_value_layer = std::format("master_value_{}", i);
      const auto music_value_layer = std::format("mscVolume_value_{}", i);
      const auto sfx_value_layer = std::format("sfxVolume_value_{}", i);

      _volume_layers_master.push_back(_layers[master_value_layer]);
      _volume_layers_music.push_back(_layers[music_value_layer]);
      _volume_layers_sfx.push_back(_layers[sfx_value_layer]);
   }

   updateLayers();
}

void MenuScreenAudio::showEvent()
{
   updateLayers();
}

void MenuScreenAudio::updateLayers()
{
   auto master = _selection == Selection::Master;
   auto sfx = _selection == Selection::SFX;
   auto music = _selection == Selection::Music;

   _layers["defaults_xbox_0"]->_visible = isControllerUsed();
   _layers["defaults_xbox_1"]->_visible = false;
   _layers["back_xbox_0"]->_visible = isControllerUsed();
   _layers["back_xbox_1"]->_visible = false;

   _layers["defaults_pc_0"]->_visible = !isControllerUsed();
   _layers["defaults_pc_1"]->_visible = false;
   _layers["back_pc_0"]->_visible = !isControllerUsed();
   _layers["back_pc_1"]->_visible = false;

   _layers["sfxVolume_body_0"]->_visible = !sfx;
   _layers["sfxVolume_body_1"]->_visible = sfx;
   _layers["sfxVolume_text_0"]->_visible = !sfx;
   _layers["sfxVolume_text_1"]->_visible = sfx;
   _layers["sfxVolume_highlight"]->_visible = sfx;
   _layers["sfxVolume_help"]->_visible = sfx;
   _layers["sfxVolume_arrows"]->_visible = sfx;
   _layers["sfxVolume_h_0"]->_visible = !sfx;
   _layers["sfxVolume_h_1"]->_visible = sfx;

   _layers["mscVolume_body_0"]->_visible = !music;
   _layers["mscVolume_body_1"]->_visible = music;
   _layers["mscVolume_text_0"]->_visible = !music;
   _layers["mscVolume_text_1"]->_visible = music;
   _layers["mscVolume_highlight"]->_visible = music;
   _layers["mscVolume_help"]->_visible = music;
   _layers["mscVolume_arrows"]->_visible = music;
   _layers["mscVolume_h_0"]->_visible = !music;
   _layers["mscVolume_h_1"]->_visible = music;

   _layers["master_text_0"]->_visible = !master;
   _layers["master_text_1"]->_visible = master;
   _layers["master_body_1"]->_visible = master;
   _layers["master_highlight"]->_visible = master;
   _layers["master_help"]->_visible = master;
   _layers["master_arrows"]->_visible = master;
   _layers["master_h_0"]->_visible = !master;
   _layers["master_h_1"]->_visible = master;

   const auto master_volume = GameConfiguration::getInstance()._audio_volume_master;
   const auto sfx_volume = GameConfiguration::getInstance()._audio_volume_sfx;
   const auto music_volume = GameConfiguration::getInstance()._audio_volume_music;

   const auto master_volume_layer_index = static_cast<int32_t>(master_volume / 10);
   const auto sfx_volume_layer_index = static_cast<int32_t>(sfx_volume / 10);
   const auto music_volume_layer_index = static_cast<int32_t>(music_volume / 10);

   for (auto i = 0; i < 11; i++)
   {
      _volume_layers_master[i]->_visible = (i == master_volume_layer_index);
      _volume_layers_sfx[i]->_visible = (i == sfx_volume_layer_index);
      _volume_layers_music[i]->_visible = (i == music_volume_layer_index);
   }

   _layers["master_h_0"]->_sprite->setOrigin({50.0f - master_volume, 0.0f});
   _layers["sfxVolume_h_0"]->_sprite->setOrigin({50.0f - sfx_volume, 0.0f});
   _layers["mscVolume_h_0"]->_sprite->setOrigin({50.0f - music_volume, 0.0f});

   _layers["master_h_1"]->_sprite->setOrigin({50.0f - master_volume, 0.0f});
   _layers["sfxVolume_h_1"]->_sprite->setOrigin({50.0f - sfx_volume, 0.0f});
   _layers["mscVolume_h_1"]->_sprite->setOrigin({50.0f - music_volume, 0.0f});
}

/*
data/menus/audio.psd
    bg_temp
    audio-window-bg
    audio_window-main
    main_body_0
    header
*/

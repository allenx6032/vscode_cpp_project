#include "deceptus_game.h"

#ifdef _WIN32
#include <windows.h>
#endif

#include <chrono>
#include <cstdint>
#include <iostream>
#include <sstream>

#include "deceptus_logthread.h"
#include "deceptus_constants.h"
#include "deceptus_logui.h"
#include "deceptus_preloader.h"
#include "deceptus_test.h"

#ifdef __linux__
extern "C" int XInitThreads();
#endif

void debugAuthors()
{
   const std::chrono::time_point<std::chrono::system_clock> now{std::chrono::system_clock::now()};
   const std::chrono::year_month_day ymd{std::chrono::floor<std::chrono::days>(now)};

   std::stringstream text;
   text << GAME_NAME;
   text << " (c) ";
   text << static_cast<int32_t>(ymd.year());
   text << " dstar/mueslee";

   std::cout << text.str() << std::endl;
   for (auto i = 0u; i < text.str().length(); i++)
   {
      std::cout << "-";
   }
   std::cout << std::endl;
}

#ifdef __linux__
int main(int /*argc*/, char** /*argv*/)
#else
#ifdef DEBUG
int main(int /*argc*/, char** /*argv*/)
#else
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
#endif
#endif
{
#ifndef DEBUG
   // setup logging to file
   LogThread log_thread;
   Log::registerListenerCallback([&log_thread](const auto& time_point, auto level, const auto& message, const auto& location)
                                 { log_thread.log(time_point, level, message, location); });

   Log::registerListenerCallback([](const auto& time_point, auto level, const auto& message, const auto& location)
                                 { LogUiBuffer::log(time_point, level, message, location); });

#endif

#ifdef __linux__
   XInitThreads();
#endif

   debugAuthors();
   Test test;
   Game game;
   game.initialize();
   Preloader::preload();
   return game.loop();
}

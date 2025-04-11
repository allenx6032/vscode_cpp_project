#include "LEngine_logger.h"
#include <cstring>
#include <memory>

Logger::Logger(std::string str, color_T *color) {
  if(color && debug) {
    std::cout << get_color_code(*color) << "[Logger] Initialized with message: " << str << get_color_code(color_T::white) << std::endl;
  }
}

Logger::Logger() {
  if(debug) {
    std::cout << "[Logger] Initialized" << std::endl;
  }
}


void Logger::setDebug(bool val) {
  debug = val;
}

void Logger::setBuffer(std::string buffer) {
  buff = buffer;
}

void Logger::print(msg_T msg, bool bold) {
  std::unique_ptr<std::string> formatter(new std::string);
  color_T color = white;

  switch (msg) {
    case error:
      color = red;
      *formatter = "[!] ";
      break;
    case warning:
      color = yellow;
      *formatter = "[*] ";
      break;
    case info:
      color = green;
      *formatter = "[+] ";
      break;
  }
  
  std::cout << get_color_code(color, bold) << *formatter << buff << get_color_code(Colors::white) << std::endl;
}

std::string get_color_code(color_T color, bool bold) {
  switch(color) {
    case bright_green:
      if(!bold)
        return "\x1b[0;92m";
      else return "\x1b[1;92m";
    case red:
      if(!bold)
        return "\x1b[0;31m";
      else return "\x1b[1;31m";
    case green:
      if(!bold)
        return "\x1b[0;32m";
      else return "\x1b[1;32m";
    case black:
      if(!bold)
        return "\x1b[0;30m";
      else return "\x1b[1;30m";
    case yellow:
      if(!bold)
        return "\x1b[0;93m";
      else return "\x1b[1;93m";
    case white:
      if(!bold)
        return "\x1b[0;37m";
      else return "\x1b[1;37m";
    default:
      if(!bold)
        return "\x1b[0;97m";
      else return "\x1b[1;97m";
  }
}

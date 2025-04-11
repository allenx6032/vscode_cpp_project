#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>
#include <string>

#define uint unsigned int

typedef enum Colors : uint {
  white = 0,
  bright_green,
  green,
  black,
  red,
  yellow,
} color_T;

typedef enum Message {
  warning,
  error,
  info,
} msg_T;

std::string get_color_code(color_T color, bool bold = false);

class Logger {
public:
  Logger(std::string str, color_T *color);
  Logger();
  virtual void print(msg_T msg, bool bold = false);

  void setDebug(bool val);
  void setBuffer(std::string buffer);
  ~Logger() = default;
protected:
  std::string buff;
  bool debug = false;
};

#endif

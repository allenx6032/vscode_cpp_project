#ifndef PARSE_H_
#define PARSE_H_
#include <string>
#include <optional>
#include <filesystem>
namespace config
{
struct Args
{
  std::filesystem::path config;
  std::filesystem::path path;
};
typedef boost::optional<Args> OptionalArgs;

auto Parse(int argc, char* argv[]) -> OptionalArgs;
}
#endif
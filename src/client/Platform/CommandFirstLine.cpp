#include "CommandFirstLine.hpp"
#include <array>
#include <cstdio>
#include <string>
#include "PlatformConstants.hpp"

namespace rtype::client {

std::string firstLineOf(const char* command) {
  // NOLINTNEXTLINE(cert-env33-c,misc-include-cleaner)
  std::FILE* answer = popen(command, COMMAND_READ_MODE);
  if (answer == nullptr) {
    return {};
  }
  std::array<char, COMMAND_ANSWER_LENGTH> line{};
  const bool read =
      std::fgets(line.data(), static_cast<int>(line.size()), answer) != nullptr;
  // NOLINTNEXTLINE(misc-include-cleaner)
  pclose(answer);
  return read ? std::string(line.data()) : std::string();
}

}  // namespace rtype::client

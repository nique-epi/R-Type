#include <array>
#include <cstdio>
#include <string_view>
#include "PlatformConstants.hpp"
#include "ReducedMotion.hpp"

namespace rtype::client {

bool systemPrefersReducedMotion() {
  // NOLINTNEXTLINE(cert-env33-c,misc-include-cleaner)
  std::FILE* answer = popen(GNOME_ANIMATIONS_COMMAND, COMMAND_READ_MODE);
  if (answer == nullptr) {
    return false;
  }
  std::array<char, GNOME_ANSWER_LENGTH> line{};
  const bool read =
      std::fgets(line.data(), static_cast<int>(line.size()), answer) != nullptr;
  // NOLINTNEXTLINE(misc-include-cleaner)
  pclose(answer);
  return read &&
         std::string_view(line.data()).starts_with(GNOME_ANIMATIONS_OFF);
}

}  // namespace rtype::client

#include "CommandWords.hpp"
#include <span>
#include <string>
#include <string_view>
#include "EngineException.hpp"

namespace rtype::server {

void requireNoWord(std::span<const std::string> words,
                   std::string_view command) {
  if (!words.empty()) {
    throw engine::CommandRefusedException("expected: " + std::string{command});
  }
}

}  // namespace rtype::server

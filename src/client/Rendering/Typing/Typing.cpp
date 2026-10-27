#include "Typing.hpp"
#include <SFML/System/Utf.hpp>
#include <algorithm>
#include <cstddef>
#include <string_view>
#include "TimeConstants.hpp"

namespace rtype::client {

std::size_t lettersTypedAt(engine::Duration intoTyping,
                           engine::Duration letterTime, std::size_t length) {
  if (intoTyping < engine::Duration::zero()) {
    return 0;
  }
  return std::min(length,
                  static_cast<std::size_t>(intoTyping / letterTime) + 1);
}

engine::Duration typingTimeOf(engine::Duration letterTime,
                              std::size_t letters) {
  return letterTime * static_cast<engine::Duration::rep>(letters);
}

std::size_t letterCountOf(std::string_view text) {
  return sf::Utf8::count(text.begin(), text.end());
}

std::string_view typedPrefix(std::string_view text, std::size_t letters) {
  std::string_view::const_iterator end = text.begin();
  for (std::size_t typed = 0; typed < letters && end != text.end(); ++typed) {
    end = sf::Utf8::next(end, text.end());
  }
  return {text.begin(), end};
}

}  // namespace rtype::client

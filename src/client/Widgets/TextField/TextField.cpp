#include "TextField.hpp"
#include <SFML/System/String.hpp>
#include <SFML/System/Utf.hpp>
#include <SFML/Window/Event.hpp>
#include <cstddef>
#include <iterator>
#include <string>
#include <string_view>
#include "NetworkConstants.hpp"
#include "WidgetConstants.hpp"

namespace rtype::client {

namespace {

/** @returns Whether a field may hold the character. */
bool isPrintable(char32_t character) {
  return (character >= FIRST_PRINTABLE_BASIC_LATIN &&
          character <= LAST_PRINTABLE_BASIC_LATIN) ||
         (character >= FIRST_PRINTABLE_LATIN1_SUPPLEMENT &&
          character <= LAST_PRINTABLE_LATIN1_SUPPLEMENT);
}

bool isDigit(char32_t character) {
  return character >= FIRST_DIGIT && character <= LAST_DIGIT;
}

}  // namespace

TextField::TextField(std::string_view text, std::size_t maximumSize,
                     Characters accepted)
    : maximumSize_(maximumSize), accepted_(accepted) {
  for (const char32_t character :
       sf::String::fromUtf8(text.begin(), text.end())) {
    type(character);
  }
}

bool TextField::handleEvent(const sf::Event& event) {
  if (const auto* typed = event.getIf<sf::Event::TextEntered>()) {
    type(typed->unicode);
    return true;
  }
  const auto* pressed = event.getIf<sf::Event::KeyPressed>();
  if (pressed != nullptr && pressed->code == FIELD_ERASE_KEY) {
    erase();
    return true;
  }
  return false;
}

void TextField::type(char32_t character) {
  if (!accepts(character)) {
    return;
  }
  std::string bytes;
  sf::Utf8::encode(character, std::back_inserter(bytes));
  if (text_.size() + bytes.size() <= maximumSize_) {
    text_ += bytes;
  }
}

void TextField::erase() {
  auto lastCharacter = text_.cbegin();
  for (auto next = text_.cbegin(); next != text_.cend();
       next = sf::Utf8::next(next, text_.cend())) {
    lastCharacter = next;
  }
  text_.erase(lastCharacter, text_.cend());
}

const std::string& TextField::text() const { return text_; }

bool TextField::accepts(char32_t character) const {
  switch (accepted_) {
    case Characters::Digits:
      return isDigit(character);
    case Characters::DigitsAndDots:
      return isDigit(character) ||
             character ==
                 static_cast<char32_t>(network::IPV4_ADDRESS_SEPARATOR);
    case Characters::Printable:
      break;
  }
  return isPrintable(character);
}

}  // namespace rtype::client

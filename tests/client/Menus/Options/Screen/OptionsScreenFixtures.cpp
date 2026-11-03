#include "OptionsScreenFixtures.hpp"
#include <SFML/Window/Keyboard.hpp>
#include <string>
#include "KeyNames.hpp"
#include "OptionsScreenTestConstants.hpp"
#include "RecordingInterfaceCanvas.hpp"

std::string nameInTheFile(sf::Keyboard::Scancode key) {
  return std::string(rtype::client::keyName(key).value_or("?"));
}

const RecordedWriting* descriptionOf(const InterfaceCanvasRecord& record) {
  for (const RecordedWriting& written : record.writings) {
    if (written.anchor.x == rtype::client::testing::DESCRIPTION_LEFT &&
        written.text.starts_with("> ")) {
      return &written;
    }
  }
  return nullptr;
}

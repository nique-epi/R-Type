#include "MenuFixtures.hpp"
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <string>
#include "Menu.hpp"
#include "MenuPainting.hpp"
#include "MenuTestConstants.hpp"
#include "PointerEvents.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "WidgetConstants.hpp"

using rtype::client::testing::CHARACTER_WIDTH;
using rtype::client::testing::ONE_PIXEL_HIGHER;
using rtype::client::testing::THIN_CARET_SIZE;

sf::Vector2i barCornerOf(const rtype::client::Menu& menu,
                         const std::string& line) {
  const InterfaceCanvasRecord record = paintingOf(menu);
  const RecordedWriting* writing = writingOf(record, line);
  return writing == nullptr
             ? sf::Vector2i()
             : (writing->anchor - rtype::client::BUTTON_TEXT_OFFSET) *
                   INTERFACE_PIXEL;
}

sf::IntRect caretAfter(sf::Vector2i textAnchor, int characters) {
  return {textAnchor + sf::Vector2i(characters * CHARACTER_WIDTH, 0) +
              ONE_PIXEL_HIGHER,
          THIN_CARET_SIZE};
}

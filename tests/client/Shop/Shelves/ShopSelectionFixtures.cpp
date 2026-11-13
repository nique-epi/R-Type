#include "ShopSelectionFixtures.hpp"
#include <SFML/Window/Keyboard.hpp>
#include "ShopSelection.hpp"

namespace rtype::client::testing {

void moveBy(ShopSelection& selection, sf::Keyboard::Key key, int times) {
  for (int press = 0; press < times; ++press) {
    static_cast<void>(selection.move(key));
  }
}

}  // namespace rtype::client::testing

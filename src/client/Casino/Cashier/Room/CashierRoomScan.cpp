#include "CashierRoomScan.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include "CashierRoom.hpp"
#include "CashierRoomConstants.hpp"
#include "ColoredPixel.hpp"
#include "InterfaceConstants.hpp"

namespace rtype::client {

namespace {

/** @returns Whether @p color is the gold of the vault. */
bool isGold(sf::Color color) {
  return color.r > CASHIER_GOLD_RED_FLOOR &&
         color.g > CASHIER_GOLD_GREEN_FLOOR &&
         color.b < CASHIER_GOLD_BLUE_CEILING;
}

/** @returns Whether @p color is the brass of the grille. */
bool isBrass(sf::Color color) {
  return color.r > CASHIER_BRASS_RED_FLOOR &&
         color.g > CASHIER_BRASS_GREEN_FLOOR &&
         color.b<CASHIER_BRASS_BLUE_CEILING&& static_cast<int>(color.r)> color
                 .b +
             CASHIER_BRASS_WARMTH;
}

/** @brief Files @p pixel of @p picture in @p room when it is gold of the
 * vault or brass of the grille. */
void file(CashierRoom& room, const sf::Image& picture, sf::Vector2i pixel) {
  const sf::Color color = picture.getPixel(sf::Vector2u(pixel));
  if (CASHIER_VAULT_AREA.contains(pixel) && isGold(color)) {
    room.gold.push_back(pixel);
  }
  if (CASHIER_GRILLE_AREA.contains(pixel) && isBrass(color)) {
    room.grille.push_back(ColoredPixel{.position = pixel, .color = color});
  }
}

}  // namespace

CashierRoom cashierRoomOf(const sf::Image& picture) {
  CashierRoom room;
  if (sf::Vector2i(picture.getSize()) != INTERFACE_AREA.size) {
    return room;
  }
  for (int row = 0; row < INTERFACE_AREA.size.y; ++row) {
    for (int column = 0; column < INTERFACE_AREA.size.x; ++column) {
      file(room, picture, {column, row});
    }
  }
  return room;
}

}  // namespace rtype::client

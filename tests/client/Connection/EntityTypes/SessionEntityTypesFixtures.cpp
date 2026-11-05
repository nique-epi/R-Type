#include "SessionEntityTypesFixtures.hpp"
#include <string>
#include <string_view>
#include <vector>
#include "EntitySide.hpp"
#include "EntityTypeEntry.hpp"
#include "SessionEntityTypesTestConstants.hpp"

namespace rtype::client::testing {

namespace {

/** @returns The entry of an enemy named @p name. */
network::EntityTypeEntry enemyNamed(std::string_view name) {
  return network::EntityTypeEntry{.side = network::EntitySide::Enemy,
                                  .name = std::string{name}};
}

/** @returns The entry of the ship of a player, named DRAWN_SHIP_NAME. */
network::EntityTypeEntry drawnShip() {
  return network::EntityTypeEntry{.side = network::EntitySide::Player,
                                  .name = std::string{DRAWN_SHIP_NAME}};
}

}  // namespace

bool isDrawnInTheTests(std::string_view name) {
  return name == DRAWN_ENEMY_NAME || name == DRAWN_SHIP_NAME ||
         name == OTHER_DRAWN_ENEMY_NAME;
}

std::vector<network::EntityTypeEntry> drawnTypes() {
  return {enemyNamed(DRAWN_ENEMY_NAME), drawnShip()};
}

std::vector<network::EntityTypeEntry> otherDrawnTypes() {
  return {enemyNamed(OTHER_DRAWN_ENEMY_NAME)};
}

std::vector<network::EntityTypeEntry> oneUndrawnType() {
  return {enemyNamed(DRAWN_ENEMY_NAME), enemyNamed(UNDRAWN_ENEMY_NAME)};
}

std::vector<network::EntityTypeEntry> threeUndrawnTypes() {
  return {enemyNamed(UNDRAWN_ENEMY_NAME), drawnShip(),
          enemyNamed(OTHER_UNDRAWN_ENEMY_NAME),
          enemyNamed(THIRD_UNDRAWN_ENEMY_NAME)};
}

}  // namespace rtype::client::testing

#pragma once

#include <cstdint>
#include <vector>

namespace rtype::game {

/**
 * @brief The crystals an enemy drops when a missile kills it: the numbers of
 * their sheets, in the order of its sheet. It drops one of them, the one at
 * its network id modulo their count (see DamageRules).
 */
struct CarriedCrystals {
  std::vector<std::uint8_t> sheetNumbers;
};

}  // namespace rtype::game

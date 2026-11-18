#pragma once

#include <string>
#include <vector>

namespace rtype::network {

/** @brief The looks an account owns, by their names in
 * identifiers::SHIP_LOOK_ITEM_NAMES, in the order of that list: the answer of
 * `GET /api/users/me/looks`. */
struct OwnedLooks {
  std::vector<std::string> items;

  bool operator==(const OwnedLooks&) const = default;
};

}  // namespace rtype::network

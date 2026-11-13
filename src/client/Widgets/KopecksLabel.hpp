#pragma once

#include <SFML/System/Vector2.hpp>
#include <cstdint>
#include <string_view>

namespace rtype::client {

/** @brief An amount of kopecks as paintKopecks() writes it. */
struct KopecksLabel {
  /** @brief Where the last digit ends, on the top row of the label, which is
   * KOPECKS_LABEL_HEIGHT rows tall. */
  sf::Vector2i topRight;
  std::uint32_t amount{};
  /** @brief Written before the amount: KOPECKS_GAIN_PREFIX for a gain,
   * nothing for a balance or a price. */
  std::string_view prefix;
  /** @brief Whether the digits are outlined in ink: for a label laid on the
   * sky, outside any panel. */
  bool outlined = false;
};

}  // namespace rtype::client

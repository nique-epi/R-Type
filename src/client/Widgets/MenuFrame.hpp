#pragma once

#include <cstdint>

namespace rtype::client {

/**
 * @brief How MenuBuilder lays a menu out: a page is a column against the left
 * side of the safe margin; a dialog is a panel centered over the dimmed
 * screens below.
 */
enum class MenuFrame : std::uint8_t {
  Page,
  Dialog,
};

}  // namespace rtype::client

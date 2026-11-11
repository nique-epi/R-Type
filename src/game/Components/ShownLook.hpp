#pragma once

#include <cstdint>

namespace rtype::game {

/**
 * @brief Component of a body that shows one of the looks of its sheet instead
 * of its idle loop: the number of that look, as lookNumberOf() gives it, 0 for
 * its own idle loop. The server lists it in each WorldState; a body without it
 * shows its own idle loop.
 */
struct ShownLook {
  std::uint8_t number{0};
};

}  // namespace rtype::game

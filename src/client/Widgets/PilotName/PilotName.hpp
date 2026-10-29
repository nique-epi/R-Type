#pragma once

#include <string>
#include <string_view>

namespace rtype::client {

/** @returns The name a pilot is shown by: its nickname, or P and its number
 * when the waiting room never named it. */
[[nodiscard]] std::string pilotNameOf(std::string_view nickname,
                                      int playerNumber);

}  // namespace rtype::client

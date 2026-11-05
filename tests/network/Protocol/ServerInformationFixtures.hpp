#pragma once

#include "ServerInformation.hpp"

namespace rtype::network::testing {

/** @returns An answer of a server of two games, five players seated and three
 * free seats. */
[[nodiscard]] ServerInformation someServerInformation();

}  // namespace rtype::network::testing

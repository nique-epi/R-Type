#pragma once

#include <string>
#include "RoomRound.hpp"

namespace rtype::master {

/** @returns How the journal of a wallet names @p place:
 * `round:<room>:<round>`. */
[[nodiscard]] std::string roundReferenceOf(network::RoomRound place);

}  // namespace rtype::master

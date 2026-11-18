#pragma once

#include <cstdint>
#include <string>

namespace rtype::client {

/** @returns @p amount as it is written on screen: 1250 gives "1 250". */
[[nodiscard]] std::string kopecksText(std::uint32_t amount);

}  // namespace rtype::client

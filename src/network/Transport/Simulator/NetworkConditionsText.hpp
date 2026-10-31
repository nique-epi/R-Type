#pragma once

#include <string>
#include "NetworkConditions.hpp"

namespace rtype::network {

/**
 * @returns What @p conditions do to the datagrams, in words, such as "ping
 * +60 ms, up to 20 ms more, 1 % lost, 0 % duplicated, 0 % reordered".
 */
[[nodiscard]] std::string conditionsText(const NetworkConditions& conditions);

}  // namespace rtype::network

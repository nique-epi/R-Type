#pragma once

#include <cstdint>
#include "DeviceLinkDecision.hpp"

namespace rtype::master {

/** @brief What a signed-in player decided on a device link: the account that
 * decided and its decision. */
struct DeviceLinkVerdict {
  std::int64_t userId{};
  network::DeviceLinkDecision decision{network::DeviceLinkDecision::Approved};
};

}  // namespace rtype::master

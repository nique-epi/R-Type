#pragma once

#include <cstdint>
#include <string>

namespace rtype::client {

/**
 * @brief What the frame loop asked of the sign-in through the web site so
 * far, as MasterDeviceLinkFlow hands it to its thread: each ask has a number
 * one above the previous one of its kind, so the thread serves each new ask
 * once. 0 means not asked yet. The token is kept in memory only.
 */
struct DeviceLinkAsks {
  std::uint64_t linkAsk{};
  /** @brief The newest link asked when the frame loop cancelled: that link
   * and every older one are forgotten, a newer one is not. */
  std::uint64_t cancelledLinkAsk{};
  std::uint64_t signOutAsk{};
  /** @brief The token of the session signOutAsk closes. */
  std::string signOutToken;
};

}  // namespace rtype::client

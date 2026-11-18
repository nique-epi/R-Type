#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include "DeviceLinkReading.hpp"

namespace rtype::client {

/**
 * @brief The sign-in of the game through the web site, without the frame
 * ever waiting on HTTP: the game asks the master for a link, shows its short
 * code, reads it at its interval, and once a player approves it on the site,
 * trades it for the token of a session. An ask returns at once; its reading
 * shows up on a later frame.
 */
class IDeviceLinkFlow {
 public:
  virtual ~IDeviceLinkFlow() = default;

  /** @returns The number of this ask for a new link; a link asked before is
   * forgotten. */
  virtual std::uint64_t askLink() = 0;

  /** @brief Forgets the link asked last, on the master too. */
  virtual void askCancel() = 0;

  /** @brief Closes the session of @p token on the master. */
  virtual void askSignOut(const std::string& token) = 0;

  /** @returns Where the newest link stands, nothing before the first ask. */
  [[nodiscard]] virtual std::optional<DeviceLinkReading> reading() const = 0;
};

}  // namespace rtype::client

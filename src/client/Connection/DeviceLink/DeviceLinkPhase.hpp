#pragma once

#include <cstdint>

namespace rtype::client {

/** @brief Where the sign-in of the game through the web site stands. */
enum class DeviceLinkPhase : std::uint8_t {
  /** @brief The game asked the master for a link and waits for its code. */
  Requesting,
  /** @brief The code is shown; the game reads the link at its interval. */
  Waiting,
  /** @brief A player approved the link: the reading holds the token. */
  SignedIn,
  /** @brief The player refused to connect the game. */
  Denied,
  /** @brief The link expired before anyone decided. */
  Expired,
  /** @brief The master refused to open a link: its quota of the hour. */
  Refused,
  /** @brief No master answered: the game tries again, waiting longer. */
  Unreachable,
};

}  // namespace rtype::client

#pragma once

#include <cstdint>

namespace rtype::client {

/** @brief Whether the client can play on a server: it can only when the
 * server speaks network::PROTOCOL_VERSION. */
enum class ServerCompatibility : std::uint8_t {
  Compatible,
  /** @brief The server speaks another version: the list greys it out with
   * UPDATE, and nothing joins it. */
  UpdateNeeded,
};

}  // namespace rtype::client

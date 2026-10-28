#pragma once

#include <cstdint>

namespace rtype::client {

/** @brief Why the client lost its game, as the NO SIGNAL card tells it: the
 * server stopped answering, or it said it shut down. */
enum class NoSignalReason : std::uint8_t {
  SilentServer,
  ClosedServer,
};

}  // namespace rtype::client

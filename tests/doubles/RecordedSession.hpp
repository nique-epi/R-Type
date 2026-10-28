#pragma once

#include <cstdint>
#include "Endpoint.hpp"

/** @brief An address and the session token a RecordingServerLink was told to
 * admit or forget. */
struct RecordedSession {
  rtype::network::Endpoint endpoint;
  std::uint64_t sessionToken{};

  bool operator==(const RecordedSession&) const = default;
};

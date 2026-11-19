#pragma once

#include <cstddef>
#include <vector>
#include "MessageType.hpp"

/** @brief One message a part of a session sent to the server: its type and
 * its payload. */
struct SentToServer {
  rtype::network::MessageType type{};
  std::vector<std::byte> payload;
};

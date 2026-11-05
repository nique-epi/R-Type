#pragma once

#include <chrono>
#include <string>
#include "ServerCreation.hpp"

namespace rtype::master {

/** @brief A request to create a server: what it asks, from where, and when. */
struct CreationAttempt {
  network::ServerCreation creation;
  /** @brief The IPv4 address of the TCP peer that asked. */
  std::string address;
  std::chrono::sys_seconds requestedAt;
};

}  // namespace rtype::master

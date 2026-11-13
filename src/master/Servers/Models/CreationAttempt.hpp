#pragma once

#include <chrono>
#include <string>
#include "ServerCreation.hpp"

namespace rtype::master {

/** @brief A request to create a server: what it asks, from where, and when. */
struct CreationAttempt {
  network::ServerCreation creation;
  /**
   * @brief The address the creation is counted under: the visitor the web
   * site names (visitorAddressOf()), or the client the request came from.
   */
  std::string address;
  std::chrono::sys_seconds requestedAt;
};

}  // namespace rtype::master

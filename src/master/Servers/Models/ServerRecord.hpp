#pragma once

#include <chrono>
#include <cstdint>
#include "ServerRegistration.hpp"

namespace rtype::master {

/** @brief A registration as the table `servers` keeps it. */
struct ServerRecord {
  std::int64_t serverId{};
  /** @brief What the server sent, its address always known. */
  network::ServerRegistration registration;
  std::chrono::sys_seconds registeredAt;
};

}  // namespace rtype::master

#pragma once

#include <chrono>
#include <cstdint>
#include "ServerRegistration.hpp"

namespace rtype::master {

/** @brief A registration as the table `servers` keeps it. */
struct ServerRecord {
  /** @brief The key it registered with, which names the server for good. */
  std::int64_t keyId{};
  /** @brief What it sent, its address always known. */
  network::ServerRegistration registration;
  std::chrono::sys_seconds registeredAt;
};

}  // namespace rtype::master

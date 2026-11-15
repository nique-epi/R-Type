#pragma once

#include <chrono>
#include <string>
#include "AccountCredentials.hpp"

namespace rtype::master {

/** @brief Credentials a client sent to create an account or to open a session:
 * what it sent, from where, and when. */
struct CredentialsRequest {
  network::AccountCredentials credentials;
  /** @brief The address of the client, which the quotas are counted under. */
  std::string address;
  std::chrono::sys_seconds requestedAt;
};

}  // namespace rtype::master

#pragma once

#include <cstdint>
#include <string>
#include "UserRole.hpp"

namespace rtype::network {

/** @brief An account as the master shows it: never its password. */
struct User {
  std::int64_t id{};
  std::string nickname;
  UserRole role{UserRole::Player};

  bool operator==(const User&) const = default;
};

}  // namespace rtype::network

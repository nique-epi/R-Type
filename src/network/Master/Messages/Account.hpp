#pragma once

#include <cstdint>
#include <string>
#include "AccountRole.hpp"

namespace rtype::network {

/** @brief An account as the master shows it: never its password. */
struct Account {
  std::int64_t id{};
  std::string nickname;
  AccountRole role{AccountRole::Player};

  bool operator==(const Account&) const = default;
};

}  // namespace rtype::network

#pragma once

#include <chrono>
#include <cstdint>
#include <string>
#include "LookOrigin.hpp"

namespace rtype::master {

/** @brief A look an account comes to own. */
struct LookOwnership {
  std::int64_t userId{};
  /** @brief Its name in identifiers::SHIP_LOOK_ITEM_NAMES. */
  std::string item;
  LookOrigin origin{LookOrigin::Starter};
  std::chrono::sys_seconds acquiredAt;
};

}  // namespace rtype::master

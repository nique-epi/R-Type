#pragma once

#include <chrono>
#include <cstdint>

namespace rtype::master {

/** @brief How many servers one address may create since a date. */
struct CreationQuota {
  std::int64_t maximum{};
  std::chrono::sys_seconds since;
};

}  // namespace rtype::master

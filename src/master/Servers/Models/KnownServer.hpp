#pragma once

#include <cstdint>
#include <optional>
#include "ServerLimits.hpp"

namespace rtype::master {

/** @brief A server found by its key. */
struct KnownServer {
  std::int64_t id{};
  /** @brief The bounds its registration set; none before it registered. */
  std::optional<ServerLimits> limits = std::nullopt;
};

}  // namespace rtype::master

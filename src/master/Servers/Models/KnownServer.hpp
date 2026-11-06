#pragma once

#include <cstdint>
#include <optional>
#include "ProbeTarget.hpp"
#include "ServerLimits.hpp"

namespace rtype::master {

/** @brief A server found by its key. */
struct KnownServer {
  std::int64_t id{};
  /** @brief The bounds its registration set; none before it registered. */
  std::optional<ServerLimits> limits = std::nullopt;
  /** @brief Where its probes go; none before it registered, or when it
   * registered before the probes and has no secret. */
  std::optional<ProbeTarget> probe = std::nullopt;
};

}  // namespace rtype::master

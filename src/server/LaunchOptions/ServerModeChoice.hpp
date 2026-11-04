#pragma once

#include <span>
#include <string_view>
#include "ServerMode.hpp"

namespace rtype::server {

/** @brief The program the launch arguments choose, and the options left. */
struct ServerModeChoice {
  /** @brief The program the binary runs. */
  ServerMode mode{ServerMode::Server};
  /** @brief The arguments after `--mode` and its value, or all of them. */
  std::span<const std::string_view> options;
};

}  // namespace rtype::server

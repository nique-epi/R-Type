#pragma once

#include <cstdint>
#include <memory>
#include "IChildProcess.hpp"

namespace rtype::server {

/** @brief What the registry is given when a game process was started. */
struct GameOpening {
  std::uint32_t number{};
  std::uint16_t port{};
  std::uint64_t linkToken{};
  std::unique_ptr<IChildProcess> process;
};

}  // namespace rtype::server

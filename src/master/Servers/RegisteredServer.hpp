#pragma once

#include <cstdint>

namespace rtype::master {

/** @brief What a registration did: the id of the server, and if it is new. */
struct RegisteredServer {
  std::int64_t id{};
  bool created{false};
};

}  // namespace rtype::master

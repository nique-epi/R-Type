#pragma once

#include <functional>
#include <string>

namespace rtype::master {

/**
 * @brief The key `--new-server-key <name>` asks the module of the keys to
 * issue when it starts, and who receives it.
 */
struct ServerKeyOrder {
  std::string name;
  /** @brief Receives the key, the only place it is ever written. */
  std::function<void(const std::string&)> onIssued;
};

}  // namespace rtype::master

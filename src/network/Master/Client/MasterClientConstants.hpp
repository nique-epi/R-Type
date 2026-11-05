#pragma once

#include <chrono>

namespace rtype::network {

/**
 * @brief How long a call waits to connect to the master, then for each read
 * and write of its answer: a master that does not answer within them is
 * unreachable, where cpp-httplib would wait 300 seconds by default.
 */
constexpr std::chrono::seconds MASTER_CONNECTION_TIMEOUT{5};
constexpr std::chrono::seconds MASTER_TRANSFER_TIMEOUT{5};

}  // namespace rtype::network

#pragma once

#include <functional>
#include <string>

namespace rtype::master {

/**
 * @brief Told, with the reason, when a part of the master fails while it runs:
 * the program then stops, as the server does on a failure of its network.
 */
using FailureReport = std::function<void(const std::string& reason)>;

}  // namespace rtype::master

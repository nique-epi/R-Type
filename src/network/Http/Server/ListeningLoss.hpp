#pragma once

#include <functional>
#include <string>

namespace rtype::network {

/**
 * @brief Told, with the reason, when an HttpServer stops listening without
 * close() asking for it: its routes can no longer be reached.
 */
using ListeningLoss = std::function<void(const std::string& reason)>;

}  // namespace rtype::network

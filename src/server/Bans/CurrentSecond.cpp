#include "CurrentSecond.hpp"
#include <chrono>

namespace rtype::server {

std::chrono::sys_seconds currentSecond() {
  return std::chrono::floor<std::chrono::seconds>(
      std::chrono::system_clock::now());
}

}  // namespace rtype::server

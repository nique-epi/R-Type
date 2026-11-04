#include "MasterException.hpp"
#include <cstdint>
#include <stdexcept>
#include <string>

namespace rtype::master {

MasterException::MasterException(const std::string& message)
    : std::runtime_error(message) {}

HttpPortUnavailableException::HttpPortUnavailableException(
    const std::string& address, std::uint16_t port)
    : MasterException("cannot listen for HTTP on " + address + " port " +
                      std::to_string(port) +
                      ": another program holds the port, or the address is "
                      "not one of this machine") {}

}  // namespace rtype::master

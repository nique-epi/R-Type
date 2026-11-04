#pragma once

#include <cstdint>
#include <stdexcept>
#include <string>

namespace rtype::master {

/** @brief Root of every error raised by the master. */
class MasterException : public std::runtime_error {
 public:
  explicit MasterException(const std::string& message);
};

/**
 * @brief The HTTP server of the master could not take its address and port:
 * another program holds the port, or the address is none of the machine.
 */
class HttpPortUnavailableException : public MasterException {
 public:
  HttpPortUnavailableException(const std::string& address, std::uint16_t port);
};

/**
 * @brief A request read the database of the master while it is not open:
 * before the storage module started, or after it stopped.
 */
class DatabaseClosedException : public MasterException {
 public:
  DatabaseClosedException();
};

}  // namespace rtype::master

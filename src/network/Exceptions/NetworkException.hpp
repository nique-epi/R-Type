#pragma once

#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <string>

namespace rtype::network {

/**
 * @brief Root of every error raised by the network module.
 */
class NetworkException : public std::runtime_error {
 public:
  explicit NetworkException(const std::string& message);
};

/**
 * @brief A text that should hold an IPv4 or IPv6 address, and does not.
 */
class InvalidAddressException : public NetworkException {
 public:
  explicit InvalidAddressException(const std::string& address);
};

/**
 * @brief The system refused to open a socket or to bind it to its address
 *        and port, for example because the port is already taken.
 */
class SocketOpenException : public NetworkException {
 public:
  SocketOpenException(const std::string& address, std::uint16_t port,
                      const std::string& reason);
};

/**
 * @brief A datagram to send holds more bytes than MAX_DATAGRAM_SIZE.
 */
class DatagramTooLargeException : public NetworkException {
 public:
  explicit DatagramTooLargeException(std::size_t size);
};

/**
 * @brief A socket was asked to start receiving a second time.
 */
class ReceivingAlreadyStartedException : public NetworkException {
 public:
  ReceivingAlreadyStartedException();
};

}  // namespace rtype::network

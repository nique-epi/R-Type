#include "NetworkException.hpp"
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <string>
#include "NetworkConstants.hpp"

namespace rtype::network {

NetworkException::NetworkException(const std::string& message)
    : std::runtime_error(message) {}

InvalidAddressException::InvalidAddressException(const std::string& address)
    : NetworkException("Not an IP address: " + address) {}

SocketOpenException::SocketOpenException(const std::string& address,
                                         std::uint16_t port,
                                         const std::string& reason)
    : NetworkException("Cannot open a UDP socket on " + address + " port " +
                       std::to_string(port) + ": " + reason) {}

DatagramTooLargeException::DatagramTooLargeException(std::size_t size)
    : NetworkException("A datagram holds at most " +
                       std::to_string(MAX_DATAGRAM_SIZE) + " bytes, not " +
                       std::to_string(size)) {}

ReceivingAlreadyStartedException::ReceivingAlreadyStartedException()
    : NetworkException("The socket already hands its datagrams to a handler") {}

BufferUnderflowException::BufferUnderflowException(std::size_t requestedBytes,
                                                   std::size_t availableBytes)
    : NetworkException("Cannot read " + std::to_string(requestedBytes) +
                       " byte(s), only " + std::to_string(availableBytes) +
                       " left in the buffer") {}

StringTooLongException::StringTooLongException(std::size_t length,
                                               std::size_t maximumLength)
    : NetworkException("String of " + std::to_string(length) +
                       " byte(s) exceeds the maximum of " +
                       std::to_string(maximumLength)) {}

}  // namespace rtype::network

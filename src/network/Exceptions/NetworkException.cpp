#include "NetworkException.hpp"
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <string_view>
#include "NetworkConstants.hpp"

namespace rtype::network {

NetworkException::NetworkException(const std::string& message)
    : std::runtime_error(message) {}

InvalidAddressException::InvalidAddressException(const std::string& address)
    : NetworkException("Not an IP address: " + address) {}

InvalidPortException::InvalidPortException(const std::string& text)
    : NetworkException(
          "Invalid port \"" + text + "\": expected a number from " +
          std::to_string(LOWEST_PORT) + " to " + std::to_string(HIGHEST_PORT)) {
}

InvalidIpv4AddressException::InvalidIpv4AddressException(
    const std::string& text)
    : NetworkException("Invalid IPv4 address \"" + text +
                       "\": expected four numbers from 0 to " +
                       std::to_string(IPV4_ADDRESS_PART_MAXIMUM) +
                       " separated by dots, such as " +
                       std::string(LOOPBACK_IPV4_ADDRESS)) {}

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

MalformedMessageException::MalformedMessageException(const std::string& reason)
    : NetworkException("Malformed message: " + reason) {}

MalformedDatagramException::MalformedDatagramException(std::size_t datagramSize)
    : NetworkException("A datagram of " + std::to_string(datagramSize) +
                       " bytes is not a header followed by whole messages") {}

InvalidBoolException::InvalidBoolException(std::uint8_t value)
    : MalformedMessageException("a bool holds " + std::to_string(value) +
                                ", where only 0 and 1 are valid") {}

TrailingBytesException::TrailingBytesException(std::size_t count)
    : MalformedMessageException(std::to_string(count) +
                                " byte(s) left after the last field") {}

InvalidNicknameException::InvalidNicknameException(std::size_t size)
    : MalformedMessageException("a nickname of " + std::to_string(size) +
                                " byte(s) is empty, too long, not UTF-8 or "
                                "holds a control character") {}

InvalidFieldValueException::InvalidFieldValueException(std::string_view field,
                                                       std::uint64_t value)
    : MalformedMessageException(std::string{field} + " holds " +
                                std::to_string(value) +
                                ", a value the protocol does not allow") {}

InvalidCoordinateException::InvalidCoordinateException()
    : NetworkException("A coordinate to send is not a number") {}

DuplicatePlayerNumberException::DuplicatePlayerNumberException(
    std::uint8_t playerNumber)
    : MalformedMessageException(
          "player number " + std::to_string(playerNumber) + " appears twice") {}

TooManyPlayersException::TooManyPlayersException(std::size_t playerCount,
                                                 std::size_t maximumCount)
    : MalformedMessageException(
          "a message lists at most " + std::to_string(maximumCount) +
          " players, not " + std::to_string(playerCount)) {}

EmptyMessageHandlerException::EmptyMessageHandlerException()
    : NetworkException("A message handler must not be empty") {}

DuplicateMessageHandlerException::DuplicateMessageHandlerException(
    std::uint8_t messageType)
    : NetworkException("Message type " + std::to_string(messageType) +
                       " already has a handler") {}

}  // namespace rtype::network

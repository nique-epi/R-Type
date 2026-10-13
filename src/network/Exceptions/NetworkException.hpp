#pragma once

#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <string_view>

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

/**
 * @brief A read asked for more bytes than the buffer still holds.
 */
class BufferUnderflowException : public NetworkException {
 public:
  BufferUnderflowException(std::size_t requestedBytes,
                           std::size_t availableBytes);
};

/**
 * @brief A string is longer than the length prefix can announce.
 */
class StringTooLongException : public NetworkException {
 public:
  StringTooLongException(std::size_t length, std::size_t maximumLength);
};

/**
 * @brief Bytes that cannot be a message: an announced count out of range, an
 * unknown value, or bytes left after the message.
 */
class MalformedMessageException : public NetworkException {
 public:
  explicit MalformedMessageException(const std::string& reason);
};

/**
 * @brief Received bytes that are not a packet header followed by whole
 * messages: shorter than the header, without any message, or with message
 * sizes that do not add up to the end of the datagram.
 */
class MalformedDatagramException : public NetworkException {
 public:
  explicit MalformedDatagramException(std::size_t datagramSize);
};

/**
 * @brief A bool on the wire holds a byte other than 0 or 1.
 */
class InvalidBoolException : public MalformedMessageException {
 public:
  explicit InvalidBoolException(std::uint8_t value);
};

/**
 * @brief Bytes left in a payload after the last field of its message.
 */
class TrailingBytesException : public MalformedMessageException {
 public:
  explicit TrailingBytesException(std::size_t count);
};

/**
 * @brief A nickname that is empty, longer than the protocol allows, or not
 * UTF-8, read from a payload or about to be written into one.
 */
class InvalidNicknameException : public MalformedMessageException {
 public:
  explicit InvalidNicknameException(std::size_t size);
};

/**
 * @brief A field holds a value the protocol page does not list for it, read
 * from a payload or about to be written into one. The field is named as on
 * the protocol page.
 */
class InvalidFieldValueException : public MalformedMessageException {
 public:
  InvalidFieldValueException(std::string_view field, std::uint64_t value);
};

/**
 * @brief A coordinate to send is not a number, so it has no place on the wire.
 */
class InvalidCoordinateException : public NetworkException {
 public:
  InvalidCoordinateException();
};

/**
 * @brief The same player number appears twice in the players of a message,
 * read from a payload or about to be written into one.
 */
class DuplicatePlayerNumberException : public MalformedMessageException {
 public:
  explicit DuplicatePlayerNumberException(std::uint8_t playerNumber);
};

/**
 * @brief A message lists more players than a game has, read from a payload or
 * about to be written into one.
 */
class TooManyPlayersException : public MalformedMessageException {
 public:
  TooManyPlayersException(std::size_t playerCount, std::size_t maximumCount);
};

}  // namespace rtype::network

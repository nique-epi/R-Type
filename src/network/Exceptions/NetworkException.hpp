#pragma once

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <string_view>

namespace rtype::network {

/**
 * @brief Root of every error raised by the network library.
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
 * @brief A text that should hold a port, from LOWEST_PORT to HIGHEST_PORT
 *        in decimal digits, and does not.
 */
class InvalidPortException : public NetworkException {
 public:
  explicit InvalidPortException(const std::string& text);
};

/**
 * @brief A text that should hold an IPv4 address, four decimal numbers from
 *        0 to 255 separated by dots, and does not.
 */
class InvalidIpv4AddressException : public NetworkException {
 public:
  explicit InvalidIpv4AddressException(const std::string& text);
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
 * @brief A datagram to send holds no message: every datagram holds at least
 * one, and a receiver drops one that does not.
 */
class EmptyDatagramException : public NetworkException {
 public:
  EmptyDatagramException();
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
 * @brief A nickname that is empty, longer than the protocol allows, not UTF-8,
 * or holding a control character, read from a payload or about to be written
 * into one.
 */
class InvalidNicknameException : public MalformedMessageException {
 public:
  explicit InvalidNicknameException(std::size_t size);
};

/**
 * @brief A difficulty that is empty, longer than the protocol allows, not
 * UTF-8, or holding a control character, read from a payload or about to be
 * written into one.
 */
class InvalidDifficultyException : public MalformedMessageException {
 public:
  explicit InvalidDifficultyException(std::size_t size);
};

/**
 * @brief A LeaderboardAnswer lists more rows than the protocol allows.
 */
class TooManyLeaderboardRowsException : public MalformedMessageException {
 public:
  TooManyLeaderboardRowsException(std::size_t rowCount,
                                  std::size_t maximumCount);
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

/**
 * @brief A state would need more datagrams than MAXIMUM_DELTA_PARTS, so it
 * cannot be written.
 */
class TooManyDeltaPartsException : public NetworkException {
 public:
  TooManyDeltaPartsException(std::size_t partCount, std::size_t maximumCount);
};

/**
 * @brief A message handler was registered empty.
 */
class EmptyMessageHandlerException : public NetworkException {
 public:
  EmptyMessageHandlerException();
};

/**
 * @brief A second handler was registered for a message type that already has
 * one.
 */
class DuplicateMessageHandlerException : public NetworkException {
 public:
  explicit DuplicateMessageHandlerException(std::uint8_t messageType);
};

/**
 * @brief The value after an option of the network simulator is not one it
 * takes: an unknown scenario, or a number out of its range.
 */
class InvalidSimulatorOptionException : public NetworkException {
 public:
  InvalidSimulatorOptionException(const std::string& option,
                                  const std::string& value);
};

/**
 * @brief An option of the network simulator is the last argument, without
 * its value.
 */
class MissingSimulatorOptionValueException : public NetworkException {
 public:
  explicit MissingSimulatorOptionValueException(const std::string& option);
};

/**
 * @brief The conditions of a simulated network have a lag or a jitter below
 * zero or over the longest the simulator takes: no network makes a datagram
 * wait less than nothing, no wait can be drawn from a negative jitter, and the
 * longest keeps every wait far below one that would overflow the clock the
 * waits are counted on.
 */
class SimulatedDelayOutOfRangeException : public NetworkException {
 public:
  SimulatedDelayOutOfRangeException(std::chrono::milliseconds lag,
                                    std::chrono::milliseconds jitter,
                                    std::chrono::milliseconds longest);
};

/**
 * @brief A file the traffic of a program should go to, and that the system
 * refuses to open.
 */
class TrafficFileOpenException : public NetworkException {
 public:
  explicit TrafficFileOpenException(const std::string& path);
};

/**
 * @brief A message exchanged with the master, in JSON, that is not JSON or
 * holds a member that is missing, of another type or out of its bounds. The
 * member is named by its JSON Pointer (RFC 6901), such as
 * `/games/2/players/1`; the empty pointer names the whole message.
 */
class InvalidMasterMessageException : public NetworkException {
 public:
  explicit InvalidMasterMessageException(const std::string& pointer);
};

/**
 * @brief The master answered a request with an error status: the status, and
 * the `detail` of its problem document (RFC 9457), empty when it gave none.
 */
class MasterRefusedException : public NetworkException {
 public:
  MasterRefusedException(int status, const std::string& detail);

  /** @returns The HTTP status the master answered, 400 or more. */
  [[nodiscard]] int status() const;

 private:
  int status_;
};

/**
 * @brief A url of the master the client cannot call: not `http://<host>`,
 * with an optional port. HTTPS is refused too, since cpp-httplib is built
 * without TLS.
 */
class InvalidMasterUrlException : public NetworkException {
 public:
  explicit InvalidMasterUrlException(const std::string& url);
};

/**
 * @brief No answer came from the master: no connection, a timeout, or a
 * connection cut before the answer; @p reason says which, as cpp-httplib names
 * it.
 */
class MasterUnreachableException : public NetworkException {
 public:
  explicit MasterUnreachableException(const std::string& reason);
};

}  // namespace rtype::network

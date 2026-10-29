#pragma once

#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>
#include "WaveFilePlace.hpp"
#include "WholeNumberRange.hpp"

namespace rtype::server {

/**
 * @brief Root of every error raised by the server program.
 */
class ServerException : public std::runtime_error {
 public:
  explicit ServerException(const std::string& message);
};

/** @brief The waves file cannot be opened. */
class UnreadableWaveFileException : public ServerException {
 public:
  /** @param file The path of the file, as the errors show it. */
  explicit UnreadableWaveFileException(const std::string& file);
};

/** @brief The waves file is not TOML. */
class WaveFileSyntaxException : public ServerException {
 public:
  /** @param description What the TOML reader found wrong at @p place. */
  WaveFileSyntaxException(const WaveFilePlace& place,
                          const std::string& description);
};

/**
 * @brief A key the waves file must hold is missing; the place is the table it
 * is missing from.
 */
class MissingWaveFileKeyException : public ServerException {
 public:
  explicit MissingWaveFileKeyException(const WaveFilePlace& place);
};

/** @brief The waves file holds a key it may not hold, often a misspelt one. */
class UnknownWaveFileKeyException : public ServerException {
 public:
  explicit UnknownWaveFileKeyException(const WaveFilePlace& place);
};

/** @brief A key of the waves file that must hold a table holds a value. */
class WaveFileTableExpectedException : public ServerException {
 public:
  explicit WaveFileTableExpectedException(const WaveFilePlace& place);
};

/**
 * @brief A value of the waves file is not a TOML integer within the range of
 * its key.
 */
class WaveFileValueOutOfRangeException : public ServerException {
 public:
  WaveFileValueOutOfRangeException(const WaveFilePlace& place,
                                   WholeNumberRange range);
};

/**
 * @brief The enemies of a difficulty are not a list of names, or hold fewer
 * or more names than @p length allows.
 */
class InvalidEnemyListException : public ServerException {
 public:
  InvalidEnemyListException(const WaveFilePlace& place,
                            WholeNumberRange length);
};

/**
 * @brief The enemies of a difficulty name an enemy the server does not know.
 */
class UnknownEnemyException : public ServerException {
 public:
  /** @param known The names of the enemies the server knows, in the message. */
  UnknownEnemyException(const WaveFilePlace& place, const std::string& enemy,
                        const std::vector<std::string>& known);
};

/** @brief The waves file describes no difficulty of the name asked for. */
class UnknownDifficultyException : public ServerException {
 public:
  /** @param known The difficulties the file describes, in the message. */
  UnknownDifficultyException(const std::string& difficulty,
                             const std::vector<std::string>& known);
};

/** @brief A new player has no game to join: no game was added yet. */
class NoJoinableGameException : public ServerException {
 public:
  NoJoinableGameException();
};

/** @brief ServerApplication::open() was called on a server already open. */
class ServerAlreadyOpenException : public ServerException {
 public:
  ServerAlreadyOpenException();
};

/** @brief A server was used before open() bound its socket. */
class ServerNotOpenException : public ServerException {
 public:
  ServerNotOpenException();
};

/** @brief The system refuses to block the signals that ask the server to
 * stop (Linux and macOS). */
class SignalMaskException : public ServerException {
 public:
  /** @param reason What the system answered, in the message. */
  explicit SignalMaskException(const std::string& reason);
};

/** @brief Windows refuses the handler of the console events that ask the
 * server to stop. */
class ConsoleHandlerException : public ServerException {
 public:
  /** @param errorCode The code GetLastError() gave, in the message. */
  explicit ConsoleHandlerException(std::uint32_t errorCode);
};

/** @brief A wave plan names no enemy, so its waves would bring nothing. */
class EmptyEnemyCycleException : public ServerException {
 public:
  EmptyEnemyCycleException();
};

/** @brief A game was set up with a number of world states per second the
 * server cannot send. */
class InvalidStateRateException : public ServerException {
 public:
  /** @param statesPerSecond The number given, in the message. */
  explicit InvalidStateRateException(std::size_t statesPerSecond);
};

}  // namespace rtype::server

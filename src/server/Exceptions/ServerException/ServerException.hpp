#pragma once

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

/** @brief A wave plan names no enemy, so its waves would bring nothing. */
class EmptyEnemyCycleException : public ServerException {
 public:
  EmptyEnemyCycleException();
};

/**
 * @brief A module used the link to the network while the network module
 * had not opened it, or had closed it: a defect in the order of the modules.
 */
class ServerLinkClosedException : public ServerException {
 public:
  ServerLinkClosedException();
};

/** @brief A new player has no game to join: the games module started none. */
class NoGameException : public ServerException {
 public:
  NoGameException();
};

/** @brief The system refused the handler of the stop signals. */
class StopSignalsRefusedException : public ServerException {
 public:
  StopSignalsRefusedException();
};

}  // namespace rtype::server

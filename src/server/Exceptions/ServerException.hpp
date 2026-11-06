#pragma once

#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>
#include "DataFilePlace.hpp"
#include "WholeNumberRange.hpp"

namespace rtype::server {

/**
 * @brief Root of every error raised by the server program.
 */
class ServerException : public std::runtime_error {
 public:
  explicit ServerException(const std::string& message);
};

/**
 * @brief The enemies of a difficulty are not a list of names, or hold fewer
 * or more names than @p length allows.
 */
class InvalidEnemyListException : public ServerException {
 public:
  InvalidEnemyListException(const engine::DataFilePlace& place,
                            engine::WholeNumberRange length);
};

/**
 * @brief The enemies of a difficulty name an enemy the server does not know.
 */
class UnknownEnemyException : public ServerException {
 public:
  /** @param known The names of the enemies the server knows, in the message. */
  UnknownEnemyException(const engine::DataFilePlace& place,
                        const std::string& enemy,
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

/** @brief The server was asked to host more games than a heartbeat lists. */
class TooManyGamesException : public ServerException {
 public:
  TooManyGamesException(std::size_t requested, std::size_t maximum);
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

/** @brief A game was set up with a number of ticks to hold each input that
 * the server cannot hold. */
class InvalidInputDelayException : public ServerException {
 public:
  /** @param delayTicks The number given, in the message. */
  explicit InvalidInputDelayException(std::size_t delayTicks);
};

/** @brief A child process could not be started. */
class ProcessLaunchException : public ServerException {
 public:
  /** @param description What the system said about @p program. */
  ProcessLaunchException(const std::string& program,
                         const std::string& description);
};

/**
 * @brief The system refused to tie the life of a child process to the one of
 * the program that started it, so the child could outlive its parent.
 */
class ChildBindingException : public ServerException {
 public:
  /** @param step The system call that failed. */
  explicit ChildBindingException(const std::string& step);
};

/**
 * @brief A game process was launched without one of the two options that link
 * it to its supervisor.
 */
class MissingSupervisorLinkException : public ServerException {
 public:
  /** @param missingOption The option that was not given. */
  explicit MissingSupervisorLinkException(const std::string& missingOption);
};

/**
 * @brief The supervisor could not write the file of commands of its games, so
 * they would start without the lines of server.cfg meant for them.
 */
class GameConfigurationWriteException : public ServerException {
 public:
  /** @param file The file that could not be written. */
  explicit GameConfigurationWriteException(const std::string& file);
};

/**
 * @brief A game was opened while every place of the supervisor, from
 * network::FIRST_HOSTED_GAME_NUMBER to MAXIMUM_RUNNING_GAMES, holds a game.
 */
class NoFreeGameSlotException : public ServerException {
 public:
  /** @param slotCount How many places the supervisor has. */
  explicit NoFreeGameSlotException(std::size_t slotCount);
};

}  // namespace rtype::server

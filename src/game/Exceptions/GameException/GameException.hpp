#pragma once

#include <cstddef>
#include <span>
#include <stdexcept>
#include <string>
#include <string_view>
#include "DataFilePlace.hpp"

namespace rtype::game {

/**
 * @brief Root of every error raised by the game library.
 */
class GameException : public std::runtime_error {
 public:
  explicit GameException(const std::string& message);
};

/**
 * @brief A player number outside 1 to the number of players of the game was
 * given.
 */
class InvalidPlayerNumberException : public GameException {
 public:
  InvalidPlayerNumberException();
};

/**
 * @brief A random draw outside 0 up to, but not including, 1 was given where a
 * unit draw is expected.
 */
class InvalidRandomUnitException : public GameException {
 public:
  InvalidRandomUnitException();
};

/**
 * @brief A wave curve would bring a wave without Bydos, Bydos with no time
 * between them, waves that slow down, a negative pause, or a level without
 * a wave (see isPlayable).
 */
class InvalidWaveCurveException : public GameException {
 public:
  InvalidWaveCurveException();
};

/**
 * @brief Thrown when a BehaviourRegistry is given a second behaviour under a
 * name it already holds.
 */
class DuplicateBehaviourException : public GameException {
 public:
  explicit DuplicateBehaviourException(std::string_view name);
};

/**
 * @brief Thrown when a behaviour reads a setting its body was not given: a
 * Pilot made without the values of its sheet.
 */
class MissingBehaviourSettingException : public GameException {
 public:
  explicit MissingBehaviourSettingException(std::size_t index);
};

/**
 * @brief Thrown when no entity sheet has the name asked for; the message lists
 * the sheets.
 */
class UnknownEntityTypeException : public GameException {
 public:
  UnknownEntityTypeException(std::string_view name,
                             std::span<const std::string_view> known);
};

/**
 * @brief Thrown when the file of a sheet is not named with 1 to
 * @p longestName lowercase letters, digits and underscores.
 */
class InvalidEntityTypeNameException : public GameException {
 public:
  InvalidEntityTypeNameException(const std::string& file,
                                 std::size_t longestName);
};

/** @brief Thrown when more sheets are given than a program loads. */
class TooManyEntityTypesException : public GameException {
 public:
  TooManyEntityTypesException(std::size_t count, std::size_t maximum);
};

/** @brief Thrown when the folder of the sheets holds none: a program that
 * knows no type could create or draw nothing. */
class NoEntitySheetException : public GameException {
 public:
  explicit NoEntitySheetException(const std::string& folder);
};

/** @brief Thrown when two sheets are given the same name. */
class DuplicateEntityTypeException : public GameException {
 public:
  explicit DuplicateEntityTypeException(std::string_view name);
};

/**
 * @brief Thrown when a sheet breaks a rule that ties two of its values, or a
 * value to another sheet; the message names the file, the line, the column,
 * the key and the rule.
 */
class InconsistentEntitySheetException : public GameException {
 public:
  InconsistentEntitySheetException(const engine::DataFilePlace& place,
                                   std::string_view rule);
};

}  // namespace rtype::game

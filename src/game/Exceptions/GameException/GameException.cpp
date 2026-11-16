#include "GameException.hpp"
#include <cstddef>
#include <span>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>
#include "DataFilePlace.hpp"
#include "ErrorText.hpp"
#include "LdtkRefusal.hpp"

namespace rtype::game {

GameException::GameException(const std::string& message)
    : std::runtime_error(message) {}

InvalidPlayerNumberException::InvalidPlayerNumberException()
    : GameException(
          "The player number must be between 1 and the number of players") {}

InvalidRandomUnitException::InvalidRandomUnitException()
    : GameException("The random draw must be at least 0 and under 1") {}

InvalidWaveCurveException::InvalidWaveCurveException()
    : GameException(
          "A wave curve must bring at least one Bydo per wave, a time above "
          "zero between two Bydos no longer than in its first wave, and no "
          "negative cut or pause, and a level of at least one wave") {}

DuplicateBehaviourException::DuplicateBehaviourException(std::string_view name)
    : GameException("A behaviour is already named " + std::string(name)) {}

MissingBehaviourSettingException::MissingBehaviourSettingException(
    std::size_t index)
    : GameException("The behaviour reads setting " + std::to_string(index) +
                    ", which its body was not given") {}

UnknownEntityTypeException::UnknownEntityTypeException(
    std::string_view name, std::span<const std::string_view> known)
    : GameException("No entity sheet is named " + std::string(name) +
                    "; the sheets are: " + engine::listText(known)) {}

InvalidEntityTypeNameException::InvalidEntityTypeNameException(
    const std::string& file, std::size_t longestName)
    : GameException("The entity sheet " + file + " must be named with 1 to " +
                    std::to_string(longestName) +
                    " lowercase letters, digits or underscores") {}

TooManyEntityTypesException::TooManyEntityTypesException(std::size_t count,
                                                         std::size_t maximum)
    : GameException(std::to_string(count) + " entity sheets, more than the " +
                    std::to_string(maximum) + " a program loads") {}

NoEntitySheetException::NoEntitySheetException(const std::string& folder)
    : GameException("The folder " + folder + " holds no entity sheet") {}

DuplicateEntityTypeException::DuplicateEntityTypeException(
    std::string_view name)
    : GameException("Two entity sheets are named " + std::string(name)) {}

InvalidCollisionLayerNameException::InvalidCollisionLayerNameException(
    const engine::DataFilePlace& place, std::size_t longestName)
    : GameException(engine::placeText(place) + ": " + place.key +
                    " must be 1 to " + std::to_string(longestName) +
                    " lowercase letters, digits or underscores") {}

TooManyCollisionLayersException::TooManyCollisionLayersException(
    std::size_t count, std::size_t maximum)
    : GameException(std::to_string(count) +
                    " collision layers, more than the " +
                    std::to_string(maximum) + " the sheets may name") {}

InconsistentEntitySheetException::InconsistentEntitySheetException(
    const engine::DataFilePlace& place, std::string_view rule)
    : GameException(engine::placeText(place) + ": " + place.key + ": " +
                    std::string(rule)) {}

SharedContentFileException::SharedContentFileException(
    const std::string& path, const std::vector<std::string>& folders)
    : GameException(
          "The content file " + path +
          " is in more than one folder: " + engine::listText(folders)) {}

InvalidLdtkFileException::InvalidLdtkFileException(const LdtkRefusal& refusal)
    : GameException("The level file " + refusal.file +
                    " is refused: " + refusal.reason) {}

}  // namespace rtype::game

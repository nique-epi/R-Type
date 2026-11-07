#include "ClientException.hpp"
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>
#include "ClientConstants.hpp"
#include "ProtocolConstants.hpp"

namespace rtype::client {

namespace {

void appendProblemLines(std::string& message,
                        const std::vector<std::string>& subjects,
                        std::string_view reason) {
  for (const std::string& subject : subjects) {
    message += "\n  ";
    message += subject;
    message += ": ";
    message += reason;
  }
}

std::string assetLoadingMessage(std::string_view assetFolder,
                                const AssetProblems& problems) {
  std::string message = "Cannot load the assets of ";
  message += assetFolder;
  appendProblemLines(message, problems.invalidIds,
                     "breaks the asset id rule, see assets/README.md");
  appendProblemLines(message, problems.missingFiles, "missing");
  appendProblemLines(message, problems.unreadableFiles, "cannot be read");
  return message;
}

}  // namespace

ClientException::ClientException(const std::string& message)
    : std::runtime_error(message) {}

ForeignSpriteSheetException::ForeignSpriteSheetException(
    const std::string& sheet)
    : ClientException("a sheet of the content pack names " + sheet +
                      ", which the pack does not hold") {}

ContentFileWriteException::ContentFileWriteException(const std::string& file)
    : ClientException("cannot write the file of a content pack " + file) {}

AssetLoadingException::AssetLoadingException(std::string_view assetFolder,
                                             const AssetProblems& problems)
    : ClientException(assetLoadingMessage(assetFolder, problems)) {}

AssetNotLoadedException::AssetNotLoadedException(std::string_view assetId)
    : ClientException("Asset \"" + std::string(assetId) +
                      "\" is not loaded: list it in the interface assets or "
                      "in the assets of a game") {}

RenderTextureNotCreatedException::RenderTextureNotCreatedException(
    unsigned int width, unsigned int height)
    : ClientException("A render texture could not be created, size " +
                      std::to_string(width) + " x " + std::to_string(height)) {}

DuplicateScreenException::DuplicateScreenException(std::string_view screenName)
    : ClientException("Two modules registered the screen \"" +
                      std::string(screenName) + "\"") {}

InvalidNicknameOptionException::InvalidNicknameOptionException(
    std::string_view nickname)
    : ClientException(
          "Invalid nickname \"" + std::string(nickname) + "\": expected " +
          std::to_string(network::MINIMUM_NICKNAME_SIZE) + " to " +
          std::to_string(network::MAXIMUM_NICKNAME_SIZE) + " bytes of UTF-8") {}

InvalidGameOptionException::InvalidGameOptionException(std::string_view game)
    : ClientException(
          "Invalid game \"" + std::string(game) + "\": expected " +
          std::string(NEW_GAME_VALUE) + " or a game number from " +
          std::to_string(network::FIRST_HOSTED_GAME_NUMBER) + " to " +
          std::to_string(std::numeric_limits<std::uint16_t>::max())) {}

}  // namespace rtype::client

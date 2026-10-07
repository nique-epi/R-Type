#include "ClientException.hpp"
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

namespace rtype::client {

namespace {

std::string joinedFolders(const std::vector<std::string>& folders) {
  std::string joined;
  for (const std::string& folder : folders) {
    if (!joined.empty()) {
      joined += ", ";
    }
    joined += folder;
  }
  return joined;
}

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

UnknownExecutablePathException::UnknownExecutablePathException()
    : ClientException(
          "The system did not tell where the client executable is, so its "
          "assets folder cannot be found") {}

AssetFolderNotFoundException::AssetFolderNotFoundException(
    const std::vector<std::string>& searchedFolders)
    : ClientException("No assets folder found, searched: " +
                      joinedFolders(searchedFolders)) {}

AssetLoadingException::AssetLoadingException(std::string_view assetFolder,
                                             const AssetProblems& problems)
    : ClientException(assetLoadingMessage(assetFolder, problems)) {}

AssetNotLoadedException::AssetNotLoadedException(std::string_view assetId)
    : ClientException("Asset \"" + std::string(assetId) +
                      "\" is not loaded: list it in the assets of the screen "
                      "that uses it") {}

}  // namespace rtype::client

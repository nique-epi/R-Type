#include "SessionTokenFile.hpp"
#include <filesystem>
#include <optional>
#include <string>
#include <system_error>
#include <utility>
#include "PathText.hpp"
#include "PlayerFile.hpp"
#include "PlayerFileWrite.hpp"
#include "SessionTokenFileConstants.hpp"

namespace rtype::client {

SessionTokenFile::SessionTokenFile(std::optional<std::filesystem::path> path)
    : path_(std::move(path)), logger_(std::string{ACCOUNT_LOGGER_NAME}) {}

std::optional<std::string> SessionTokenFile::load() const {
  if (!path_.has_value()) {
    return std::nullopt;
  }
  std::optional<std::string> token =
      readPlayerFile(*path_, MAXIMUM_SESSION_TOKEN_FILE_SIZE, logger_);
  if (token.has_value() && token->empty()) {
    return std::nullopt;
  }
  return token;
}

void SessionTokenFile::keep(const std::string& token) {
  if (!path_.has_value()) {
    return;
  }
  static_cast<void>(replacePlayerFile(
      PlayerFileWrite{.file = *path_,
                      .text = token,
                      .permissions = SESSION_TOKEN_FILE_PERMISSIONS},
      logger_));
}

void SessionTokenFile::forget() {
  if (!path_.has_value()) {
    return;
  }
  std::error_code error;
  std::filesystem::remove(*path_, error);
  if (error) {
    logger_.warn("cannot remove ", logging::genericText(*path_), ": ",
                 error.message());
  }
}

}  // namespace rtype::client

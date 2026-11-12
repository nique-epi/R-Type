#pragma once

#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

namespace rtype::client {

/** @brief The asset ids and files a loading step could not load, by reason. */
struct AssetProblems {
  std::vector<std::string> invalidIds;
  std::vector<std::string> missingFiles;
  std::vector<std::string> unreadableFiles;
};

/**
 * @brief Root of every error raised by the client.
 */
class ClientException : public std::runtime_error {
 public:
  explicit ClientException(const std::string& message);
};

/** @brief A sheet of a content pack names a sprite sheet the pack does not
 * hold: a path outside its folder, or a file it left out. */
class ForeignSpriteSheetException : public ClientException {
 public:
  explicit ForeignSpriteSheetException(const std::string& sheet);
};

/** @brief A file of a content pack could not be written below the folder
 * the client keeps packs in. */
class ContentFileWriteException : public ClientException {
 public:
  explicit ContentFileWriteException(const std::string& file);
};

/** @brief A loading step ended with assets it could not load. */
class AssetLoadingException : public ClientException {
 public:
  AssetLoadingException(std::string_view assetFolder,
                        const AssetProblems& problems);
};

/** @brief An asset was asked for before being loaded. */
class AssetNotLoadedException : public ClientException {
 public:
  explicit AssetNotLoadedException(std::string_view assetId);
};

/**
 * @brief A render texture could not be created on the graphics card.
 */
class RenderTextureNotCreatedException : public ClientException {
 public:
  /** @param width,height Size asked for, in pixels. */
  RenderTextureNotCreatedException(unsigned int width, unsigned int height);
};

/** @brief Two modules registered a screen under the same identifier. */
class DuplicateScreenException : public ClientException {
 public:
  explicit DuplicateScreenException(std::string_view screenName);
};

/**
 * @brief The nickname given at launch is not one the protocol accepts: an
 * error of the arguments, unlike network::InvalidNicknameException, which
 * reports a malformed message.
 */
class InvalidNicknameOptionException : public ClientException {
 public:
  /** @param nickname The value given after NICKNAME_OPTION. */
  explicit InvalidNicknameOptionException(std::string_view nickname);
};

/**
 * @brief The game given at launch is neither NEW_GAME_VALUE nor the number of
 * a game, network::FIRST_HOSTED_GAME_NUMBER to 65535.
 */
class InvalidGameOptionException : public ClientException {
 public:
  /** @param game The value given after GAME_OPTION. */
  explicit InvalidGameOptionException(std::string_view game);
};

/**
 * @brief Velopack could not check, download or apply an update of the client,
 * or the client was not installed by Velopack.
 */
class UpdateException : public ClientException {
 public:
  /** @param step What was being done, as "check" or "download".
   * @param reason What Velopack answered. */
  UpdateException(std::string_view step, std::string_view reason);
};

}  // namespace rtype::client

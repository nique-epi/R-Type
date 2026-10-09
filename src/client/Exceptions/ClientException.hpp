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

/** @brief The system did not tell where the running executable is. */
class UnknownExecutablePathException : public ClientException {
 public:
  UnknownExecutablePathException();
};

/** @brief None of the searched folders is an assets folder. */
class AssetFolderNotFoundException : public ClientException {
 public:
  explicit AssetFolderNotFoundException(
      const std::vector<std::string>& searchedFolders);
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

}  // namespace rtype::client

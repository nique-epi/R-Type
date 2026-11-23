#include "PlayerFile.hpp"
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <ios>
#include <iterator>
#include <optional>
#include <string>
#include <system_error>
#include "Logger.hpp"
#include "PathText.hpp"
#include "PlayerFileConstants.hpp"
#include "PlayerFileWrite.hpp"

namespace rtype::client {

namespace {

/** @returns Whether @p folder exists, created open to the player only when
 * it did not; a failure is logged on @p logger. */
bool folderReady(const std::filesystem::path& folder,
                 const logging::Logger& logger) {
  std::error_code error;
  if (std::filesystem::create_directories(folder, error)) {
    std::filesystem::permissions(folder, PLAYER_FOLDER_PERMISSIONS, error);
  }
  if (error) {
    logger.warn("cannot create the folder ", logging::genericText(folder), ": ",
                error.message());
    return false;
  }
  return true;
}

/** @brief Writes the text of @p write to @p file, replacing it.
 * @returns Whether the permissions were given and the whole text written. */
bool writeFile(const std::filesystem::path& file,
               const PlayerFileWrite& write) {
  std::ofstream stream(file, std::ios::binary | std::ios::trunc);
  if (write.permissions.has_value()) {
    std::error_code error;
    std::filesystem::permissions(file, *write.permissions, error);
    if (error) {
      return false;
    }
  }
  stream << write.text;
  stream.close();
  return !stream.fail();
}

}  // namespace

std::optional<std::string> readPlayerFile(const std::filesystem::path& file,
                                          std::uintmax_t maximumSize,
                                          const logging::Logger& logger) {
  std::error_code error;
  const std::uintmax_t size = std::filesystem::file_size(file, error);
  if (error) {
    return std::nullopt;
  }
  if (size > maximumSize) {
    logger.warn("file too large, ignored: ", logging::genericText(file));
    return std::nullopt;
  }
  std::ifstream stream(file, std::ios::binary);
  std::string text((std::istreambuf_iterator<char>(stream)),
                   std::istreambuf_iterator<char>());
  if (!stream.good() && !stream.eof()) {
    logger.warn("file unreadable, ignored: ", logging::genericText(file));
    return std::nullopt;
  }
  return text;
}

bool replacePlayerFile(const PlayerFileWrite& write,
                       const logging::Logger& logger) {
  if (!folderReady(write.file.parent_path(), logger)) {
    return false;
  }
  std::filesystem::path temporary = write.file;
  temporary += TEMPORARY_PLAYER_FILE_SUFFIX;
  std::error_code error;
  if (!writeFile(temporary, write)) {
    logger.warn("cannot write ", logging::genericText(temporary));
    std::filesystem::remove(temporary, error);
    return false;
  }
  std::filesystem::rename(temporary, write.file, error);
  if (error) {
    logger.warn("cannot replace ", logging::genericText(write.file), ": ",
                error.message());
    std::filesystem::remove(temporary, error);
    return false;
  }
  return true;
}

}  // namespace rtype::client

#pragma once

#include <filesystem>
#include <optional>
#include <string_view>

namespace rtype::client {

/** @brief What replacePlayerFile() writes: the whole text of a file of the
 * player, and who may read it. */
struct PlayerFileWrite {
  std::filesystem::path file;
  std::string_view text;
  /** @brief The permissions the file is given before the text is written;
   * nothing leaves those the system gives a new file. */
  std::optional<std::filesystem::perms> permissions;
};

}  // namespace rtype::client

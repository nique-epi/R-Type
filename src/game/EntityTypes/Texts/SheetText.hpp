#pragma once

#include <optional>
#include <string>
#include "SheetMaskText.hpp"

namespace rtype::game {

/** @brief One entity sheet as it was read, before anything is checked. */
struct SheetText {
  /** @brief The name of the type: the name of the file, without `.toml`. */
  std::string name;
  /** @brief How the errors name the file: its absolute path. */
  std::string source;
  /** @brief Everything the file holds. */
  std::string text;
  /** @brief The mask file next to the sheet, MASK_FOLDER_OF_SHEETS/<name>
   * MASK_FILE_EXTENSION, when there is one: the sheet reads its pixels from
   * it, and the fingerprint of the sheets covers it. */
  std::optional<SheetMaskText> mask = std::nullopt;
};

}  // namespace rtype::game

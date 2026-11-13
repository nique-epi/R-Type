#pragma once

#include <string_view>

namespace rtype::server {

/** @brief The text of the two files of `/proc` a Linux machine counts its
 * processors and its memory in; it owns neither. */
struct ProcTexts {
  /** @brief The text of `/proc/stat`. */
  std::string_view processors;
  /** @brief The text of `/proc/meminfo`. */
  std::string_view memory;
};

}  // namespace rtype::server

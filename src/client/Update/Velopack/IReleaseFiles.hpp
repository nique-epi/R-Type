#pragma once

#include <cstddef>
#include <optional>
#include <string>
#include <string_view>
#include "ReleaseDownload.hpp"

namespace rtype::client {

/** @brief The files of the releases of the game, as their origin serves them.
 * Velopack may call it from several threads at once. */
class IReleaseFiles {
 public:
  virtual ~IReleaseFiles() = default;

  /** @returns The text at @p path of the origin, or nothing when it could not
   * be read whole or holds more than @p limit bytes. */
  [[nodiscard]] virtual std::optional<std::string> text(std::string_view path,
                                                        std::size_t limit) = 0;

  /** @brief Writes the file of @p download, never more than its size.
   * @returns Whether it was written whole. */
  [[nodiscard]] virtual bool save(const ReleaseDownload& download) = 0;

  /** @brief Ends every request in progress and every later one, so the
   * threads waiting on them can be joined at once. */
  virtual void stop() = 0;
};

}  // namespace rtype::client

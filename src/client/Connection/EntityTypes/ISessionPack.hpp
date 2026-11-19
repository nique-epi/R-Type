#pragma once

#include <filesystem>
#include <memory>
#include <optional>
#include <string_view>
#include "BackgroundCatalog.hpp"

namespace rtype::client {

/** @brief What the pack of the server of the session brings beside its
 * sheets: the backgrounds it lays over those of the client, and the files it
 * wrote below its folder. */
class ISessionPack {
 public:
  virtual ~ISessionPack() = default;

  /**
   * @returns The backgrounds the pack of the server brings; nullptr outside
   * a session, when the server plays the client's own content, and for a
   * pack without a background. They are shared, so what shows them keeps
   * them alive once the session is over.
   */
  [[nodiscard]] virtual std::shared_ptr<const BackgroundCatalog> backgrounds()
      const = 0;

  /**
   * @returns Where the file at @p path of the pack of the server lies; nothing
   * outside a session and when the server plays the client's own content.
   * @param path A path of the pack, one network::isContentPath() accepts, so
   * the file lies below the folder of the pack.
   */
  [[nodiscard]] virtual std::optional<std::filesystem::path> packFileOf(
      std::string_view path) const = 0;
};

}  // namespace rtype::client

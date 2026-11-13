#pragma once

#include <functional>
#include <set>
#include <string>
#include <string_view>
#include "Logger.hpp"

namespace sf {
class Texture;
}  // namespace sf

namespace rtype::client {

class ITextureSource;

/**
 * @brief Finds textures in a texture source, and writes in the log, once per
 * asset id, each id it was asked for and did not find.
 *
 * The source must outlive it.
 */
class ReportingTextureSource {
 public:
  /** @param loggerName The name of the log the missing ids are written to. */
  ReportingTextureSource(const ITextureSource& textures,
                         std::string_view loggerName);

  /** @returns The texture of @p assetId, or nullptr once the log says it is
   * missing; a missing id is reported once. */
  [[nodiscard]] const sf::Texture* found(std::string_view assetId);

 private:
  const ITextureSource* textures_;
  logging::Logger logger_;
  std::set<std::string, std::less<>> reportedMissingIds_;
};

}  // namespace rtype::client

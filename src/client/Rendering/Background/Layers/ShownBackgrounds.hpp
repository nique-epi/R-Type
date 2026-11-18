#pragma once

#include <functional>
#include <map>
#include <memory>
#include <string>
#include <string_view>
#include "BackgroundCatalog.hpp"
#include "BackgroundPace.hpp"
#include "LayerSources.hpp"
#include "LayeredBackground.hpp"
#include "Logger.hpp"
#include "SkyLevel.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @brief Which background the sky shows, and which one is leaving.
 *
 * Every background of the catalog is built once, when this is, so a level
 * that enters a biome reads no file. The background shown is the one named
 * after the biome of the level, DEFAULT_BACKGROUND_NAME for a level without
 * a biome, and for a biome the catalog lacks, told once in the journal. A
 * background that has an entrance arrives over the one it replaces, which
 * keeps moving behind it until the entrance has left the screen; one without
 * replaces it at once. A background shown again starts over.
 *
 * The backgrounds of the pack of a server may be laid over those of the
 * catalog for a session (see layOver()): one of the same name replaces it,
 * the others are added.
 *
 * The scroll the server tells is kept, then moved on at the speed it tells
 * until the next one comes, so the layers move between two messages and stop
 * while the scroll of the level is paused. It refers to the catalog, which
 * must outlive it.
 */
class ShownBackgrounds {
 public:
  ShownBackgrounds(const BackgroundCatalog& catalog, LayerSources sources);

  /** @brief Shows the background of the biome of @p level, where its scroll
   * puts it, from now. */
  void show(const SkyLevel& level);

  /** @brief Moves the backgrounds on by @p elapsed, capped at
   * MAXIMUM_BACKGROUND_STEP. A negative time moves nothing. */
  void advance(engine::Duration elapsed);

  /** @brief From the next advance, the layers that warp move @p factor
   * times their own speed, 0 at least. */
  void setSpeedFactor(float factor);

  /** @returns The background shown; nullptr when the catalog has neither
   * the one asked for nor the default one. */
  [[nodiscard]] const LayeredBackground* shown() const;

  /** @returns The background that is leaving behind the one shown; nullptr
   * when none is. */
  [[nodiscard]] const LayeredBackground* leaving() const;

  /**
   * @brief Lays the backgrounds of @p catalog over those of the catalog
   * given at construction, in place of those laid before, and builds them
   * now; nullptr only takes off what was laid. The biome of the level then
   * shows again whatever names it: a background that changes starts over
   * with none leaving behind it, one that stays the same goes on. Laying
   * what is laid already does nothing.
   */
  void layOver(std::shared_ptr<const BackgroundCatalog> catalog);

  /** @returns The catalog laid over; nullptr when none is. */
  [[nodiscard]] const BackgroundCatalog* laidOver() const;

 private:
  /** @returns The background of the biome @p biome, as the class says. */
  [[nodiscard]] LayeredBackground* backgroundOf(std::string_view biome);

  /** @returns The background named @p name, among those laid over first;
   * nullptr when neither has one. */
  [[nodiscard]] LayeredBackground* builtNamed(std::string_view name);

  /** @returns What moves the background shown. */
  [[nodiscard]] BackgroundPace paceOfTheShown() const;

  LayerSources sources_;
  std::map<std::string, LayeredBackground, std::less<>> built_;
  /** @brief Kept before what is built from it, so it outlives it. */
  std::shared_ptr<const BackgroundCatalog> laidOverCatalog_;
  std::map<std::string, LayeredBackground, std::less<>> laidOver_;
  logging::Logger logger_;
  LayeredBackground* shown_{};
  LayeredBackground* leaving_{};
  SkyLevel level_;
  float speedFactor_{1.0F};
  float leavingScrolled_{};
  std::string unknownBiome_;
};

}  // namespace rtype::client

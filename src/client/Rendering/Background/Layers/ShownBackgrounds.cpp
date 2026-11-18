#include "ShownBackgrounds.hpp"
#include <algorithm>
#include <chrono>
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <string_view>
#include <utility>
#include "BackgroundCatalog.hpp"
#include "BackgroundConstants.hpp"
#include "BackgroundDescription.hpp"
#include "BackgroundPace.hpp"
#include "LayerSources.hpp"
#include "LayeredBackground.hpp"
#include "PixelGridConstants.hpp"
#include "SkyLevel.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

namespace {

/** @returns How many background pixels @p level scrolled since its biome
 * began. */
float scrolledBy(const SkyLevel& level) {
  return (level.scrollPosition - level.biomeSince) / BACKGROUND_PIXEL_SIZE;
}

/** @brief Builds into @p built every background of @p catalog from
 * @p sources. */
void buildInto(std::map<std::string, LayeredBackground, std::less<>>& built,
               const BackgroundCatalog& catalog, const LayerSources& sources) {
  for (const std::string& name : catalog.names()) {
    const BackgroundDescription* const description = catalog.named(name);
    if (description != nullptr) {
      built.try_emplace(name, *description, sources);
    }
  }
}

}  // namespace

ShownBackgrounds::ShownBackgrounds(const BackgroundCatalog& catalog,
                                   LayerSources sources)
    : sources_(std::move(sources)),
      logger_(std::string{BACKGROUND_LOGGER_NAME}) {
  buildInto(built_, catalog, sources_);
  shown_ = backgroundOf(level_.biome);
}

void ShownBackgrounds::show(const SkyLevel& level) {
  LayeredBackground* const next = backgroundOf(level.biome);
  if (next != shown_) {
    const bool arrives = next != nullptr && next->entranceLayer().has_value();
    leaving_ = arrives ? shown_ : nullptr;
    leavingScrolled_ = scrolledBy(level_);
    shown_ = next;
    if (shown_ != nullptr) {
      shown_->restart();
    }
  }
  level_ = level;
}

void ShownBackgrounds::advance(engine::Duration elapsed) {
  const engine::Duration step =
      std::clamp(elapsed, engine::Duration::zero(), MAXIMUM_BACKGROUND_STEP);
  level_.scrollPosition +=
      level_.scrollSpeed * std::chrono::duration<float>(step).count();
  if (leaving_ != nullptr) {
    leaving_->advance(step, BackgroundPace{.scrolled = leavingScrolled_,
                                           .speedFactor = speedFactor_});
  }
  if (shown_ == nullptr) {
    return;
  }
  shown_->advance(step, paceOfTheShown());
  if (shown_->entranceColumn().value_or(ENTRANCE_GONE_COLUMN - 1) <
      ENTRANCE_GONE_COLUMN) {
    leaving_ = nullptr;
  }
}

void ShownBackgrounds::setSpeedFactor(float factor) {
  speedFactor_ = std::max(factor, 0.0F);
}

const LayeredBackground* ShownBackgrounds::shown() const { return shown_; }

const LayeredBackground* ShownBackgrounds::leaving() const { return leaving_; }

void ShownBackgrounds::layOver(
    std::shared_ptr<const BackgroundCatalog> catalog) {
  if (catalog == laidOverCatalog_) {
    return;
  }
  const bool showsItsOwn = std::ranges::any_of(
      built_,
      [this](const auto& background) { return &background.second == shown_; });
  laidOver_.clear();
  laidOverCatalog_ = std::move(catalog);
  if (laidOverCatalog_ != nullptr) {
    buildInto(laidOver_, *laidOverCatalog_, sources_);
  }
  leaving_ = nullptr;
  LayeredBackground* const next = backgroundOf(level_.biome);
  if (showsItsOwn && next == shown_) {
    return;
  }
  shown_ = next;
  if (shown_ != nullptr) {
    shown_->restart();
  }
}

const BackgroundCatalog* ShownBackgrounds::laidOver() const {
  return laidOverCatalog_.get();
}

LayeredBackground* ShownBackgrounds::backgroundOf(std::string_view biome) {
  const std::string_view name = biome.empty() ? DEFAULT_BACKGROUND_NAME : biome;
  LayeredBackground* const found = builtNamed(name);
  if (found != nullptr) {
    return found;
  }
  if (unknownBiome_ != name) {
    unknownBiome_ = name;
    logger_.warn("no background is named ", name, ": the sky shows ",
                 DEFAULT_BACKGROUND_NAME);
  }
  return builtNamed(DEFAULT_BACKGROUND_NAME);
}

LayeredBackground* ShownBackgrounds::builtNamed(std::string_view name) {
  const auto laid = laidOver_.find(name);
  if (laid != laidOver_.end()) {
    return &laid->second;
  }
  const auto own = built_.find(name);
  return own == built_.end() ? nullptr : &own->second;
}

BackgroundPace ShownBackgrounds::paceOfTheShown() const {
  return BackgroundPace{.scrolled = scrolledBy(level_),
                        .speedFactor = speedFactor_};
}

}  // namespace rtype::client

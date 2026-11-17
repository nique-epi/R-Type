#include "ShownBackgrounds.hpp"
#include <algorithm>
#include <chrono>
#include <string>
#include <string_view>
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

}  // namespace

ShownBackgrounds::ShownBackgrounds(const BackgroundCatalog& catalog,
                                   const LayerSources& sources)
    : logger_(std::string{BACKGROUND_LOGGER_NAME}) {
  for (const std::string& name : catalog.names()) {
    const BackgroundDescription* const description = catalog.named(name);
    if (description != nullptr) {
      built_.try_emplace(name, *description, sources);
    }
  }
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

LayeredBackground* ShownBackgrounds::backgroundOf(std::string_view biome) {
  const std::string_view name = biome.empty() ? DEFAULT_BACKGROUND_NAME : biome;
  const auto found = built_.find(name);
  if (found != built_.end()) {
    return &found->second;
  }
  if (unknownBiome_ != name) {
    unknownBiome_ = name;
    logger_.warn("no background is named ", name, ": the sky shows ",
                 DEFAULT_BACKGROUND_NAME);
  }
  const auto fallback = built_.find(DEFAULT_BACKGROUND_NAME);
  return fallback == built_.end() ? nullptr : &fallback->second;
}

BackgroundPace ShownBackgrounds::paceOfTheShown() const {
  return BackgroundPace{.scrolled = scrolledBy(level_),
                        .speedFactor = speedFactor_};
}

}  // namespace rtype::client

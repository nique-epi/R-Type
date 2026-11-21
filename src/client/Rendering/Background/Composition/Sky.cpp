#include "Sky.hpp"
#include <SFML/Graphics/BlendMode.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/System/Vector2.hpp>
#include <cstddef>
#include <memory>
#include <optional>
#include <string>
#include <utility>
#include "BackgroundCatalog.hpp"
#include "BackgroundColorConstants.hpp"
#include "BackgroundDescription.hpp"
#include "LayerSources.hpp"
#include "LayeredBackground.hpp"
#include "PixelGridConstants.hpp"
#include "PlanePlacement.hpp"
#include "SeamCover.hpp"
#include "SkyLevel.hpp"
#include "SkySources.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

Sky::Sky(const BackgroundCatalog& catalog, SkySources sources)
    : backgrounds_(catalog, LayerSources{.fileOf = std::move(sources.fileOf),
                                         .seed = sources.seed}),
      catalog_(&catalog),
      canvas_(std::move(sources.textureOf)),
      picture_(0) {}

void Sky::advance(engine::Duration elapsed) { backgrounds_.advance(elapsed); }

void Sky::setSpeedFactor(float factor) { backgrounds_.setSpeedFactor(factor); }

void Sky::setOffset(sf::Vector2i pixels) { offset_ = pixels; }

void Sky::showLevel(const SkyLevel& level) {
  biome_ = level.biome;
  backgrounds_.show(level);
}

std::optional<std::string> Sky::biomeMusic() const {
  const BackgroundDescription* shown =
      laidOver_ == nullptr ? nullptr : laidOver_->named(biome_);
  if (shown == nullptr) {
    shown = catalog_->named(biome_);
  }
  return shown == nullptr ? std::nullopt : shown->music;
}

void Sky::layOver(std::shared_ptr<const BackgroundCatalog> backgrounds) {
  laidOver_ = backgrounds;
  backgrounds_.layOver(std::move(backgrounds));
}

void Sky::draw(sf::RenderTarget& target) {
  const LayeredBackground* const leaving = backgrounds_.leaving();
  const LayeredBackground* const shown = backgrounds_.shown();
  const std::size_t planesOfTheLeaving =
      leaving == nullptr ? 0 : leaving->layerCount();
  picture_.holdPlanes(planesOfTheLeaving +
                      (shown == nullptr ? 0 : shown->layerCount()));
  if (leaving != nullptr) {
    paintLayers(*leaving, 0);
  }
  if (shown != nullptr) {
    paintLayers(*shown, planesOfTheLeaving);
  }
  picture_.show(target);
  drawHalo(target);
}

void Sky::paintLayers(const LayeredBackground& background,
                      std::size_t firstPlane) {
  const std::optional<std::size_t> entrance = background.entranceLayer();
  const std::optional<int> seam = background.entranceColumn();
  for (std::size_t layer = 0; layer < background.layerCount(); ++layer) {
    const std::size_t plane = firstPlane + layer;
    sf::RenderTexture& surface = picture_.paintPlane(
        plane, PlanePlacement{.cameraOffset = offset_,
                              .shift = background.shiftOf(layer)});
    const bool behind =
        entrance.has_value() && seam.has_value() && layer < *entrance;
    if (behind && hidesTheWholeCanvas(surface, *seam)) {
      picture_.leavePlaneEmpty(plane);
      continue;
    }
    canvas_.paintOn(surface);
    background.paint(layer, canvas_);
    if (!canvas_.flush()) {
      picture_.leavePlaneEmpty(plane);
      continue;
    }
    if (behind) {
      entranceMask_.cover(surface, *seam);
    }
  }
}

void Sky::drawHalo(sf::RenderTarget& target) {
  if (!halo_.isAvailable()) {
    return;
  }
  sf::Sprite halo(halo_.apply(picture_.flatPicture()));
  halo.setScale({BACKGROUND_PIXEL_SIZE, BACKGROUND_PIXEL_SIZE});
  halo.setColor(HALO_TINT);
  target.draw(halo, sf::BlendAdd);
}

}  // namespace rtype::client

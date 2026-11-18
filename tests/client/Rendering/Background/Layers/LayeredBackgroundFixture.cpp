#include "LayeredBackgroundFixture.hpp"
#include <cstddef>
#include <string_view>
#include "BackgroundCatalog.hpp"
#include "BackgroundCatalogFixture.hpp"
#include "BackgroundDescription.hpp"
#include "BackgroundReadingFixtures.hpp"
#include "LayerSources.hpp"
#include "LayerTestConstants.hpp"
#include "LayeredBackground.hpp"
#include "LayeredBackgroundTestConstants.hpp"
#include "LdtkProjectFileFixtures.hpp"
#include "RecordingLayerCanvas.hpp"

using rtype::client::BackgroundDescription;
using rtype::client::LayeredBackground;
using rtype::client::LayerSources;

LayerSources shippedSources() {
  return LayerSources{.fileOf = shippedFile, .seed = LAYER_SEED};
}

LayeredBackgroundFixture::LayeredBackgroundFixture()
    : sky_(backgroundReading(A_SMALL_SKY)), shipped_(shippedBackgrounds()) {}

LayeredBackground LayeredBackgroundFixture::smallSky() const {
  return LayeredBackground{sky_, shippedSources()};
}

LayeredBackground LayeredBackgroundFixture::shipped(
    std::string_view name) const {
  const BackgroundDescription* const description = shipped_.named(name);
  return LayeredBackground{description == nullptr ? sky_ : *description,
                           shippedSources()};
}

const BackgroundDescription& LayeredBackgroundFixture::smallSkyDescription()
    const {
  return sky_;
}

const BackgroundDescription& LayeredBackgroundFixture::pitDescription() const {
  const BackgroundDescription* const pit = shipped_.named(THE_PIT);
  return pit == nullptr ? sky_ : *pit;
}

float whereLies(const LayeredBackground& background, std::size_t layer) {
  return static_cast<float>(background.frameOf(layer).column) +
         background.shiftOf(layer);
}

std::size_t paintedFrom(const LayeredBackground& background,
                        std::size_t first) {
  RecordingLayerCanvas canvas;
  for (std::size_t layer = first; layer < background.layerCount(); ++layer) {
    background.paint(layer, canvas);
  }
  return canvas.plots().size() + canvas.frames().size();
}

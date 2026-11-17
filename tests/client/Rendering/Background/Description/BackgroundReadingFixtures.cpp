#include "BackgroundReadingFixtures.hpp"
#include <cstddef>
#include <string>
#include <string_view>
#include "BackgroundDescription.hpp"
#include "BackgroundReading.hpp"
#include "BackgroundReadingTestConstants.hpp"
#include "BackgroundText.hpp"
#include "LayerDescription.hpp"
#include "ParticleContent.hpp"

using rtype::client::BackgroundDescription;
using rtype::client::BackgroundText;
using rtype::client::LayerDescription;
using rtype::client::ParticleContent;
using rtype::client::readBackground;

BackgroundDescription backgroundReading(std::string_view text) {
  return readBackground(
      BackgroundText{.name = std::string{READ_BACKGROUND_NAME},
                     .source = std::string{READ_BACKGROUND_FILE},
                     .text = std::string{text}});
}

LayerDescription layerReading(std::string_view keys) {
  const BackgroundDescription background =
      backgroundReading(std::string{LAYER_START} + std::string{keys});
  return background.layers.empty() ? LayerDescription{}
                                   : background.layers.front();
}

ParticleContent particlesReading(std::string_view keys) {
  const BackgroundDescription background =
      backgroundReading(std::string{PARTICLE_LAYER_START} + std::string{keys});
  return background.layers.empty()
             ? ParticleContent{}
             : contentOf<ParticleContent>(background.layers.front());
}

std::string layersCounting(std::size_t count) {
  std::string text;
  for (std::size_t layer = 0; layer < count; ++layer) {
    text += LAYER_START;
    text += AN_IMAGE_WITHOUT_A_PLACE;
  }
  return text;
}

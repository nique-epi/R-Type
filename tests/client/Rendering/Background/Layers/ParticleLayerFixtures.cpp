#include "ParticleLayerFixtures.hpp"
#include <cstddef>
#include <string_view>
#include <vector>
#include "BackgroundReadingFixtures.hpp"
#include "ColoredPixel.hpp"
#include "LayerFrame.hpp"
#include "LayerTestConstants.hpp"
#include "Particle.hpp"
#include "ParticleLayer.hpp"
#include "ParticleLayerParts.hpp"
#include "RecordedFrame.hpp"
#include "RecordingLayerCanvas.hpp"

using rtype::client::ColoredPixel;
using rtype::client::LayerFrame;
using rtype::client::Particle;
using rtype::client::ParticleLayer;
using rtype::client::ParticleLayerParts;

ParticleLayer particleLayerOf(std::string_view keys, float repeat) {
  return ParticleLayer{ParticleLayerParts{.description = particlesReading(keys),
                                          .repeat = repeat,
                                          .seed = LAYER_SEED}};
}

std::vector<ColoredPixel> pixelsPaintedBy(const ParticleLayer& layer,
                                          const LayerFrame& frame) {
  RecordingLayerCanvas canvas;
  layer.paint(canvas, frame);
  return canvas.plots();
}

std::vector<RecordedFrame> spritesPaintedBy(const ParticleLayer& layer,
                                            const LayerFrame& frame) {
  RecordingLayerCanvas canvas;
  layer.paint(canvas, frame);
  return canvas.frames();
}

int particlesDrawnWith(const ParticleLayer& layer, std::size_t sprite) {
  int drawn = 0;
  for (const Particle& particle : layer.particles()) {
    if (particle.sprite == sprite) {
      ++drawn;
    }
  }
  return drawn;
}

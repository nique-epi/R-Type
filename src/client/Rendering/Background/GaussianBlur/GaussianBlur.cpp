#include "GaussianBlur.hpp"
#include <SFML/Graphics/BlendMode.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Glsl.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/Shader.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <string>
#include "BackgroundConstants.hpp"
#include "ClientException.hpp"

namespace rtype::client {

GaussianBlur::GaussianBlur()
    : logger_(std::string(BACKGROUND_LOG_MODULE_NAME)),
      available_(sf::Shader::isAvailable()) {
  createPass(horizontalPass_);
  createPass(verticalPass_);
  verticalPass_.setSmooth(true);
  if (!available_) {
    logger_.warn("shaders are not available: the sky is drawn without halo");
    return;
  }
  const std::string source = std::string(HALO_KERNEL_RADIUS_DEFINITION) +
                             std::to_string(HALO_KERNEL_RADIUS) +
                             std::string(HALO_FRAGMENT_SHADER);
  if (!shader_.loadFromMemory(source, sf::Shader::Type::Fragment)) {
    available_ = false;
    logger_.error(
        "the halo shader did not compile: the sky is drawn without halo");
    return;
  }
  shader_.setUniform(std::string(HALO_SOURCE_UNIFORM),
                     sf::Shader::CurrentTexture);
  shader_.setUniform(std::string(HALO_DEVIATION_UNIFORM),
                     HALO_STANDARD_DEVIATION);
}

bool GaussianBlur::isAvailable() const { return available_; }

const sf::Texture& GaussianBlur::apply(const sf::Texture& source) {
  const sf::Glsl::Vec2 texel(1.0F / static_cast<float>(source.getSize().x),
                             1.0F / static_cast<float>(source.getSize().y));
  blurInto(horizontalPass_, source, {texel.x, 0.0F});
  blurInto(verticalPass_, horizontalPass_.getTexture(), {0.0F, texel.y});
  return verticalPass_.getTexture();
}

void GaussianBlur::createPass(sf::RenderTexture& pass) {
  if (!pass.resize(BACKGROUND_SIZE)) {
    throw RenderTextureNotCreatedException(BACKGROUND_SIZE.x,
                                           BACKGROUND_SIZE.y);
  }
}

void GaussianBlur::blurInto(sf::RenderTexture& pass, const sf::Texture& source,
                            sf::Glsl::Vec2 texelStep) {
  shader_.setUniform(std::string(HALO_DIRECTION_UNIFORM), texelStep);
  sf::RenderStates states(&shader_);
  states.blendMode = sf::BlendNone;
  pass.clear(sf::Color::Transparent);
  pass.draw(sf::Sprite(source), states);
  pass.display();
}

}  // namespace rtype::client

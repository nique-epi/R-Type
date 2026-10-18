#include "FragmentShader.hpp"
#include <SFML/Graphics/Shader.hpp>
#include <optional>
#include <string>
#include "Logger.hpp"

namespace rtype::client {

std::optional<sf::Shader> loadFragmentShader(
    const FragmentShaderDescription& shader, const logging::Logger& logger) {
  if (!sf::Shader::isAvailable()) {
    logger.warn(shader.unavailableMessage);
    return std::nullopt;
  }
  sf::Shader compiled;
  if (!compiled.loadFromMemory(shader.source, sf::Shader::Type::Fragment)) {
    logger.error(shader.notCompiledMessage);
    return std::nullopt;
  }
  compiled.setUniform(std::string(shader.textureUniform),
                      sf::Shader::CurrentTexture);
  return compiled;
}

}  // namespace rtype::client

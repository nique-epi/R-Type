#pragma once

#include <SFML/Graphics/Shader.hpp>
#include <optional>
#include <string_view>

namespace rtype::logging {

class Logger;

}  // namespace rtype::logging

namespace rtype::client {

/**
 * @brief A fragment shader that reads the texture of what it draws, and the
 * two lines the log writes when it cannot be used, each saying how the drawing
 * looks without it.
 */
struct FragmentShaderDescription {
  /** @brief Source code of the shader, in GLSL. */
  std::string_view source;
  /** @brief Name of its sampler uniform, set to sf::Shader::CurrentTexture,
   * the texture being drawn. */
  std::string_view textureUniform;
  /** @brief Logged as a warning when the graphics card has no shaders. */
  std::string_view unavailableMessage;
  /** @brief Logged as an error when the source does not compile. */
  std::string_view notCompiledMessage;
};

/**
 * @returns The shader compiled from the description, its texture uniform set
 * to the texture being drawn; nothing when the graphics card has no shaders or
 * the source does not compile, each case logged by `logger` with its message.
 * It needs an OpenGL context, as any sf::Shader does.
 */
[[nodiscard]] std::optional<sf::Shader> loadFragmentShader(
    const FragmentShaderDescription& shader, const logging::Logger& logger);

}  // namespace rtype::client

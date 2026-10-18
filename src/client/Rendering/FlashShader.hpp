#pragma once

#include <string_view>
#include "FragmentShader.hpp"

namespace rtype::client {

/**
 * @brief Name of the uniform of FLASH_FRAGMENT_SHADER that holds the texture
 * of the sprite; SfmlDrawSurface sets it to sf::Shader::CurrentTexture, the
 * sheet of the sprite being drawn.
 */
constexpr std::string_view FLASH_SPRITE_UNIFORM = "sprite";

/**
 * @brief Source code, in GLSL, of the fragment shader that paints a sprite as
 * a silhouette: every pixel takes the color SFML gives the vertices (the color
 * of the sprite), with the transparency of the texture, so the shape stays and
 * the colors of the sheet go.
 */
constexpr std::string_view FLASH_FRAGMENT_SHADER = R"glsl(
uniform sampler2D sprite;

void main()
{
    float alpha = texture2D(sprite, gl_TexCoord[0].xy).a;
    gl_FragColor = vec4(gl_Color.rgb, gl_Color.a * alpha);
}
)glsl";

/** @brief The flash shader, and what the log says when a hit is drawn
 * without it. */
constexpr FragmentShaderDescription FLASH_SHADER{
    .source = FLASH_FRAGMENT_SHADER,
    .textureUniform = FLASH_SPRITE_UNIFORM,
    .unavailableMessage =
        "shaders are not available: a hit is drawn brightened",
    .notCompiledMessage =
        "the flash shader did not compile: a hit is drawn brightened"};

}  // namespace rtype::client

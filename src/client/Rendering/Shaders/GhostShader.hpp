#pragma once

#include <string_view>
#include "FragmentShader.hpp"

namespace rtype::client {

/**
 * @brief Name of the uniform of GHOST_FRAGMENT_SHADER that holds the texture
 * of the sprite; SfmlDrawSurface sets it to sf::Shader::CurrentTexture.
 */
constexpr std::string_view GHOST_SPRITE_UNIFORM = "sprite";

/**
 * @brief Source code, in GLSL, of the fragment shader that draws a sprite
 * dithered: a pixel of the canvas keeps its color when its column plus its
 * row is even, and is left transparent otherwise. gl_FragCoord counts the
 * pixels of the target, the canvas of the entities, so the checkerboard lies
 * on the pixel grid wherever the sprite moves.
 */
constexpr std::string_view GHOST_FRAGMENT_SHADER = R"glsl(
uniform sampler2D sprite;

void main()
{
    vec4 pixel = texture2D(sprite, gl_TexCoord[0].xy) * gl_Color;
    float hidden = mod(floor(gl_FragCoord.x) + floor(gl_FragCoord.y), 2.0);
    gl_FragColor = vec4(pixel.rgb, pixel.a * (1.0 - hidden));
}
)glsl";

/** @brief The ghost shader, and what the log says when a ship is drawn
 * without it. */
constexpr FragmentShaderDescription GHOST_SHADER{
    .source = GHOST_FRAGMENT_SHADER,
    .textureUniform = GHOST_SPRITE_UNIFORM,
    .unavailableMessage =
        "shaders are not available: an invulnerable ship is "
        "drawn half transparent",
    .notCompiledMessage =
        "the ghost shader did not compile: an invulnerable "
        "ship is drawn half transparent"};

}  // namespace rtype::client

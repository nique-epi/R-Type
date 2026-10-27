#pragma once

#include <cstddef>
#include <string_view>
#include "FragmentShader.hpp"

namespace rtype::client {

/** @brief Names of the uniforms of SHADE_SWAP_FRAGMENT_SHADER: the texture of
 * the copy, which loadFragmentShader() sets to sf::Shader::CurrentTexture, the
 * shades it looks for and those it paints instead, dark, main then light. */
constexpr std::string_view SHADE_SWAP_IMAGE_UNIFORM = "image";
constexpr std::string_view SHADE_SWAP_FROM_UNIFORM = "fromShades";
constexpr std::string_view SHADE_SWAP_TO_UNIFORM = "toShades";

/** @brief How many shades a player color has: the length of the arrays of
 * SHADE_SWAP_FRAGMENT_SHADER. */
constexpr std::size_t PLAYER_SHADE_COUNT = 3;

/**
 * @brief Source code, in GLSL, of the fragment shader that swaps the shades of
 * a player color: a texel within half a step of 8 bits of fromShades[i] takes
 * toShades[i], the others keep their color, then the vertex color multiplies
 * it as without shader. The sheets are drawn unsmoothed, so a texel holds the
 * exact color of the sheet.
 */
constexpr std::string_view SHADE_SWAP_FRAGMENT_SHADER = R"glsl(
uniform sampler2D image;
uniform vec4 fromShades[3];
uniform vec4 toShades[3];

const float SAME_SHADE = 0.5 / 255.0;

vec3 swapped(vec3 color)
{
    for (int shade = 0; shade < 3; ++shade) {
        if (distance(color, fromShades[shade].rgb) < SAME_SHADE) {
            return toShades[shade].rgb;
        }
    }
    return color;
}

void main()
{
    vec4 pixel = texture2D(image, gl_TexCoord[0].xy);
    gl_FragColor = vec4(swapped(pixel.rgb), pixel.a) * gl_Color;
}
)glsl";

/** @brief The shade swap shader, and what the log says when a copy is drawn
 * without it. */
constexpr FragmentShaderDescription SHADE_SWAP_SHADER{
    .source = SHADE_SWAP_FRAGMENT_SHADER,
    .textureUniform = SHADE_SWAP_IMAGE_UNIFORM,
    .unavailableMessage =
        "shaders are not available: the sheets keep the "
        "player colors of the sheet",
    .notCompiledMessage =
        "the shade swap shader did not compile: the sheets "
        "keep the player colors of the sheet"};

}  // namespace rtype::client

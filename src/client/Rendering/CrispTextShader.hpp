#pragma once

#include <string_view>

namespace rtype::client {

/**
 * @brief Name of the uniform of CRISP_TEXT_FRAGMENT_SHADER that holds the
 * texture of the glyphs; InterfaceCanvas sets it to sf::Shader::CurrentTexture,
 * the glyph texture of the text being drawn.
 */
constexpr std::string_view CRISP_TEXT_GLYPHS_UNIFORM = "glyphs";

/**
 * @brief Source code, in GLSL, of the fragment shader that draws text with
 * whole pixels only.
 *
 * SFML renders glyphs with FreeType's autohinter, which leaves pixels of
 * Silkscreen partly covered at its size of 8 pixels. The shader lights a pixel
 * only when its glyph covers at least half of it, and leaves it transparent
 * otherwise: on these fonts this gives back the outline sampled at the center
 * of each pixel, as the browser draws text with smoothing turned off. The
 * color of the text, given by SFML as the color of the vertices, is kept.
 */
constexpr std::string_view CRISP_TEXT_FRAGMENT_SHADER = R"glsl(
uniform sampler2D glyphs;

void main()
{
    float covered = step(0.5, texture2D(glyphs, gl_TexCoord[0].xy).a);
    gl_FragColor = vec4(gl_Color.rgb, gl_Color.a * covered);
}
)glsl";

}  // namespace rtype::client

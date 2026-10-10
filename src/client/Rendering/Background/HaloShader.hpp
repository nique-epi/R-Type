#pragma once

#include <string_view>

namespace rtype::client {

/**
 * @brief Prepended to HALO_FRAGMENT_SHADER, followed by HALO_KERNEL_RADIUS,
 * so the loop of the shader stops at the radius set in C++.
 */
constexpr std::string_view HALO_KERNEL_RADIUS_DEFINITION =
    "#define KERNEL_RADIUS ";

/**
 * @brief Names of the uniforms of HALO_FRAGMENT_SHADER.
 *
 * A uniform is an input of a shader that C++ sets before drawing, with
 * sf::Shader::setUniform; it has the same value for every pixel of a draw.
 * C++ and GLSL match uniforms by name only, so each name is written once,
 * here, and the shader declares the same three:
 * - `source`: the texture to blur. GaussianBlur sets it to
 *   sf::Shader::CurrentTexture, the texture of the sprite being drawn.
 * - `direction`: one pixel along the axis of the pass, in texture
 *   coordinates, which run from 0 to 1: (1 / width, 0) for the horizontal
 *   pass, then (0, 1 / height) for the vertical one.
 * - `standardDeviation`: the width of the blur, HALO_STANDARD_DEVIATION, in
 *   pixels.
 */
constexpr std::string_view HALO_SOURCE_UNIFORM = "source";
constexpr std::string_view HALO_DIRECTION_UNIFORM = "direction";
constexpr std::string_view HALO_DEVIATION_UNIFORM = "standardDeviation";

/**
 * @brief Source code of the shader that blurs the sky for the halo, in GLSL.
 *
 * A shader is a small program that the graphics card runs instead of the
 * processor. This one is a fragment shader: the graphics card runs its
 * `main()` once for every pixel being drawn, all pixels in parallel, and the
 * value it writes to `gl_FragColor` is the color of that pixel. It is not
 * C++: to the C++ compiler it is only the text of a raw string, and its
 * `main()` has nothing to do with the client's. The graphics driver compiles
 * it when GaussianBlur loads it with sf::Shader::loadFromMemory.
 *
 * Each pixel becomes the weighted mean of the pixels up to KERNEL_RADIUS
 * steps of `direction` away on each side, with Gaussian weights of standard
 * deviation `standardDeviation`. One run blurs along one axis only, so
 * GaussianBlur runs it twice, horizontally then vertically: the same result as
 * a square blur, for 2 × 19 reads per pixel instead of 19 × 19.
 */
constexpr std::string_view HALO_FRAGMENT_SHADER = R"glsl(
uniform sampler2D source;
uniform vec2 direction;
uniform float standardDeviation;

void main()
{
    vec4 sum = vec4(0.0);
    float weightSum = 0.0;
    for (int offset = -KERNEL_RADIUS; offset <= KERNEL_RADIUS; ++offset)
    {
        float pixelOffset = float(offset);
        float weight = exp(-(pixelOffset * pixelOffset)
                           / (2.0 * standardDeviation * standardDeviation));
        sum += texture2D(source, gl_TexCoord[0].xy + direction * pixelOffset)
               * weight;
        weightSum += weight;
    }
    gl_FragColor = gl_Color * (sum / weightSum);
}
)glsl";

}  // namespace rtype::client

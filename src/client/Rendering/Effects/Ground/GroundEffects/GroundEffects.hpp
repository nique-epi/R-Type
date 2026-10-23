#pragma once

namespace rtype::engine {

class ComponentRegistry;

}  // namespace rtype::engine

namespace rtype::client {

class IPixelSurface;

/**
 * @brief Paints every pixel effect of the registry on a surface in pixels of
 * the canvas: debris, drops and sparks, shockwaves, speed lines. They lie
 * under every sprite, so the entity layer paints them first.
 *
 * A particle is one pixel, or two by two while it is younger than
 * PARTICLE_SHRINK_SHARE of its life and `shrinks`. A shockwave is a circle one
 * pixel thick, traced on whole pixels, whose radius opens fast then slowly (a
 * cubic ease out) up to `radius`; on the last SHOCKWAVE_DOTTED_SHARE of its
 * life, it lights only the pixels whose column plus row is even. A speed line
 * runs from column 0 to its Position: its head leaves first, at
 * SPEED_LINE_HEAD_PACE times the pace of its life, and its tail follows
 * SPEED_LINE_TAIL_DELAY of a life later. Each takes the color of `colors` its
 * share of life has reached. An effect whose age is negative is not painted.
 */
void paintGroundEffects(engine::ComponentRegistry& components,
                        IPixelSurface& surface);

}  // namespace rtype::client

#pragma once

#include <SFML/System/Vector2.hpp>
#include <cstddef>
#include <optional>

namespace rtype::client {

/** @returns Where the top-left corner of the ship of @p pilot is during its
 * close-up, in pixels of the world: it crosses the shot fast, slows down in
 * its middle, leaves fast; nothing outside its close-up. */
[[nodiscard]] std::optional<sf::Vector2f> closeupPlaceAt(std::size_t pilot,
                                                         float scene);

/** @returns Where the top-left corner of the ship of @p pilot is in the wide
 * shot: it enters from the left, sways, swaps rows for the pink and green
 * ships, pulls back for the charge, trembles, recoils at the release and
 * leaves right at the boost; nothing before INTRO_SWARM_CUT. */
[[nodiscard]] std::optional<sf::Vector2f> squadronPlaceAt(std::size_t pilot,
                                                          float scene);

/** @returns The place of the ship of @p pilot: its close-up, else its place in
 * the wide shot, else nothing. */
[[nodiscard]] std::optional<sf::Vector2f> shipPlaceAt(std::size_t pilot,
                                                      float scene);

/** @returns How long the flames of the ships are, from 0 to 1. */
[[nodiscard]] float exhaustAt(float scene);

/** @returns How far the ghosts of the ship of @p pilot trail, from 0 to 1:
 * as it enters, and from the boost. */
[[nodiscard]] float smearAt(std::size_t pilot, float scene);

/** @returns The nose of a ship whose top-left corner is at @p place, where its
 * charge grows and its beam leaves. */
[[nodiscard]] sf::Vector2f noseOf(sf::Vector2f place);

}  // namespace rtype::client

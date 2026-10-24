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
 * ships, pulls back for the charge, trembles and recoils at the release;
 * nothing before INTRO_SWARM_CUT. */
[[nodiscard]] std::optional<sf::Vector2f> squadronPlaceAt(std::size_t pilot,
                                                          float scene);

/** @returns The place of the ship of @p pilot: its close-up, else its place in
 * the wide shot, else nothing. */
[[nodiscard]] std::optional<sf::Vector2f> shipPlaceAt(std::size_t pilot,
                                                      float scene);

/**
 * @returns The place of the ship of @p pilot, landing from the boost on its
 * place in the squadron of the title screen, whose box lies at
 * @p squadronBox: it surges forward and brakes there in INTRO_LANDING_TIME,
 * INTRO_LANDING_STAGGER after the ship before it, then stays, so the title
 * opens on the ships where they are; nothing while the ship is not on screen.
 */
[[nodiscard]] std::optional<sf::Vector2f> landedPlaceAt(
    std::size_t pilot, float scene, sf::Vector2i squadronBox);

/** @returns How long the flame of the ship of @p pilot is, from 0 to 1: at
 * its longest from the boost, then dying out as the ship lands. */
[[nodiscard]] float exhaustAt(std::size_t pilot, float scene);

/** @returns How far the ghosts of the ship of @p pilot trail, from 0 to 1:
 * as it enters, and as it lands, none once landed. */
[[nodiscard]] float smearAt(std::size_t pilot, float scene);

/** @returns The nose of a ship whose top-left corner is at @p place, where its
 * charge grows and its beam leaves. */
[[nodiscard]] sf::Vector2f noseOf(sf::Vector2f place);

}  // namespace rtype::client

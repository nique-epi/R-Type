#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <string_view>

namespace rtype::game::testing {

/** @brief Today's values of the entities of the game, which the shipped
 * sheets must keep: a Bydo is 96 by 96, crosses at 240 logical units per
 * second, has 3 health points and fires every 1500 ms, first 1500 ms after it
 * appeared (90 ticks); a Drifter is worth 100 points and a Weaver 200, weaving
 * 48 units every 2000 ms; a ship is 128 by 64 and fires 3 times per second (20
 * ticks), at once; a missile of a player is 64 by 24 and flies right at 960, a
 * missile of an enemy 32 by 32, left at 360. */
constexpr float REQUIRED_BYDO_SIZE = 96.0F;
constexpr float REQUIRED_BYDO_SPEED = 240.0F;
constexpr int REQUIRED_BYDO_HEALTH = 3;
constexpr std::size_t REQUIRED_BYDO_SHOT_TICKS = 90;
constexpr std::uint32_t REQUIRED_DRIFTER_POINTS = 100;
constexpr std::uint32_t REQUIRED_WEAVER_POINTS = 200;
constexpr std::array<std::int64_t, 1> REQUIRED_LEFT_HEADING{-1};
constexpr std::array<std::int64_t, 1> REQUIRED_RIGHT_HEADING{1};
constexpr std::array<std::int64_t, 2> REQUIRED_WEAVE{48, 2000};
constexpr float REQUIRED_SHIP_WIDTH = 128.0F;
constexpr float REQUIRED_SHIP_HEIGHT = 64.0F;
constexpr std::size_t REQUIRED_SHIP_SHOT_TICKS = 20;
constexpr std::size_t REQUIRED_SHIP_FIRST_SHOT_TICKS = 0;
constexpr float REQUIRED_PLAYER_MISSILE_WIDTH = 64.0F;
constexpr float REQUIRED_PLAYER_MISSILE_HEIGHT = 24.0F;
constexpr float REQUIRED_PLAYER_MISSILE_SPEED = 960.0F;
constexpr float REQUIRED_ENEMY_MISSILE_SIZE = 32.0F;
constexpr float REQUIRED_ENEMY_MISSILE_SPEED = 360.0F;
constexpr std::string_view REQUIRED_STRAIGHT_NAME = "straight";
constexpr std::string_view REQUIRED_WEAVE_NAME = "weave";

/** @brief The names of the sheets the game ships, as the tests ask for them. */
constexpr std::string_view SHIP_SHEET = "ship";
constexpr std::string_view DRIFTER_SHEET = "drifter";
constexpr std::string_view WEAVER_SHEET = "weaver";
constexpr std::string_view PLAYER_MISSILE_SHEET = "player_missile";
constexpr std::string_view ENEMY_MISSILE_SHEET = "enemy_missile";
constexpr std::string_view VOLTJELLY_SHEET = "voltjelly";
constexpr std::string_view REAPER_SHEET = "reaper";
constexpr std::string_view DRILLBACK_SHEET = "drillback";
constexpr std::string_view BYDOMANCER_SHEET = "bydomancer";
constexpr std::string_view BYDO_TYRANT_SHEET = "bydo_tyrant";
constexpr std::string_view TYRANT_DIVER_SHEET = "tyrant_diver";
constexpr std::string_view TYRANT_MISSILE_SHEET = "tyrant_missile";
constexpr std::string_view TYRANT_SPORE_SHEET = "tyrant_spore";
constexpr std::string_view TYRANT_SPORE_SHARD_SHEET = "tyrant_spore_shard";
constexpr std::string_view RUNE_ORB_SHEET = "rune_orb";
constexpr std::string_view ORB_SHARD_SHEET = "orb_shard";
constexpr std::string_view LOOK_CHEF_TOQUE_SHEET = "look_chef_toque";
constexpr std::string_view LOOK_COWBOY_HAT_SHEET = "look_cowboy_hat";
constexpr std::string_view LOOK_CROWN_SHEET = "look_crown";
constexpr std::string_view LOOK_PARTY_HAT_SHEET = "look_party_hat";
constexpr std::string_view LOOK_PROPELLER_CAP_SHEET = "look_propeller_cap";
constexpr std::string_view LOOK_VIKING_HELMET_SHEET = "look_viking_helmet";
constexpr std::string_view LOOK_WIZARD_HAT_SHEET = "look_wizard_hat";
constexpr std::string_view LOOK_ANTENNAE_SHEET = "look_antennae";
constexpr std::string_view LOOK_CHAPKA_SHEET = "look_chapka";
constexpr std::string_view LOOK_DEVIL_HORNS_SHEET = "look_devil_horns";
constexpr std::string_view LOOK_FEZ_SHEET = "look_fez";
constexpr std::string_view LOOK_GLASSES_3D_SHEET = "look_glasses_3d";
constexpr std::string_view LOOK_GRAD_CAP_SHEET = "look_grad_cap";
constexpr std::string_view LOOK_HALO_SHEET = "look_halo";
constexpr std::string_view LOOK_KABUTO_SHEET = "look_kabuto";
constexpr std::string_view LOOK_LONG_BEARD_SHEET = "look_long_beard";
constexpr std::string_view LOOK_MOHAWK_SHEET = "look_mohawk";
constexpr std::string_view LOOK_MONOCLE_SHEET = "look_monocle";
constexpr std::string_view LOOK_SANTA_HAT_SHEET = "look_santa_hat";
constexpr std::string_view LOOK_SOMBRERO_SHEET = "look_sombrero";
constexpr std::string_view LOOK_TRAFFIC_CONE_SHEET = "look_traffic_cone";
constexpr std::string_view LOOK_TRICORN_SHEET = "look_tricorn";

/** @brief The names of every sheet the game ships, in the order of their
 * names, so of their numbers. */
constexpr std::array SHIPPED_SHEET_NAMES{BYDO_TYRANT_SHEET,
                                         BYDOMANCER_SHEET,
                                         DRIFTER_SHEET,
                                         DRILLBACK_SHEET,
                                         ENEMY_MISSILE_SHEET,
                                         LOOK_ANTENNAE_SHEET,
                                         LOOK_CHAPKA_SHEET,
                                         LOOK_CHEF_TOQUE_SHEET,
                                         LOOK_COWBOY_HAT_SHEET,
                                         LOOK_CROWN_SHEET,
                                         LOOK_DEVIL_HORNS_SHEET,
                                         LOOK_FEZ_SHEET,
                                         LOOK_GLASSES_3D_SHEET,
                                         LOOK_GRAD_CAP_SHEET,
                                         LOOK_HALO_SHEET,
                                         LOOK_KABUTO_SHEET,
                                         LOOK_LONG_BEARD_SHEET,
                                         LOOK_MOHAWK_SHEET,
                                         LOOK_MONOCLE_SHEET,
                                         LOOK_PARTY_HAT_SHEET,
                                         LOOK_PROPELLER_CAP_SHEET,
                                         LOOK_SANTA_HAT_SHEET,
                                         LOOK_SOMBRERO_SHEET,
                                         LOOK_TRAFFIC_CONE_SHEET,
                                         LOOK_TRICORN_SHEET,
                                         LOOK_VIKING_HELMET_SHEET,
                                         LOOK_WIZARD_HAT_SHEET,
                                         ORB_SHARD_SHEET,
                                         PLAYER_MISSILE_SHEET,
                                         REAPER_SHEET,
                                         RUNE_ORB_SHEET,
                                         SHIP_SHEET,
                                         TYRANT_DIVER_SHEET,
                                         TYRANT_MISSILE_SHEET,
                                         TYRANT_SPORE_SHEET,
                                         TYRANT_SPORE_SHARD_SHEET,
                                         VOLTJELLY_SHEET,
                                         WEAVER_SHEET};

}  // namespace rtype::game::testing

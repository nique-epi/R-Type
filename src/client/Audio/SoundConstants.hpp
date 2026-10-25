#pragma once

#include <array>

namespace rtype::client {

/**
 * @brief Asset ids of the sounds of the client, each renamed after what it
 * stands for: all from the Kenney packs Sci-fi Sounds, Impact Sounds and
 * Interface Sounds (CC0) but the hit, the first hit of a file of Epidemic
 * Sound; assets/README.md names the file each one comes from.
 */
constexpr const char* BYDO_GROWL_SOUND_ID = "sounds/kenney/bydo_growl.ogg";
constexpr const char* ENEMY_SHOT_SOUND_ID = "sounds/kenney/enemy_shot.ogg";
constexpr const char* FLYBY_SOUND_ID = "sounds/kenney/flyby.ogg";
constexpr const char* PLAYER_SHOT_SOUND_ID = "sounds/kenney/player_shot.ogg";
constexpr const char* SMALL_EXPLOSION_SOUND_ID =
    "sounds/kenney/small_explosion.ogg";
constexpr const char* HEAVY_IMPACT_SOUND_ID = "sounds/kenney/heavy_impact.ogg";
constexpr const char* ENTRY_WHOOSH_SOUND_ID = "sounds/kenney/entry_whoosh.ogg";
constexpr const char* CHARGE_SOUND_ID = "sounds/kenney/charge.ogg";
constexpr const char* BEAM_SOUND_ID = "sounds/kenney/beam.ogg";
constexpr const char* BIG_EXPLOSION_SOUND_ID =
    "sounds/kenney/big_explosion.ogg";
constexpr const char* BOOST_SOUND_ID = "sounds/kenney/boost.ogg";
constexpr const char* LOGO_IMPACT_SOUND_ID = "sounds/kenney/logo_impact.ogg";
constexpr const char* HIT_SOUND_ID = "sounds/epidemic/hit.flac";
constexpr const char* PLAYER_HIT_SOUND_ID = "sounds/kenney/player_hit.ogg";
constexpr const char* CLICK_SOUND_ID = "sounds/kenney/click.ogg";

/** @brief Every sound of the client, loaded at launch with the interface
 * assets. */
constexpr std::array ALL_SOUND_IDS{
    BYDO_GROWL_SOUND_ID,    ENEMY_SHOT_SOUND_ID,      FLYBY_SOUND_ID,
    PLAYER_SHOT_SOUND_ID,   SMALL_EXPLOSION_SOUND_ID, HEAVY_IMPACT_SOUND_ID,
    ENTRY_WHOOSH_SOUND_ID,  CHARGE_SOUND_ID,          BEAM_SOUND_ID,
    BIG_EXPLOSION_SOUND_ID, BOOST_SOUND_ID,           LOGO_IMPACT_SOUND_ID,
    HIT_SOUND_ID,           PLAYER_HIT_SOUND_ID,      CLICK_SOUND_ID};

}  // namespace rtype::client

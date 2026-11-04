#pragma once

#include <array>
#include <cstddef>
#include <optional>
#include "EnemyKind.hpp"
#include "EntityType.hpp"
#include "ProtocolConstants.hpp"
#include "WireReadingRow.hpp"
#include "WorldState.hpp"

namespace rtype::client {

/**
 * @brief What each protocol entity type is, one line per type, in the order of
 * the protocol page. A Bydo is read as a Drifter: it is drawn alike a Weaver.
 * The projectiles of the new enemies (RuneOrb, Bone, Beam) are read as an
 * EnemyMissile and drawn as one until their own pull request.
 */
constexpr std::array WIRE_READINGS{
    WireReadingRow{
        .wireType = network::WireEntityType::PlayerShip,
        .entity = {.type = game::EntityType::Player, .kind = std::nullopt}},
    WireReadingRow{.wireType = network::WireEntityType::PlayerMissile,
                   .entity = {.type = game::EntityType::PlayerMissile,
                              .kind = std::nullopt}},
    WireReadingRow{.wireType = network::WireEntityType::Bydo,
                   .entity = {.type = game::EntityType::Enemy,
                              .kind = game::EnemyKind::Drifter}},
    WireReadingRow{.wireType = network::WireEntityType::EnemyMissile,
                   .entity = {.type = game::EntityType::EnemyMissile,
                              .kind = std::nullopt}},
    WireReadingRow{.wireType = network::WireEntityType::Voltjelly,
                   .entity = {.type = game::EntityType::Enemy,
                              .kind = game::EnemyKind::Voltjelly}},
    WireReadingRow{.wireType = network::WireEntityType::Reaper,
                   .entity = {.type = game::EntityType::Enemy,
                              .kind = game::EnemyKind::Reaper}},
    WireReadingRow{.wireType = network::WireEntityType::Bydomancer,
                   .entity = {.type = game::EntityType::Enemy,
                              .kind = game::EnemyKind::Bydomancer}},
    WireReadingRow{.wireType = network::WireEntityType::Drillback,
                   .entity = {.type = game::EntityType::Enemy,
                              .kind = game::EnemyKind::Drillback}},
    WireReadingRow{.wireType = network::WireEntityType::BydoTyrant,
                   .entity = {.type = game::EntityType::Enemy,
                              .kind = game::EnemyKind::BydoTyrant}},
    WireReadingRow{.wireType = network::WireEntityType::RuneOrb,
                   .entity = {.type = game::EntityType::EnemyMissile,
                              .kind = std::nullopt}},
    WireReadingRow{.wireType = network::WireEntityType::Bone,
                   .entity = {.type = game::EntityType::EnemyMissile,
                              .kind = std::nullopt}},
    WireReadingRow{.wireType = network::WireEntityType::Beam,
                   .entity = {.type = game::EntityType::EnemyMissile,
                              .kind = std::nullopt}}};

static_assert(WIRE_READINGS.size() ==
                  static_cast<std::size_t>(network::LAST_WIRE_ENTITY_TYPE -
                                           network::FIRST_WIRE_ENTITY_TYPE) +
                      1,
              "every protocol entity type has its line");

}  // namespace rtype::client

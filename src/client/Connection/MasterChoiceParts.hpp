#pragma once

#include <functional>
#include "IServerCatalog.hpp"
#include "MasterLink.hpp"
#include "PlayerSettings.hpp"
#include "SettingValue.hpp"

namespace rtype::client {

/**
 * @brief What a MasterChoice reads and drives, which must outlive it.
 *
 * A std::reference_wrapper has no default constructor, so this struct is
 * only ever built with designated initializers naming every member: the
 * member-init checks, which report its implicit default constructor, see a
 * constructor that cannot be called.
 */
// NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init,hicpp-member-init)
struct MasterChoiceParts {
  /** @brief The settings of the player, which may name a master. */
  std::reference_wrapper<const PlayerSettings> settings;
  /** @brief The master-url setting, read when the player named none. */
  std::reference_wrapper<const engine::SettingValue> masterUrl;
  /** @brief The link a new choice points at its master. */
  std::reference_wrapper<MasterLink> link;
  /** @brief The list a new choice asks again. */
  std::reference_wrapper<IServerCatalog> servers;
};

}  // namespace rtype::client

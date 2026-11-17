#pragma once

#include "OwnedLooks.hpp"
#include "User.hpp"

namespace rtype::client::testing {

/** @returns The account of the token: LINK_ACE_ID, named LINK_ACE_NICKNAME, a
 * player. */
[[nodiscard]] network::User linkAce();

/** @returns The looks of that account: the starter kit without the crown. */
[[nodiscard]] network::OwnedLooks linkLooks();

}  // namespace rtype::client::testing

#pragma once

#include "OwnedLooks.hpp"
#include "User.hpp"

namespace rtype::client::testing {

/** @returns The account of ACE_TOKEN: ACE_ID, named ACE_ACCOUNT_NICKNAME, a
 * player. */
[[nodiscard]] network::User aceAccount();

/** @returns The looks of ACE: the crown alone, CROWN_NAME. */
[[nodiscard]] network::OwnedLooks crownOnlyLooks();

}  // namespace rtype::client::testing

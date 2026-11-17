#pragma once

#include <cstdint>

namespace rtype::client {

/** @brief The number of no ask: the link numbers the asks of each kind from
 * 1, so no reading ever answers it. */
constexpr std::uint64_t NO_ACCOUNT_ASK = 0;

/** @brief The id of no account, which IAccountSession::accountId() gives a
 * player signed out: the master numbers the accounts from 1. */
constexpr std::uint64_t NO_ACCOUNT_ID = 0;

}  // namespace rtype::client

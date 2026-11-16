#pragma once

#include <array>
#include <cstdint>
#include "MasterRouteConstants.hpp"
#include "WalletController.hpp"
#include "WalletRoute.hpp"

namespace rtype::master {

/** @brief The routes of the wallets and what answers each. */
constexpr std::array WALLET_ROUTES{
    WalletRoute{.route = network::READ_OWN_WALLET_ROUTE,
                .answer = &WalletController::readOwnWallet},
    WalletRoute{.route = network::LIST_OWN_MOVEMENTS_ROUTE,
                .answer = &WalletController::listOwnMovements},
    WalletRoute{.route = network::CREATE_OWN_EXCHANGE_ROUTE,
                .answer = &WalletController::createOwnExchange},
    WalletRoute{.route = network::READ_OWN_EXCHANGE_ROUTE,
                .answer = &WalletController::readOwnExchange},
    WalletRoute{.route = network::LIST_OWN_LOOKS_ROUTE,
                .answer = &WalletController::listOwnLooks},
    WalletRoute{.route = network::BUY_OWN_LOOK_ROUTE,
                .answer = &WalletController::buyOwnLook}};

/** @brief What a wallet holds when it opens while the settings
 * `wallet-starting-kopecks` and `wallet-starting-chips` keep their default:
 * no kopecks, and chips to try the casino. */
constexpr std::int64_t DEFAULT_STARTING_KOPECKS = 0;
constexpr std::int64_t DEFAULT_STARTING_CHIPS = 500;

}  // namespace rtype::master

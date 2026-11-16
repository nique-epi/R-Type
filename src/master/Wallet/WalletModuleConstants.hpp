#pragma once

#include <array>
#include <cstdint>
#include "ControllerRoute.hpp"
#include "MasterRouteConstants.hpp"
#include "WalletController.hpp"

namespace rtype::master {

/** @brief The routes of the wallets and what answers each. */
constexpr std::array WALLET_ROUTES{
    ControllerRoute<WalletController>{
        .route = network::READ_OWN_WALLET_ROUTE,
        .answer = &WalletController::readOwnWallet},
    ControllerRoute<WalletController>{
        .route = network::LIST_OWN_MOVEMENTS_ROUTE,
        .answer = &WalletController::listOwnMovements},
    ControllerRoute<WalletController>{
        .route = network::CREATE_OWN_EXCHANGE_ROUTE,
        .answer = &WalletController::createOwnExchange},
    ControllerRoute<WalletController>{
        .route = network::READ_OWN_EXCHANGE_ROUTE,
        .answer = &WalletController::readOwnExchange},
    ControllerRoute<WalletController>{
        .route = network::LIST_OWN_LOOKS_ROUTE,
        .answer = &WalletController::listOwnLooks},
    ControllerRoute<WalletController>{.route = network::BUY_OWN_LOOK_ROUTE,
                                      .answer = &WalletController::buyOwnLook}};

/** @brief What a wallet holds when it opens while the settings
 * `wallet-starting-kopecks` and `wallet-starting-chips` keep their default:
 * no kopecks, and chips to try the casino. */
constexpr std::int64_t DEFAULT_STARTING_KOPECKS = 0;
constexpr std::int64_t DEFAULT_STARTING_CHIPS = 500;

}  // namespace rtype::master

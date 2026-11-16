#pragma once

#include <array>
#include <string_view>
#include "AccountsController.hpp"
#include "ControllerRoute.hpp"
#include "MasterRouteConstants.hpp"

namespace rtype::master {

/** @brief The routes of the accounts and what answers each. */
constexpr std::array ACCOUNT_ROUTES{
    ControllerRoute<AccountsController>{
        .route = network::CREATE_USER_ROUTE,
        .answer = &AccountsController::createUser},
    ControllerRoute<AccountsController>{
        .route = network::READ_OWN_USER_ROUTE,
        .answer = &AccountsController::readOwnUser},
    ControllerRoute<AccountsController>{
        .route = network::CREATE_SESSION_ROUTE,
        .answer = &AccountsController::createSession},
    ControllerRoute<AccountsController>{
        .route = network::DELETE_OWN_SESSION_ROUTE,
        .answer = &AccountsController::deleteOwnSession}};

/** @brief The command of master.cfg that makes an account an admin. */
constexpr std::string_view ADMIN_COMMAND_NAME = "admin";
constexpr std::string_view ADMIN_ARGUMENTS = "<nickname>";
constexpr std::string_view ADMIN_HELP =
    "Makes the user of that nickname an admin when the master starts";

/** @brief What the module writes in the journal and answers a refused line
 * with. */
constexpr std::string_view NOT_ONE_NICKNAME_TEXT =
    "admin takes exactly one nickname";
constexpr std::string_view ADMIN_GIVEN_TEXT = " is an admin";
constexpr std::string_view NO_USER_NAMED_TEXT = "no user is named ";
constexpr std::string_view ADMIN_LATER_TEXT =
    "; the admin line applies once it exists and the master starts again";

}  // namespace rtype::master

#pragma once

#include <array>
#include <string_view>
#include "AccountsController.hpp"
#include "ControllerRoute.hpp"
#include "DeviceLinksController.hpp"
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

/** @brief The routes of the device links and what answers each. A route of
 * the link of the secret comes before the route of a short code, whose
 * parameter would also take `me`. */
constexpr std::array DEVICE_LINK_ROUTES{
    ControllerRoute<DeviceLinksController>{
        .route = network::CREATE_DEVICE_LINK_ROUTE,
        .answer = &DeviceLinksController::createDeviceLink},
    ControllerRoute<DeviceLinksController>{
        .route = network::READ_OWN_DEVICE_LINK_ROUTE,
        .answer = &DeviceLinksController::readOwnDeviceLink},
    ControllerRoute<DeviceLinksController>{
        .route = network::DELETE_OWN_DEVICE_LINK_ROUTE,
        .answer = &DeviceLinksController::deleteOwnDeviceLink},
    ControllerRoute<DeviceLinksController>{
        .route = network::READ_DEVICE_LINK_ROUTE,
        .answer = &DeviceLinksController::readDeviceLink},
    ControllerRoute<DeviceLinksController>{
        .route = network::REPLACE_DEVICE_LINK_DECISION_ROUTE,
        .answer = &DeviceLinksController::replaceDeviceLinkDecision}};

/** @brief The `site-url` setting: its value before any `set`, the address the
 * site is served from, what `help` says of it, and the schemes it may start
 * with. */
constexpr std::string_view DEFAULT_SITE_URL = "https://rtype.app";
constexpr std::string_view SITE_URL_HELP =
    "Address of the web site, whose page /link validates the device links a "
    "game signs in through, with no slash at its end";
constexpr std::string_view HTTPS_SCHEME = "https://";
constexpr std::string_view HTTP_SCHEME = "http://";
constexpr std::string_view SITE_URL_REFUSED_TEXT =
    "site-url starts with https:// or http://";

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

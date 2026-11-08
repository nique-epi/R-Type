#pragma once

#include <array>
#include <string_view>
#include "NetworkConstants.hpp"

namespace rtype::client {

/**
 * @brief Name of the client program, at the start of its usage and of the
 * errors it prints about its arguments.
 */
constexpr std::string_view PROGRAM_NAME = "r-type_client";

/**
 * @brief Launch option followed by the IPv4 address of the server to join.
 */
constexpr std::string_view ADDRESS_OPTION = "-h";

/**
 * @brief Address of the server to join when none is given at launch: a
 * server on the same machine.
 */
constexpr std::string_view DEFAULT_SERVER_ADDRESS =
    network::LOOPBACK_IPV4_ADDRESS;

/**
 * @brief Launch option followed by the nickname of the player.
 */
constexpr std::string_view NICKNAME_OPTION = "-n";

/**
 * @brief Nickname of the player when none is given at launch.
 */
constexpr std::string_view DEFAULT_NICKNAME = "PILOT";

/**
 * @brief Launch option followed by the game to ask the server for: the value
 * NEW_GAME_VALUE opens a game, a number joins the game of that number. Without
 * it the client takes any game that has a seat.
 */
constexpr std::string_view GAME_OPTION = "--game";
constexpr std::string_view NEW_GAME_VALUE = "new";

/**
 * @brief The file of commands the client reads when it starts; a missing file
 * is not an error.
 */
constexpr std::string_view CLIENT_CONFIGURATION_FILE_NAME = "client.cfg";

/**
 * @brief The names the settings of the client go by in `set` and in `--set`.
 */
constexpr std::string_view NICKNAME_SETTING_NAME = "nickname";
constexpr std::string_view SERVER_ADDRESS_SETTING_NAME = "server-address";
constexpr std::string_view SERVER_PORT_SETTING_NAME = "server-port";
/** @brief The master the server map reads, `http(s)://<host>[:<port>]`. */
constexpr std::string_view MASTER_URL_SETTING_NAME = "master-url";
/** @brief The master the server map reads when no setting names another: the
 * public master of the game. */
constexpr std::string_view DEFAULT_MASTER_URL = "https://master.rtype.app";

/** @brief Every setting of the client, in the order its usage lists them. */
constexpr std::array CLIENT_SETTING_NAMES{
    NICKNAME_SETTING_NAME, SERVER_ADDRESS_SETTING_NAME,
    SERVER_PORT_SETTING_NAME, MASTER_URL_SETTING_NAME};

/**
 * @brief Launch option that starts the client without sound.
 */
constexpr std::string_view NO_SOUND_OPTION = "--no-sound";

/**
 * @brief Logger name the client writes the log lines of its launch under.
 */
constexpr std::string_view CLIENT_LOGGER_NAME = "Client";

/**
 * @brief The stages of the client's loop, in the order each frame runs them:
 * the window events are read, the datagrams of the server are read, the
 * screens are updated, the frame is drawn, then shown. A module adds its work
 * to one of them during its install(); a module that draws over the screens
 * draws in RENDER_STAGE, before the window shows the frame in DISPLAY_STAGE.
 */
constexpr std::string_view EVENT_STAGE = "events";
constexpr std::string_view NETWORK_STAGE = "network";
constexpr std::string_view UPDATE_STAGE = "update";
constexpr std::string_view RENDER_STAGE = "render";
constexpr std::string_view DISPLAY_STAGE = "display";
constexpr std::array<std::string_view, 5> CLIENT_LOOP_STAGES{
    EVENT_STAGE, NETWORK_STAGE, UPDATE_STAGE, RENDER_STAGE, DISPLAY_STAGE};

}  // namespace rtype::client

#pragma once

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <string_view>
#include "InterfaceConstants.hpp"
#include "LetterCounts.hpp"
#include "NetworkConstants.hpp"
#include "ProtocolConstants.hpp"
#include "TimeConstants.hpp"
#include "Translation.hpp"
#include "WidgetConstants.hpp"

namespace rtype::client {

/**
 * @brief Texts of the connection screen: its heading, the one of a screen that
 * asks only for the nickname, then the label and the description of each bar,
 * in their order.
 */
constexpr Translation CONNECTION_HEADING{.english = "DIRECT CONNECT",
                                         .french = "CONNEXION DIRECTE"};
constexpr Translation JOIN_SERVER_HEADING{.english = "JOIN THE SERVER",
                                          .french = "REJOINDRE LE SERVEUR"};
constexpr Translation ADDRESS_LABEL{.english = "ADDRESS", .french = "ADRESSE"};
constexpr Translation ADDRESS_DESCRIPTION{
    .english = "> IPV4 ADDRESS OF THE SERVER",
    .french = "> ADRESSE IPV4 DU SERVEUR"};
constexpr Translation PORT_LABEL{.english = "PORT", .french = "PORT"};
constexpr Translation PORT_DESCRIPTION{.english = "> UDP PORT OF THE SERVER",
                                       .french = "> PORT UDP DU SERVEUR"};
constexpr Translation NICKNAME_LABEL{.english = "PILOT", .french = "PILOTE"};
constexpr Translation NICKNAME_DESCRIPTION{
    .english = "> CHOOSE YOUR CALLSIGN",
    .french = "> CHOISISSEZ VOTRE INDICATIF"};
constexpr Translation CONNECT_LABEL{.english = "CONNECT",
                                    .french = "CONNEXION"};
constexpr Translation CONNECT_DESCRIPTION{.english = "> JOIN THE GAME",
                                          .french = "> REJOINDRE LA PARTIE"};

/** @brief Order of each field in the menu of the connection screen, and of
 * the nickname in a screen that asks for it alone. */
constexpr std::size_t ADDRESS_ENTRY = 0;
constexpr std::size_t PORT_ENTRY = 1;
constexpr std::size_t NICKNAME_ENTRY = 2;
constexpr std::size_t NICKNAME_ALONE_ENTRY = 0;

/**
 * @brief Largest text of each field, in bytes: an IPv4 address of four numbers
 * of three digits and the three dots between them, the five digits of
 * HIGHEST_PORT, and the longest nickname the protocol accepts. A character
 * takes at least one byte, so each fits in its box when its size is within
 * FIELD_CHARACTER_CAPACITY.
 */
constexpr std::size_t ADDRESS_FIELD_SIZE =
    (network::IPV4_ADDRESS_PART_COUNT *
     (static_cast<std::size_t>(std::numeric_limits<std::uint8_t>::digits10) +
      1)) +
    (network::IPV4_ADDRESS_PART_COUNT - 1);
constexpr std::size_t PORT_FIELD_SIZE =
    static_cast<std::size_t>(std::numeric_limits<std::uint16_t>::digits10) + 1;
constexpr std::size_t NICKNAME_FIELD_SIZE = network::MAXIMUM_NICKNAME_SIZE;

static_assert(ADDRESS_FIELD_SIZE <= FIELD_CHARACTER_CAPACITY,
              "the longest address fits in its box");
static_assert(PORT_FIELD_SIZE <= FIELD_CHARACTER_CAPACITY,
              "the highest port fits in its box");
static_assert(NICKNAME_FIELD_SIZE <= FIELD_CHARACTER_CAPACITY,
              "the longest nickname fits in its box");

/**
 * @brief Status lines of the connection screen: how the attempt stands, or
 * which field to fix before one starts. The number of the player follows
 * ACCEPTED_STATUS. A refusal for another version names both protocol
 * versions: the client's after INCOMPATIBLE_VERSION_STATUS, the server's after
 * SERVER_VERSION_STATUS.
 */
constexpr Translation CONNECTING_STATUS{.english = "CONNECTING...",
                                        .french = "CONNEXION..."};
constexpr std::string_view NUMBER_MARK = "{NUMBER}";
constexpr Translation ACCEPTED_STATUS{.english = "CONNECTED AS PLAYER {NUMBER}",
                                      .french = "CONNECTÉ, JOUEUR {NUMBER}"};
constexpr Translation GAME_FULL_STATUS{.english = "REFUSED: GAME FULL",
                                       .french = "REFUSÉ : PARTIE PLEINE"};
constexpr Translation GAME_IN_PROGRESS_STATUS{
    .english = "REFUSED: GAME IN PROGRESS",
    .french = "REFUSÉ : PARTIE EN COURS"};
constexpr Translation NICKNAME_TAKEN_STATUS{
    .english = "REFUSED: CALLSIGN TAKEN", .french = "REFUSÉ : INDICATIF PRIS"};
constexpr Translation GAME_NOT_FOUND_STATUS{
    .english = "REFUSED: NO SUCH GAME", .french = "REFUSÉ : PARTIE INCONNUE"};
constexpr Translation SERVER_FULL_STATUS{.english = "REFUSED: SERVER FULL",
                                         .french = "REFUSÉ : SERVEUR PLEIN"};
constexpr Translation UNKNOWN_DIFFICULTY_STATUS{
    .english = "REFUSED: BAD DIFFICULTY", .french = "REFUSÉ : DIFFICULTÉ"};
/** @brief The refusal of lives out of bounds, then the fewest lives a server
 * accepts, LIVES_BOUNDS_SEPARATOR and the most it accepts. */
constexpr std::string_view LOWEST_MARK = "{LOWEST}";
constexpr std::string_view HIGHEST_MARK = "{HIGHEST}";
constexpr Translation LIVES_REFUSED_STATUS{
    .english = "REFUSED: LIVES {LOWEST} TO {HIGHEST}",
    .french = "REFUSÉ : VIES {LOWEST} À {HIGHEST}"};
constexpr std::string_view CLIENT_VERSION_MARK = "{CLIENT}";
constexpr std::string_view SERVER_VERSION_MARK = "{SERVER}";
constexpr Translation INCOMPATIBLE_VERSION_STATUS{
    .english = "REFUSED: V{CLIENT}, SERVER V{SERVER}",
    .french = "VERSION V{CLIENT}, SERVEUR V{SERVER}"};
constexpr Translation UNREACHABLE_STATUS{.english = "SERVER UNREACHABLE",
                                         .french = "SERVEUR INJOIGNABLE"};
constexpr Translation INVALID_ADDRESS_STATUS{.english = "INVALID ADDRESS",
                                             .french = "ADRESSE INVALIDE"};
constexpr Translation INVALID_PORT_STATUS{.english = "INVALID PORT",
                                          .french = "PORT INVALIDE"};
constexpr Translation MISSING_NICKNAME_STATUS{.english = "ENTER A CALLSIGN",
                                              .french = "ENTREZ UN INDICATIF"};

/**
 * @brief The status lines of the content of a server: LOADING_CONTENT_STATUS
 * then the share of the pack received, in whole percents, while it downloads;
 * then, when it cannot be loaded, why.
 */
constexpr std::string_view PERCENT_MARK = "{PERCENT}";
constexpr Translation LOADING_CONTENT_STATUS{
    .english = "LOADING CONTENT {PERCENT}%",
    .french = "CHARGEMENT {PERCENT} %"};
constexpr std::size_t WHOLE_PERCENT = 100;
constexpr Translation PACK_DOWNLOAD_FAILED_STATUS{
    .english = "PACK DOWNLOAD FAILED", .french = "ÉCHEC DU TÉLÉCHARGEMENT"};
constexpr Translation PACK_REFUSED_STATUS{.english = "PACK REFUSED",
                                          .french = "CONTENU REFUSÉ"};

/**
 * @brief The characters a status line of the form holds: the width of the
 * form less its margin on both sides, 240 - 2 x 16 = 208 pixels, over the 8
 * pixels a character of the regular font takes, so 26. The refusal of a
 * version, REFUSED: V15, SERVER V16, takes 24, and 26 for versions of three
 * digits.
 */
constexpr std::size_t STATUS_LINE_CHARACTERS = static_cast<std::size_t>(
    (FORM_WIDTH - (SIDES_PER_AXIS * FORM_HEADING_OFFSET.x)) /
    REGULAR_FONT_ADVANCE);

/** @brief The letters of the largest number a status line holds: a byte of
 * three digits, a number of lives or a protocol version. */
constexpr std::size_t LARGEST_STATUS_NUMBER_LETTERS =
    static_cast<std::size_t>(std::numeric_limits<std::uint8_t>::digits10) + 1;

static_assert(longestLettersIn(UNKNOWN_DIFFICULTY_STATUS) <=
                  STATUS_LINE_CHARACTERS,
              "the refusal of a difficulty fits its status line");
static_assert(longestLettersIn(LIVES_REFUSED_STATUS) - LOWEST_MARK.size() -
                      HIGHEST_MARK.size() +
                      (static_cast<std::size_t>(SIDES_PER_AXIS) *
                       LARGEST_STATUS_NUMBER_LETTERS) <=
                  STATUS_LINE_CHARACTERS,
              "the refusal of lives fits its status line, with two bounds of "
              "three digits");
static_assert(longestLettersIn(INCOMPATIBLE_VERSION_STATUS) -
                      CLIENT_VERSION_MARK.size() - SERVER_VERSION_MARK.size() +
                      (static_cast<std::size_t>(SIDES_PER_AXIS) *
                       LARGEST_STATUS_NUMBER_LETTERS) <=
                  STATUS_LINE_CHARACTERS,
              "the refusal of a version fits its status line, with two "
              "versions of three digits");

/**
 * @brief How long the connection screen shows that the server accepted the
 * player, before it opens the waiting room.
 */
constexpr engine::Duration ACCEPTED_STATUS_DURATION = std::chrono::seconds(1);

}  // namespace rtype::client

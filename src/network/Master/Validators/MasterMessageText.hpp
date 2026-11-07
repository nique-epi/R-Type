#pragma once

#include <optional>
#include <string>
#include <string_view>
#include "CreatedServer.hpp"
#include "GameListing.hpp"
#include "SampleListing.hpp"
#include "ServerCreation.hpp"
#include "ServerHeartbeat.hpp"
#include "ServerListing.hpp"
#include "ServerRegistration.hpp"
#include "ServerStatus.hpp"
#include "StatusListing.hpp"

namespace rtype::network {

/**
 * @returns Whether @p text has the shape of a key the master issues:
 * SERVER_KEY_PREFIX, then lowercase hexadecimal digits, SERVER_KEY_SIZE in
 * all. Anything else is refused before the master looks it up.
 */
[[nodiscard]] bool isWellFormedServerKey(std::string_view text);

/** @returns @p creation as the JSON body of `POST /api/servers`. */
[[nodiscard]] std::string serverCreationText(const ServerCreation& creation);

/**
 * @returns The creation the web site sent as @p text.
 * @throws InvalidMasterMessageException naming `/name` when it is missing,
 * empty, longer than MAXIMUM_SERVER_NAME_SIZE or not printable UTF-8.
 */
[[nodiscard]] ServerCreation serverCreationFromText(std::string_view text);

/** @returns @p created as the JSON answer of `POST /api/servers`. */
[[nodiscard]] std::string createdServerText(const CreatedServer& created);

/**
 * @returns The created server the master answered as @p text.
 * @throws InvalidMasterMessageException naming the first member missing or out
 * of its bounds: an id below FIRST_SERVER_ID, a name as for a creation, or a
 * key that is not well formed.
 */
[[nodiscard]] CreatedServer createdServerFromText(std::string_view text);

/**
 * @returns @p registration as the JSON body of `PUT /api/servers/me`, its
 * address left out when it is empty, its probeSecret included: the list of
 * servers never writes it. A text that is not UTF-8 is written with U+FFFD in
 * place of its bad bytes, so writing never throws.
 */
[[nodiscard]] std::string registrationText(
    const ServerRegistration& registration);

/**
 * @returns The registration a server sent as @p text. A member it does not
 * know is ignored, so a newer server still registers.
 * @throws InvalidMasterMessageException naming the first member that is
 * missing, of another type or out of its bounds: a name or a version that is
 * empty, too long or not printable UTF-8, an address that is no IPv4 address,
 * a port outside LOWEST_PORT to HIGHEST_PORT, a number of games outside
 * MINIMUM_HOSTED_GAMES to MAXIMUM_HOSTED_GAMES or of players outside
 * MINIMUM_PLAYERS_PER_GAME to MAXIMUM_PLAYERS, a probeSecret outside
 * LOWEST_PROBE_SECRET to HIGHEST_PROBE_SECRET.
 */
[[nodiscard]] ServerRegistration registrationFromText(std::string_view text);

/**
 * @returns @p heartbeat as the JSON body of `PUT /api/servers/me/heartbeat`,
 * every empty measure left out, written as registrationText() writes.
 */
[[nodiscard]] std::string heartbeatText(const ServerHeartbeat& heartbeat);

/**
 * @returns The heartbeat a server sent as @p text, its probeToken empty when
 * it carries none; a member it does not know is ignored.
 * @throws InvalidMasterMessageException naming the first member that is
 * missing, of another type or out of its bounds: more than
 * MAXIMUM_HOSTED_GAMES games, two games of one number, a number below
 * FIRST_HOSTED_GAME_NUMBER, a name that is empty, longer than
 * MAXIMUM_GAME_NAME_SIZE or not printable UTF-8, a state that is not one of
 * HOSTED_GAME_STATE_TEXTS, a difficulty or a player that is no valid nickname,
 * more than MAXIMUM_PLAYERS players in a game, a measure that is negative or
 * not a number, a loss above MAXIMUM_LOSS_PERCENT, a probeToken outside
 * LOWEST_PROBE_TOKEN to HIGHEST_PROBE_TOKEN.
 */
[[nodiscard]] ServerHeartbeat heartbeatFromText(std::string_view text);

/** @returns @p listing as the JSON answer of `GET /api/servers/{id}/games`. */
[[nodiscard]] std::string gameListingText(const GameListing& listing);

/**
 * @returns The listing the master answered as @p text.
 * @throws InvalidMasterMessageException naming the first member missing, of
 * another type or out of its bounds: more than MAXIMUM_HOSTED_GAMES games, two
 * games of one number, a number below FIRST_HOSTED_GAME_NUMBER, a name as for a
 * heartbeat, a state that is not one of LISTED_GAME_STATE_TEXTS, more than
 * MAXIMUM_PLAYERS players.
 */
[[nodiscard]] GameListing gameListingFromText(std::string_view text);

/** @returns How @p status is written: one of SERVER_STATUS_TEXTS. */
[[nodiscard]] std::string_view serverStatusText(ServerStatus status);

/** @returns The status @p text writes, or nothing when it is none of
 * SERVER_STATUS_TEXTS. */
[[nodiscard]] std::optional<ServerStatus> serverStatusOf(std::string_view text);

/** @returns @p listing as the JSON answer of `GET /api/servers`, written as
 * registrationText() writes. */
[[nodiscard]] std::string serverListingText(const ServerListing& listing);

/**
 * @returns The page of servers the master answered as @p text.
 * @throws InvalidMasterMessageException naming the first member missing, of
 * another type or out of its bounds: more than MAXIMUM_PAGE_SIZE servers, an id
 * below FIRST_SERVER_ID, a registration as registrationFromText() reads it but
 * its probeSecret, which a list never holds, a status that is not one of
 * SERVER_STATUS_TEXTS, more than MAXIMUM_HOSTED_GAMES games, more players than
 * MAXIMUM_HOSTED_GAMES games of MAXIMUM_PLAYERS hold.
 */
[[nodiscard]] ServerListing serverListingFromText(std::string_view text);

/** @returns @p listing as the JSON answer of `GET /api/servers/{id}/samples`,
 * every measure not taken left out, as heartbeatText() leaves it out. */
[[nodiscard]] std::string sampleListingText(const SampleListing& listing);

/** @returns @p listing as the JSON answer of
 * `GET /api/servers/{id}/statuses`. */
[[nodiscard]] std::string statusListingText(const StatusListing& listing);

/**
 * @returns The `detail` of the problem document (RFC 9457) @p text holds, or
 * an empty text when @p text is no JSON object or has no `detail` text.
 */
[[nodiscard]] std::string problemDetailOf(std::string_view text);

}  // namespace rtype::network

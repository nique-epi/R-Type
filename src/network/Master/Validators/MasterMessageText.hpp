#pragma once

#include <optional>
#include <string>
#include <string_view>
#include <vector>
#include "AccountLeaderboardRow.hpp"
#include "BetsReceipt.hpp"
#include "CasinoRoomListing.hpp"
#include "CasinoRoomState.hpp"
#include "CreatedDeviceLink.hpp"
#include "CreatedServer.hpp"
#include "CreatedSession.hpp"
#include "Currency.hpp"
#include "DeviceLinkDecision.hpp"
#include "DeviceLinkState.hpp"
#include "DeviceLinkStatus.hpp"
#include "ExchangeReceipt.hpp"
#include "ExchangeRequest.hpp"
#include "FinishedGame.hpp"
#include "GameListing.hpp"
#include "GameTicketGrant.hpp"
#include "GameTicketState.hpp"
#include "GlobalLeaderboard.hpp"
#include "LookPurchase.hpp"
#include "MovementListing.hpp"
#include "MovementReason.hpp"
#include "OwnedLooks.hpp"
#include "RouletteBetKind.hpp"
#include "RouletteBets.hpp"
#include "RoulettePhase.hpp"
#include "RoundBets.hpp"
#include "SampleListing.hpp"
#include "ServerCreation.hpp"
#include "ServerHeartbeat.hpp"
#include "ServerListing.hpp"
#include "ServerRegistration.hpp"
#include "ServerStatus.hpp"
#include "StatusListing.hpp"
#include "User.hpp"
#include "UserCredentials.hpp"
#include "UserRole.hpp"
#include "Wallet.hpp"
#include "WalletExchange.hpp"

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
 * not a number, a loss above MAXIMUM_LOSS_PERCENT, a share of busy
 * processors above MAXIMUM_CPU_PERCENT, a probeToken outside
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
 * @returns Whether @p text has the shape of the id of a game a server draws:
 * GAME_ID_SIZE lowercase hexadecimal digits.
 */
[[nodiscard]] bool isWellFormedGameId(std::string_view text);

/** @returns @p game as the JSON body of
 * `PUT /api/servers/me/results/{gameId}`, the accountId and the ticket of a
 * guest left out, victory and the wonLooks of each player always written. */
[[nodiscard]] std::string finishedGameText(const FinishedGame& game);

/**
 * @returns The finished game a server sent as @p text; a member it does not
 * know is ignored. A game that was not vanilla reads as any other: the master
 * refuses it after reading it. A game without victory reads as lost, and a
 * player without wonLooks as one who won no item, as a server older than these
 * members reports.
 * @throws InvalidMasterMessageException naming the first member that is
 * missing, of another type or out of its bounds: a playedAt outside
 * EARLIEST_PLAYED_AT to LATEST_PLAYED_AT, a difficulty or a nickname that is no
 * valid nickname, a playerCount outside MINIMUM_PLAYERS_PER_GAME to
 * MAXIMUM_PLAYERS, a vanilla that is no boolean, fewer than
 * FEWEST_FINISHED_GAME_PLAYERS or more than MAXIMUM_PLAYERS players, more
 * players than its playerCount, one nickname twice (nicknameKey()), an
 * accountId that is empty, longer than MAXIMUM_ACCOUNT_ID_SIZE or not printable
 * UTF-8, a score above HIGHEST_SCORE, a bounty above HIGHEST_BOUNTY, a
 * victory that is no boolean, a wonLooks
 * that is no array or holds more than MAXIMUM_WON_LOOKS names, a name that is
 * not one of identifiers::SHIP_LOOK_ITEM_NAMES, a ticket that is not well
 * formed (isWellFormedGameTicket()). A player without a ticket reads as a
 * guest, one without a bounty as a bounty of 0.
 */
[[nodiscard]] FinishedGame finishedGameFromText(std::string_view text);

/** @returns @p leaderboard as the JSON answer of `GET /api/leaderboard`, its
 * player left out when it has none, written as registrationText() writes. */
[[nodiscard]] std::string globalLeaderboardText(
    const GlobalLeaderboard& leaderboard);

/** @returns @p accounts as the JSON answer of `GET /api/leaderboard/accounts`,
 * `{"accounts":[{"rank":1,"nickname":"ACE","score":2234,
 * "lastPlayedAt":1800000000}]}`, written as registrationText() writes. */
[[nodiscard]] std::string accountLeaderboardText(
    const std::vector<AccountLeaderboardRow>& accounts);

/**
 * @returns The leaderboard @p text holds, as globalLeaderboardText() writes
 * it.
 * @throws InvalidMasterMessageException naming the JSON Pointer of the first
 * member it refuses: no `scores` array, more than MAXIMUM_PAGE_SIZE rows, a
 * rank below LOWEST_RANK, a nickname the protocol refuses, a score above
 * HIGHEST_SCORE, a date before EARLIEST_PLAYED_AT, a server id below
 * FIRST_SERVER_ID or a server name that is empty or longer than
 * MAXIMUM_SERVER_NAME_SIZE.
 */
[[nodiscard]] GlobalLeaderboard globalLeaderboardFromText(
    std::string_view text);

/** @returns @p credentials as the JSON body of `POST /api/users` and of
 * `POST /api/sessions`. */
[[nodiscard]] std::string userCredentialsText(
    const UserCredentials& credentials);

/**
 * @returns The credentials a player sent as @p text; a member it does not know
 * is ignored.
 * @throws InvalidMasterMessageException naming `/nickname` when it is no
 * nickname of the protocol (isValidNickname()), or `/password` when it is not
 * MINIMUM_PASSWORD_SIZE to MAXIMUM_PASSWORD_SIZE bytes of printable UTF-8.
 */
[[nodiscard]] UserCredentials userCredentialsFromText(std::string_view text);

/** @returns How @p role is written: one of USER_ROLE_TEXTS. */
[[nodiscard]] std::string_view userRoleText(UserRole role);

/** @returns The role @p text writes, or nothing for a text not in
 * USER_ROLE_TEXTS. */
[[nodiscard]] std::optional<UserRole> userRoleOf(std::string_view text);

/** @returns @p account as the JSON answer of `POST /api/users` and of
 * `GET /api/users/me`. */
[[nodiscard]] std::string userText(const User& account);

/**
 * @returns The account the master answered as @p text: an object whose `id`
 * is a whole number of at least 1, whose `nickname` is a text and whose
 * `role` is one of USER_ROLE_TEXTS.
 * @throws InvalidMasterMessageException naming the first member that is not.
 */
[[nodiscard]] User userFromText(std::string_view text);

/** @returns @p session as the JSON answer of `POST /api/sessions`. */
[[nodiscard]] std::string createdSessionText(const CreatedSession& session);

/**
 * @returns Whether @p text has the shape of a token of a session:
 * SESSION_TOKEN_PREFIX, then lowercase hexadecimal digits, SESSION_TOKEN_SIZE
 * in all. Anything else is refused before the master looks it up.
 */
[[nodiscard]] bool isWellFormedSessionToken(std::string_view text);

/** @returns How @p status is written: one of DEVICE_LINK_STATUS_TEXTS. */
[[nodiscard]] std::string_view deviceLinkStatusText(DeviceLinkStatus status);

/** @returns How @p decision is written: one of DEVICE_LINK_DECISION_TEXTS. */
[[nodiscard]] std::string_view deviceLinkDecisionText(
    DeviceLinkDecision decision);

/** @returns The decision @p text writes, or nothing for a text not in
 * DEVICE_LINK_DECISION_TEXTS. */
[[nodiscard]] std::optional<DeviceLinkDecision> deviceLinkDecisionOf(
    std::string_view text);

/**
 * @returns Whether @p text has the shape of the secret of a device link:
 * DEVICE_LINK_SECRET_PREFIX, then lowercase hexadecimal digits,
 * DEVICE_LINK_SECRET_SIZE in all. Anything else is refused before the master
 * looks it up.
 */
[[nodiscard]] bool isWellFormedDeviceLinkSecret(std::string_view text);

/**
 * @returns The short code @p text writes, as the master keeps it: its
 * DEVICE_LINK_USER_CODE_LETTERS letters of DEVICE_LINK_USER_CODE_ALPHABET in
 * capitals, joined by DEVICE_LINK_USER_CODE_SEPARATOR after the first
 * DEVICE_LINK_USER_CODE_GROUP. A player may type it in any case, with or
 * without separators (RFC 8628, section 6.1); nothing for any other text.
 */
[[nodiscard]] std::optional<std::string> deviceLinkUserCodeOf(
    std::string_view text);

/** @returns @p link as the JSON answer of `POST /api/device-links`. */
[[nodiscard]] std::string createdDeviceLinkText(const CreatedDeviceLink& link);

/**
 * @returns The device link the master answered as @p text: a well formed
 * `secret`, a short code, two page addresses, an `expiresAt` of seconds from
 * 0 and an `interval` of 1 to MAXIMUM_DEVICE_LINK_INTERVAL seconds.
 * @throws InvalidMasterMessageException naming the first member that is not.
 */
[[nodiscard]] CreatedDeviceLink createdDeviceLinkFromText(
    std::string_view text);

/** @returns @p state as the JSON answer of `GET /api/device-links/me` and of
 * `GET /api/device-links/{code}`. */
[[nodiscard]] std::string deviceLinkStateText(const DeviceLinkState& state);

/**
 * @returns The state of a device link the master answered as @p text.
 * @throws InvalidMasterMessageException naming the first member that is not
 * what deviceLinkStateText() writes.
 */
[[nodiscard]] DeviceLinkState deviceLinkStateFromText(std::string_view text);

/** @returns @p decision as the JSON body of
 * `PUT /api/device-links/{code}/decision`. */
[[nodiscard]] std::string decisionMessageText(DeviceLinkDecision decision);

/**
 * @returns The decision a player sent as @p text; a member it does not know is
 * ignored.
 * @throws InvalidMasterMessageException naming `/decision` when it is not one
 * of DEVICE_LINK_DECISION_TEXTS.
 */
[[nodiscard]] DeviceLinkDecision decisionMessageFromText(std::string_view text);

/**
 * @returns The session the master answered as @p text, what
 * createdSessionText() writes.
 * @throws InvalidMasterMessageException naming the first member that is not.
 */
[[nodiscard]] CreatedSession createdSessionFromText(std::string_view text);

/**
 * @returns Whether @p text has the shape of a game ticket: GAME_TICKET_PREFIX,
 * then lowercase hexadecimal digits, GAME_TICKET_SIZE in all.
 */
[[nodiscard]] bool isWellFormedGameTicket(std::string_view text);

/** @returns @p grant as the JSON answer of `POST /api/users/me/tickets`. */
[[nodiscard]] std::string gameTicketGrantText(const GameTicketGrant& grant);

/**
 * @returns The ticket the master answered as @p text: an object whose
 * `ticket` is well formed (isWellFormedGameTicket()) and whose `expiresAt` is
 * a whole number of seconds from 0.
 * @throws InvalidMasterMessageException naming the first member that is not.
 */
[[nodiscard]] GameTicketGrant gameTicketGrantFromText(std::string_view text);

/** @returns @p state as the JSON answer of
 * `GET /api/users/me/tickets/{ticket}`. */
[[nodiscard]] std::string gameTicketStateText(const GameTicketState& state);

/**
 * @returns The `detail` of the problem document (RFC 9457) @p text holds, or
 * an empty text when @p text is no JSON object or has no `detail` text.
 */
[[nodiscard]] std::string problemDetailOf(std::string_view text);

/** @returns @p wallet as the JSON answer of `GET /api/users/me/wallet`. */
[[nodiscard]] std::string walletText(const Wallet& wallet);

/**
 * @returns The wallet the master answered as @p text.
 * @throws InvalidMasterMessageException naming the first member that is
 * missing or out of its bounds.
 */
[[nodiscard]] Wallet walletFromText(std::string_view text);

/** @returns @p listing as the JSON answer of
 * `GET /api/users/me/wallet/movements`, newest first. */
[[nodiscard]] std::string movementListingText(const MovementListing& listing);

/** @returns @p request as the JSON body of
 * `POST /api/users/me/wallet/exchanges`. */
[[nodiscard]] std::string exchangeRequestText(const ExchangeRequest& request);

/**
 * @returns The request of the cashier a player sent as @p text; a member it
 * does not know is ignored.
 * @throws InvalidMasterMessageException naming the first member that is
 * missing, of an unknown currency, or no whole number of 32 bits.
 */
[[nodiscard]] ExchangeRequest exchangeRequestFromText(std::string_view text);

/** @returns @p exchange as the JSON answer of
 * `GET /api/users/me/wallet/exchanges/{id}`. */
[[nodiscard]] std::string walletExchangeText(const WalletExchange& exchange);

/** @returns @p receipt as the JSON answer of
 * `POST /api/users/me/wallet/exchanges`: the exchange and its `wallet`. */
[[nodiscard]] std::string exchangeReceiptText(const ExchangeReceipt& receipt);

/**
 * @returns The receipt the master answered as @p text.
 * @throws InvalidMasterMessageException naming the first member that is
 * missing or out of its bounds.
 */
[[nodiscard]] ExchangeReceipt exchangeReceiptFromText(std::string_view text);

/** @returns @p looks as the JSON answer of `GET /api/users/me/looks`. */
[[nodiscard]] std::string ownedLooksText(const OwnedLooks& looks);

/**
 * @returns The looks the master answered as @p text.
 * @throws InvalidMasterMessageException naming the first item that is no
 * name of identifiers::SHIP_LOOK_ITEM_NAMES, or a list longer than it.
 */
[[nodiscard]] OwnedLooks ownedLooksFromText(std::string_view text);

/** @returns @p purchase as the JSON answer of
 * `PUT /api/users/me/looks/{item}`. */
[[nodiscard]] std::string lookPurchaseText(const LookPurchase& purchase);

/**
 * @returns The purchase the master answered as @p text.
 * @throws InvalidMasterMessageException naming the first member that is
 * missing or out of its bounds.
 */
[[nodiscard]] LookPurchase lookPurchaseFromText(std::string_view text);

/** @returns How @p reason is written: one of MOVEMENT_REASON_TEXTS. */
[[nodiscard]] std::string_view movementReasonText(MovementReason reason);

/** @returns The reason @p text writes, nothing when it is none of
 * MOVEMENT_REASON_TEXTS. */
[[nodiscard]] std::optional<MovementReason> movementReasonOf(
    std::string_view text);

/** @returns How @p currency is written: one of CURRENCY_TEXTS. */
[[nodiscard]] std::string_view currencyText(Currency currency);

/** @returns The currency @p text writes, nothing when it is none of
 * CURRENCY_TEXTS. */
[[nodiscard]] std::optional<Currency> currencyOf(std::string_view text);

/** @returns @p listing as the JSON answer of `GET /api/casino/rooms`. */
[[nodiscard]] std::string casinoRoomListingText(
    const CasinoRoomListing& listing);

/**
 * @returns The rooms the master answered as @p text.
 * @throws InvalidMasterMessageException naming the first member that is
 * missing or out of its bounds.
 */
[[nodiscard]] CasinoRoomListing casinoRoomListingFromText(
    std::string_view text);

/** @returns @p state as the JSON answer of `GET /api/casino/rooms/{room}`:
 * winningNumber and payout written null until the number is drawn. */
[[nodiscard]] std::string casinoRoomStateText(const CasinoRoomState& state);

/**
 * @returns The state of a room the master answered as @p text.
 * @throws InvalidMasterMessageException naming the first member that is
 * missing or out of its bounds.
 */
[[nodiscard]] CasinoRoomState casinoRoomStateFromText(std::string_view text);

/** @returns @p bets as the JSON body of
 * `PUT /api/casino/rooms/{room}/rounds/{round}/bets/me`. */
[[nodiscard]] std::string rouletteBetsText(const RouletteBets& bets);

/**
 * @returns The bets a player sent as @p text, read as the wire carries them:
 * whether each lies on the cloth, their count and their total are the
 * master's to check.
 * @throws InvalidMasterMessageException naming the first member that is
 * missing, a kind that is none of ROULETTE_BET_KIND_TEXTS, a number past 255,
 * an amount past 32 bits, or a list longer than MAXIMUM_READ_ROULETTE_BETS.
 */
[[nodiscard]] RouletteBets rouletteBetsFromText(std::string_view text);

/** @returns @p receipt as the JSON answer of
 * `PUT /api/casino/rooms/{room}/rounds/{round}/bets/me`. */
[[nodiscard]] std::string betsReceiptText(const BetsReceipt& receipt);

/**
 * @returns The receipt the master answered as @p text.
 * @throws InvalidMasterMessageException naming the first member that is
 * missing or out of its bounds.
 */
[[nodiscard]] BetsReceipt betsReceiptFromText(std::string_view text);

/** @returns @p bets as the JSON answer of
 * `GET /api/casino/rooms/{room}/rounds/{round}/bets/me`: payout and
 * winningNumber written null until the number is drawn. */
[[nodiscard]] std::string roundBetsText(const RoundBets& bets);

/**
 * @returns The bets of a round the master answered as @p text.
 * @throws InvalidMasterMessageException naming the first member that is
 * missing or out of its bounds.
 */
[[nodiscard]] RoundBets roundBetsFromText(std::string_view text);

/** @returns How @p kind is written: one of ROULETTE_BET_KIND_TEXTS. */
[[nodiscard]] std::string_view rouletteBetKindText(RouletteBetKind kind);

/** @returns The kind @p text writes, nothing when it is none of
 * ROULETTE_BET_KIND_TEXTS. */
[[nodiscard]] std::optional<RouletteBetKind> rouletteBetKindOf(
    std::string_view text);

/** @returns How @p phase is written: one of ROULETTE_PHASE_TEXTS. */
[[nodiscard]] std::string_view roulettePhaseText(RoulettePhase phase);

/** @returns The phase @p text writes, nothing when it is none of
 * ROULETTE_PHASE_TEXTS. */
[[nodiscard]] std::optional<RoulettePhase> roulettePhaseOf(
    std::string_view text);

}  // namespace rtype::network

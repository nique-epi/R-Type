#pragma once

#include <cstddef>
#include <nlohmann/json_fwd.hpp>
#include <string>
#include <string_view>
#include "GameListing.hpp"
#include "SampleListing.hpp"
#include "ServerHeartbeat.hpp"
#include "ServerListing.hpp"
#include "ServerRegistration.hpp"

namespace rtype::network::testing {

/** @returns The registration of "Paris 1", with its address. */
[[nodiscard]] ServerRegistration parisRegistration();

/**
 * @returns The heartbeat of "Paris 1": a waiting game of one player, a game of
 * two players in progress, and every measure but the ping.
 */
[[nodiscard]] ServerHeartbeat parisHeartbeat();

/**
 * @returns The games of "Paris 1" as a player lists them: the waiting game of
 * one player, and the game of two players in progress.
 */
[[nodiscard]] GameListing parisGameListing();

/** @returns "Paris 1" as the master lists it: id PARIS_ID, online, with the
 * games and players of parisHeartbeat(); its registration holds no
 * probeSecret, which a list never holds. */
[[nodiscard]] ServerListing parisServerListing();

/** @returns One sample taken at SAMPLE_DATE of the games and players of
 * parisHeartbeat(), with its tick p95 and its loss only. */
[[nodiscard]] SampleListing parisSampleListing();

/** @returns The listing of "Paris 1" with @p value at @p pointer. */
[[nodiscard]] std::string gameListingTextWith(std::string_view pointer,
                                              const nlohmann::json& value);

/**
 * @returns A listing of @p gameCount waiting games of no player, numbered from
 * 1, as JSON.
 */
[[nodiscard]] std::string gameListingTextOfGames(std::size_t gameCount);

/**
 * @returns What gameListingFromText() says when it refuses @p text, or an empty
 * text when it accepts it.
 */
[[nodiscard]] std::string gameListingRefusal(std::string_view text);

/**
 * @returns The registration of "Paris 1" as JSON, with @p value at @p pointer,
 * a JSON Pointer (RFC 6901): the member is added or replaced.
 */
[[nodiscard]] std::string registrationTextWith(std::string_view pointer,
                                               const nlohmann::json& value);

/** @returns The registration of "Paris 1" without the member at @p pointer. */
[[nodiscard]] std::string registrationTextWithout(std::string_view pointer);

/** @returns The heartbeat of "Paris 1" with @p value at @p pointer. */
[[nodiscard]] std::string heartbeatTextWith(std::string_view pointer,
                                            const nlohmann::json& value);

/** @returns The heartbeat of "Paris 1" without the member at @p pointer. */
[[nodiscard]] std::string heartbeatTextWithout(std::string_view pointer);

/** @returns A JSON array of @p count distinct names of difficulties. */
[[nodiscard]] nlohmann::json difficultyNames(std::size_t count);

/**
 * @returns A heartbeat listing @p gameCount waiting games of no player,
 * numbered from 1, as JSON.
 */
[[nodiscard]] std::string heartbeatTextOfGames(std::size_t gameCount);

/**
 * @returns What registrationFromText() says when it refuses @p text, or an
 * empty text when it accepts it.
 */
[[nodiscard]] std::string registrationRefusal(std::string_view text);

/** @returns What createdServerFromText() says of @p text, as above. */
[[nodiscard]] std::string createdServerRefusal(std::string_view text);

/** @returns What heartbeatFromText() says of @p text, as above. */
[[nodiscard]] std::string heartbeatRefusal(std::string_view text);

/** @returns The list of servers of "Paris 1" with @p value at @p pointer. */
[[nodiscard]] std::string serverListingTextWith(std::string_view pointer,
                                                const nlohmann::json& value);

/** @returns A list of @p serverCount copies of "Paris 1", their ids from 1, as
 * JSON. */
[[nodiscard]] std::string serverListingTextOfServers(std::size_t serverCount);

/**
 * @returns What serverListingFromText() says when it refuses @p text, or an
 * empty text when it accepts it.
 */
[[nodiscard]] std::string serverListingRefusal(std::string_view text);

/** @returns @p pointer between the quotes a refusal writes around it. */
[[nodiscard]] std::string quoted(std::string_view pointer);

}  // namespace rtype::network::testing

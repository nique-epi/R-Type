#pragma once

#include <cstddef>
#include <nlohmann/json_fwd.hpp>
#include <string>
#include <string_view>
#include "ServerHeartbeat.hpp"
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

/** @returns @p pointer between the quotes a refusal writes around it. */
[[nodiscard]] std::string quoted(std::string_view pointer);

}  // namespace rtype::network::testing

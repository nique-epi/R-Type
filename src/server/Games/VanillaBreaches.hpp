#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>
#include <string_view>
#include <vector>

namespace rtype::server {

/**
 * @brief Why the games a server creates with these values are not vanilla,
 * that is played by the rules the game ships with, so that their scores compare
 * with the scores of other servers.
 *
 * A game is vanilla when it reads the shipped waves file, starts from no set
 * seed (a seed lets the Bydos of every game be known in advance) and holds the
 * inputs for the default delay. The server judges its own games: this keeps
 * honest changes apart, not a host who lies, and a server rebuilt with other
 * constants cannot be told apart.
 *
 * @param wavesFingerprint WaveCatalog::fingerprint() of the waves file the
 * games read.
 * @param seed The seed the games start from; empty when each game draws its
 * own.
 * @param inputDelayTicks The ticks every game holds each input.
 * @returns MODIFIED_WAVES_BREACH, IMPOSED_SEED_BREACH and
 * CHANGED_INPUT_DELAY_BREACH, in this order, for each rule broken; empty when
 * the games are vanilla.
 */
[[nodiscard]] std::vector<std::string_view> vanillaBreaches(
    std::uint64_t wavesFingerprint, std::optional<std::uint32_t> seed,
    std::size_t inputDelayTicks);

}  // namespace rtype::server

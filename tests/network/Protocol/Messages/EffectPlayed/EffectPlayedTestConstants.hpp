#pragma once

#include <cstdint>
#include <string_view>

namespace rtype::network::testing {

/** @brief The body the effects of the tests play on, and an effect it plays:
 * `spark`, 5 bytes. */
constexpr std::uint32_t EFFECT_BODY_ID = 9;
constexpr std::string_view SPARK_EFFECT = "spark";

/** @brief Effect names at the bounds of a name of the content: 24 bytes, the
 * longest, 25 bytes, one too many, and names holding a capital letter and a
 * space. */
constexpr std::string_view LONGEST_EFFECT = "abcdefghijklmnopqrstuvwx";
constexpr std::string_view TOO_LONG_EFFECT = "abcdefghijklmnopqrstuvwxy";
constexpr std::string_view EFFECT_WITH_A_CAPITAL = "Spark";
constexpr std::string_view EFFECT_WITH_A_SPACE = "big spark";

}  // namespace rtype::network::testing

#include "BanWords.hpp"
#include <chrono>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include "AdminConsoleConstants.hpp"
#include "BanKind.hpp"
#include "BanTarget.hpp"
#include "EndpointText.hpp"
#include "EngineException.hpp"
#include "NetworkException.hpp"
#include "WholeNumber.hpp"
#include "WireTypes.hpp"

namespace rtype::server {

namespace {

/** @returns The address @p word names; nothing when it is no IPv4 address. */
std::optional<BanTarget> addressTargetOf(std::string_view word) {
  try {
    return BanTarget{.kind = BanKind::Address,
                     .text = network::ipv4AddressFromText(word)};
  } catch (const network::InvalidIpv4AddressException&) {
    return std::nullopt;
  }
}

/** @returns What @p letter stands for; nothing for a letter of no unit. */
std::optional<std::chrono::seconds> banUnitOf(char letter) {
  switch (letter) {
    case MINUTE_LETTER:
      return BAN_MINUTE;
    case HOUR_LETTER:
      return BAN_HOUR;
    case DAY_LETTER:
      return BAN_DAY;
    default:
      return std::nullopt;
  }
}

/** @returns The count @p digits writes, read as engine::wholeNumberFrom()
 * reads a number; nothing when it is no whole number, or a count outside 1 to
 * MAXIMUM_BAN_COUNT. */
std::optional<std::uint32_t> banCountOf(std::string_view digits) {
  std::int64_t count = 0;
  try {
    count = engine::wholeNumberFrom(std::string{digits});
  } catch (const engine::CommandRefusedException&) {
    return std::nullopt;
  }
  if (count < 1 || std::cmp_greater(count, MAXIMUM_BAN_COUNT)) {
    return std::nullopt;
  }
  return static_cast<std::uint32_t>(count);
}

}  // namespace

std::optional<BanTarget> banTargetOf(std::string_view word) {
  std::optional<BanTarget> address = addressTargetOf(word);
  if (address.has_value()) {
    return address;
  }
  if (network::isValidNickname(word)) {
    return BanTarget{.kind = BanKind::Nickname, .text = std::string{word}};
  }
  return std::nullopt;
}

std::optional<std::chrono::seconds> banLengthOf(std::string_view word) {
  if (word.empty()) {
    return std::nullopt;
  }
  const std::optional<std::chrono::seconds> unit = banUnitOf(word.back());
  const std::optional<std::uint32_t> count =
      banCountOf(word.substr(0, word.size() - 1));
  if (!unit.has_value() || !count.has_value()) {
    return std::nullopt;
  }
  return *unit * *count;
}

}  // namespace rtype::server

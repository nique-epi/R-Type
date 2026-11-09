#include "ReleaseVersion.hpp"
#include <array>
#include <charconv>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <system_error>
#include <utility>
#include "ReleaseConstants.hpp"

namespace rtype::client {

namespace {

/** @returns The whole number @p text starts with and what follows it, or
 * nothing when it starts with no digit or passes std::uint32_t. */
std::optional<std::pair<std::uint32_t, std::string_view>> numberAt(
    std::string_view text) {
  const std::string digits{text};
  std::uint32_t number = 0;
  const char* const end = digits.data() + digits.size();
  const auto [stop, error] = std::from_chars(digits.data(), end, number);
  if (error != std::errc{} || stop == digits.data()) {
    return std::nullopt;
  }
  return std::pair{number,
                   text.substr(static_cast<std::size_t>(stop - digits.data()))};
}

}  // namespace

std::optional<ReleaseVersion> releaseVersionOf(std::string_view text) {
  std::array<std::uint32_t, VERSION_PARTS> parts{};
  for (std::size_t part = 0; part < parts.size(); ++part) {
    if (part > 0) {
      if (!text.starts_with(VERSION_SEPARATOR)) {
        return std::nullopt;
      }
      text.remove_prefix(1);
    }
    const auto read = numberAt(text);
    if (!read.has_value()) {
      return std::nullopt;
    }
    parts.at(part) = read->first;
    text = read->second;
  }
  if (!text.empty()) {
    return std::nullopt;
  }
  return ReleaseVersion{.majorNumber = parts[0],
                        .minorNumber = parts[1],
                        .patchNumber = parts[2]};
}

}  // namespace rtype::client

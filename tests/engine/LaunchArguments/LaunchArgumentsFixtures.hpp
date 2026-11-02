#pragma once

#include <array>
#include <exception>
#include <string>
#include <vector>
#include "LaunchOption.hpp"

/**
 * @brief One call of a record: the option it belongs to and the value it got.
 */
struct RecordedCall {
  std::string option;
  std::string value;

  bool operator==(const RecordedCall&) const = default;
};

/**
 * @brief What a record of the tests throws to refuse its value.
 */
class RefusedValue final : public std::exception {};

/**
 * @returns VALUE_OPTION and FLAG_OPTION, whose records append each call to
 *          @p calls.
 */
[[nodiscard]] std::array<rtype::engine::LaunchOption, 2> recordingOptions(
    std::vector<RecordedCall>& calls);

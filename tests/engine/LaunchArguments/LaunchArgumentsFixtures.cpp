#include "LaunchArgumentsFixtures.hpp"
#include <array>
#include <cstddef>
#include <span>
#include <string>
#include <string_view>
#include <vector>
#include "LaunchArgumentsTestConstants.hpp"
#include "LaunchOption.hpp"
#include "LaunchOptionReader.hpp"
#include "LaunchOptionReadingTestConstants.hpp"

using rtype::engine::LaunchOption;
using rtype::engine::LaunchOptionReader;

std::array<LaunchOption, 2> recordingOptions(std::vector<RecordedCall>& calls) {
  return {
      LaunchOption{.name = VALUE_OPTION,
                   .takesValue = true,
                   .record =
                       [&calls](std::string_view value) {
                         calls.push_back(
                             RecordedCall{.option = std::string(VALUE_OPTION),
                                          .value = std::string(value)});
                       }},
      LaunchOption{.name = FLAG_OPTION,
                   .takesValue = false,
                   .record =
                       [&calls](std::string_view value) {
                         calls.push_back(
                             RecordedCall{.option = std::string(FLAG_OPTION),
                                          .value = std::string(value)});
                       }},
  };
}

LaunchOptionReader recordingReader(std::vector<RecordedCall>& calls) {
  return [&calls](std::span<const std::string_view> arguments) -> std::size_t {
    if (arguments.empty() || arguments.front() != READER_OPTION) {
      return NO_ARGUMENT_TAKEN;
    }
    if (arguments.size() < AN_OPTION_AND_ITS_VALUE) {
      throw RefusedValue{};
    }
    calls.push_back(RecordedCall{.option = std::string(READER_OPTION),
                                 .value = std::string(arguments[1])});
    return AN_OPTION_AND_ITS_VALUE;
  };
}

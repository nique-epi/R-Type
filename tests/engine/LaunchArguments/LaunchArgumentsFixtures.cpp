#include "LaunchArgumentsFixtures.hpp"
#include <array>
#include <string>
#include <string_view>
#include <vector>
#include "LaunchArgumentsTestConstants.hpp"
#include "LaunchOption.hpp"

using rtype::engine::LaunchOption;

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

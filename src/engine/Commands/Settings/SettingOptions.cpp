#include "SettingOptions.hpp"
#include <span>
#include <string>
#include <string_view>
#include <vector>
#include "ForcedSetting.hpp"
#include "LaunchArguments.hpp"
#include "LaunchArgumentsConstants.hpp"
#include "LaunchOption.hpp"
#include "SettingLine.hpp"

namespace rtype::engine {

LaunchOption setOption(std::vector<std::string>& forcedLines) {
  return LaunchOption{
      .name = SET_OPTION,
      .takesValue = true,
      .record = [&forcedLines](std::string_view assignment) {
        forcedLines.push_back(settingLineFromSetOption(assignment));
      }};
}

LaunchOption settingOption(ForcedSetting forced,
                           std::vector<std::string>& forcedLines) {
  return LaunchOption{.name = forced.option,
                      .takesValue = true,
                      .record = [setting = forced.setting,
                                 &forcedLines](std::string_view value) {
                        forcedLines.push_back(settingLine(setting, value));
                      }};
}

std::string setOptionEntry(std::string_view configurationFile,
                           std::span<const std::string_view> settings) {
  std::string names;
  for (const std::string_view setting : settings) {
    names += names.empty() ? "" : ", ";
    names += setting;
  }
  return usageEntry(
      std::string(SET_OPTION) + " <name>=<value>",
      "Gives a setting a value, as the line `set <name> <value>` of " +
          std::string(configurationFile) +
          " does. It wins over that file and can be given several times. "
          "Settings: " +
          names + ".");
}

}  // namespace rtype::engine

#include "SettingOptions.hpp"
#include <string>
#include <string_view>
#include <vector>
#include "ForcedSetting.hpp"
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

}  // namespace rtype::engine

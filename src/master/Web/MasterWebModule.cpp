#include "MasterWebModule.hpp"
#include <filesystem>
#include <string>
#include <vector>
#include "ConfigurationRegistry.hpp"
#include "InstallContext.hpp"
#include "MasterConstants.hpp"
#include "MasterHttpServer.hpp"
#include "MasterModuleConstants.hpp"
#include "TextSettingDefinition.hpp"

namespace rtype::master {

std::string MasterWebModule::name() const {
  return std::string{WEB_MODULE_NAME};
}

std::vector<std::string> MasterWebModule::dependencies() const {
  return {std::string{HTTP_MODULE_NAME}};
}

void MasterWebModule::install(engine::InstallContext& context) {
  webSetting_ = &context.require<engine::ConfigurationRegistry>().declareText(
      engine::TextSettingDefinition{
          .name = std::string{WEB_SETTING_NAME},
          .defaultValue = {},
          .help = "folder of the web site the master serves under /; empty "
                  "for " +
                  std::string{DEFAULT_WEB_FOLDER} +
                  " in the folder the master is started from",
          .validate = {}});
  context.require<MasterHttpServer>().serveSite(
      [this] { return siteFolder(); });
}

void MasterWebModule::start() {}

void MasterWebModule::stop() {}

std::filesystem::path MasterWebModule::siteFolder() const {
  const std::string& folder = webSetting_->text();
  return folder.empty() ? std::filesystem::path{DEFAULT_WEB_FOLDER}
                        : std::filesystem::path{folder};
}

}  // namespace rtype::master

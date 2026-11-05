#pragma once

#include <filesystem>
#include <string>
#include <vector>
#include "IModule.hpp"
#include "InstallContext.hpp"
#include "SettingValue.hpp"

namespace rtype::master {

/**
 * @brief The module of the web site of the master: it has the HTTP server
 * serve the files `bun run build` writes from `web/` under `/`, beside the API.
 *
 * install() declares the `web` setting, the folder of the site. While it is
 * empty, the folder is DEFAULT_WEB_FOLDER, in the folder the master is started
 * from. The HTTP server reads it when it opens, after the settings are
 * applied; a folder that does not exist leaves the master serving its API
 * only. Turned off, the master serves no site at all.
 */
class MasterWebModule final : public engine::IModule {
 public:
  [[nodiscard]] std::string name() const override;
  [[nodiscard]] std::vector<std::string> dependencies() const override;

  /**
   * @brief Declares the `web` setting and hands the folder it names to the
   * MasterHttpServer; opens nothing.
   */
  void install(engine::InstallContext& context) override;

  /** @brief Does nothing: the HTTP module opens the site with its port. */
  void start() override;

  /** @brief Does nothing: the HTTP module closes the site with its port. */
  void stop() override;

 private:
  [[nodiscard]] std::filesystem::path siteFolder() const;

  const engine::SettingValue* webSetting_{nullptr};
};

}  // namespace rtype::master

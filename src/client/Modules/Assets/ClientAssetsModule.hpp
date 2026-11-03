#pragma once

#include <memory>
#include <string>
#include <vector>
#include "AssetLibrary.hpp"
#include "AssetLoad.hpp"
#include "IModule.hpp"
#include "InstallContext.hpp"
#include "Logger.hpp"

namespace rtype::client {

/**
 * @brief The module of the assets of the client: the AssetLibrary of the
 * assets folder, which it provides to the modules installed after it.
 *
 * install() finds the assets folder next to the program or in the folder
 * above, and builds the library, which loads nothing. start() writes the
 * folder in the journal, then runs the loads it was built with, in their
 * order. stop() releases nothing: the library lives as long as the module,
 * after every screen that draws its textures.
 */
class ClientAssetsModule final : public engine::IModule {
 public:
  /** @param startupLoads Run by start(), in this order, once the library is
   * built. */
  explicit ClientAssetsModule(std::vector<AssetLoad> startupLoads);

  /** @returns ASSETS_MODULE_NAME. */
  [[nodiscard]] std::string name() const override;
  /** @returns LOGGER_MODULE_NAME: the folder is logged once the journal is
   * open. */
  [[nodiscard]] std::vector<std::string> dependencies() const override;

  /**
   * @throws UnknownExecutablePathException when the system does not tell
   * where the program is.
   * @throws AssetFolderNotFoundException when no assets folder lies next to
   * the program or in the folder above.
   */
  void install(engine::InstallContext& context) override;

  /** @throws AssetLoadingException when a load it runs cannot load an
   * asset. */
  void start() override;

  void stop() override;

 private:
  std::vector<AssetLoad> startupLoads_;
  std::unique_ptr<AssetLibrary> library_;
  logging::Logger logger_;
};

}  // namespace rtype::client

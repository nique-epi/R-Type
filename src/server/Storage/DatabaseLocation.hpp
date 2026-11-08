#pragma once

#include <filesystem>
#include <functional>
#include "IListeningPort.hpp"
#include "SettingValue.hpp"

namespace rtype::server {

/**
 * @brief Where the file of a server is: the value of its database setting, or,
 * while that is empty, the file of its port in a folder (see databaseFileOf()).
 * Read at each call, so the value the configuration gave after the install is
 * the one used.
 */
class DatabaseLocation {
 public:
  /** @param setting The database setting; it must outlive the location.
   * @param folder The folder of the file while the setting is empty.
   * @param port Gives the port of the server; it must outlive the location. */
  DatabaseLocation(const engine::SettingValue& setting,
                   std::filesystem::path folder, const IListeningPort& port);

  [[nodiscard]] std::filesystem::path file() const;

 private:
  std::reference_wrapper<const engine::SettingValue> setting_;
  std::filesystem::path folder_;
  std::reference_wrapper<const IListeningPort> port_;
};

}  // namespace rtype::server

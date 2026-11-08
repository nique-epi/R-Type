#pragma once

#include <functional>
#include <string>
#include "IMasterAddress.hpp"
#include "SettingValue.hpp"

namespace rtype::client {

/** @brief The master the server map reads: the one the `master-url` setting
 * names, defaultMasterUrl() until the configuration names another. */
class ConfiguredMaster final : public IMasterAddress {
 public:
  /** @param masterUrl The master-url setting, which must outlive this object.
   */
  explicit ConfiguredMaster(const engine::SettingValue& masterUrl);

  [[nodiscard]] std::string url() const override;

 private:
  std::reference_wrapper<const engine::SettingValue> masterUrl_;
};

}  // namespace rtype::client

#include "ConfiguredMaster.hpp"
#include <string>
#include "SettingValue.hpp"

namespace rtype::client {

ConfiguredMaster::ConfiguredMaster(const engine::SettingValue& masterUrl)
    : masterUrl_(masterUrl) {}

std::string ConfiguredMaster::url() const { return masterUrl_.get().text(); }

}  // namespace rtype::client

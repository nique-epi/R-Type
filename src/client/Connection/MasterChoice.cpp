#include "MasterChoice.hpp"
#include <optional>
#include <string>
#include <utility>
#include "Endpoint.hpp"
#include "MasterAccess.hpp"
#include "MasterChoiceParts.hpp"
#include "MasterUrl.hpp"
#include "SettingId.hpp"

namespace rtype::client {

MasterChoice::MasterChoice(MasterChoiceParts parts)
    : parts_(std::move(parts)) {}

std::string MasterChoice::url() const {
  const std::optional<network::Endpoint>& chosen = parts_.settings.get().master;
  if (chosen.has_value()) {
    return masterUrlOf(*chosen);
  }
  return parts_.masterUrl.get().text();
}

void MasterChoice::settingChanged(SettingId changedSetting) {
  if (changedSetting != SettingId::Master) {
    return;
  }
  parts_.link.get().retarget(
      network::MasterAccess{.url = url(), .key = std::string{}});
  parts_.servers.get().refresh();
}

}  // namespace rtype::client

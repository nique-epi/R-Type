#include "ServerKeys.hpp"
#include <chrono>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include "MasterDatabase.hpp"
#include "ServerKeyRecord.hpp"
#include "ServerKeyText.hpp"

namespace rtype::master {

ServerKeys::ServerKeys(MasterDatabase& database) : repository_(database) {}

std::string ServerKeys::issue(const std::string& name,
                              std::chrono::sys_seconds now) {
  std::string key = newServerKeyText();
  repository_.insert(ServerKeyRecord{
      .name = name, .fingerprint = fingerprintOf(key), .issuedAt = now});
  return key;
}

std::optional<std::int64_t> ServerKeys::keyIdOf(std::string_view keyText) {
  if (!isWellFormedServerKey(keyText)) {
    return std::nullopt;
  }
  return repository_.idOfFingerprint(fingerprintOf(keyText));
}

}  // namespace rtype::master

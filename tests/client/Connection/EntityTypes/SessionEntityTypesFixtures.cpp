#include "SessionEntityTypesFixtures.hpp"
#include <cstdint>
#include <filesystem>
#include <string>
#include "ContentAnnouncement.hpp"
#include "ContentFingerprint.hpp"
#include "EntityType.hpp"
#include "LocalContent.hpp"
#include "NumberedEntityType.hpp"
#include "SessionEntityTypesTestConstants.hpp"

namespace rtype::client::testing {

LocalContent testLocalContent() {
  LocalContent content{
      .fingerprint = {},
      .types = {game::NumberedEntityType{.name = std::string{DRAWN_ENEMY_NAME},
                                         .side = game::EntityType::Enemy},
                game::NumberedEntityType{.name = std::string{DRAWN_SHIP_NAME},
                                         .side = game::EntityType::Player}},
      .gun = LOCAL_SHIP_GUN};
  content.fingerprint.bytes.fill(LOCAL_FINGERPRINT_BYTE);
  return content;
}

network::ContentAnnouncement localAnnouncement() {
  return network::ContentAnnouncement{
      .fingerprint = testLocalContent().fingerprint, .size = 0, .port = 0};
}

network::ContentAnnouncement otherAnnouncement(std::uint32_t size,
                                               std::uint16_t port) {
  network::ContentAnnouncement announcement{
      .fingerprint = {}, .size = size, .port = port};
  announcement.fingerprint.bytes.fill(OTHER_FINGERPRINT_BYTE);
  return announcement;
}

std::filesystem::path unusedCacheFolder() {
  return std::filesystem::temp_directory_path() / UNUSED_CACHE_FOLDER;
}

}  // namespace rtype::client::testing

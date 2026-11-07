#include "ContentPackFixtures.hpp"
#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>
#include "ByteWriter.hpp"
#include "ContentFile.hpp"
#include "ContentPackTestConstants.hpp"

namespace rtype::network::testing {

std::vector<identifiers::ContentFile> twoContentFiles() {
  return {identifiers::ContentFile{.path = std::string{DRIFTER_SHEET_PATH},
                                   .bytes = std::string{DRIFTER_SHEET_BYTES}},
          identifiers::ContentFile{.path = std::string{BYDOMANCER_SPRITES_PATH},
                                   .bytes = std::string{BYDOMANCER_PIXELS}}};
}

std::vector<identifiers::ContentFile> filesNumbered(std::size_t count) {
  std::vector<identifiers::ContentFile> files;
  for (std::size_t index = 0; index < count; ++index) {
    std::string path{NUMBERED_FILE_PREFIX};
    path.push_back(static_cast<char>('0' + (index / DECIMAL_BASE)));
    path.push_back(static_cast<char>('0' + (index % DECIMAL_BASE)));
    files.push_back(identifiers::ContentFile{
        .path = path, .bytes = std::string(1, ONE_BYTE)});
  }
  return files;
}

std::vector<identifiers::ContentFile> oneFileOfSize(std::size_t size) {
  return {identifiers::ContentFile{.path = std::string{ONE_LETTER_PATH},
                                   .bytes = std::string(size, ONE_BYTE)}};
}

std::vector<std::byte> handWrittenPack(std::uint8_t version,
                                       std::string_view path) {
  ByteWriter writer;
  writer.writeUint8(version);
  writer.writeUint16(ONE_FILE);
  writer.writeString(path);
  writer.writeUint32(ONE_FILE_BYTE);
  writer.writeUint8(static_cast<std::uint8_t>(ONE_BYTE));
  return writer.bytes();
}

std::vector<std::byte> packAnnouncing(std::uint16_t count) {
  ByteWriter writer;
  writer.writeUint8(READ_FORMAT_VERSION);
  writer.writeUint16(count);
  return writer.bytes();
}

}  // namespace rtype::network::testing

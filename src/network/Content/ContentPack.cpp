#include "ContentPack.hpp"
#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <utility>
#include <vector>
#include "ByteReader.hpp"
#include "ByteText.hpp"
#include "ByteWriter.hpp"
#include "ContentFile.hpp"
#include "ContentFingerprint.hpp"
#include "ContentPackConstants.hpp"
#include "Hashing.hpp"
#include "HashingConstants.hpp"
#include "NetworkException.hpp"

namespace rtype::network {

namespace {

static_assert(CONTENT_FINGERPRINT_CONTEXT.size() == HASH_CONTEXT_LENGTH);
static_assert(CONTENT_FINGERPRINT_BYTES == HASH_BYTES);

/** @returns Whether @p name may name a folder or a file of a path. */
bool isPathPart(std::string_view name) {
  return !name.empty() && name != CURRENT_FOLDER_NAME &&
         name != PARENT_FOLDER_NAME;
}

/** @throws InvalidContentPackException when @p files break a rule
 * encodeContentPack() states, but its size. */
void checkContentFiles(std::span<const identifiers::ContentFile> files) {
  if (files.size() > MAXIMUM_CONTENT_PACK_FILES) {
    throw TooManyContentFilesException(files.size());
  }
  for (std::size_t index = 0; index < files.size(); ++index) {
    if (!isContentPath(files[index].path)) {
      throw InvalidContentPathException(files[index].path);
    }
    if (index > 0 && files[index - 1].path >= files[index].path) {
      throw UnorderedContentFilesException(files[index].path);
    }
  }
}

/** @returns The next file @p reader holds; its size is checked against what
 * remains before any of its bytes is copied. */
identifiers::ContentFile readContentFile(ByteReader& reader) {
  std::string path = reader.readString();
  const std::uint32_t size = reader.readUint32();
  return identifiers::ContentFile{.path = std::move(path),
                                  .bytes = textOf(reader.readBytes(size))};
}

/** @returns The files of @p pack, read as decodeContentPack() says, before
 * their rules are checked. */
std::vector<identifiers::ContentFile> readContentFiles(
    std::span<const std::byte> pack) {
  ByteReader reader{pack};
  const std::uint8_t version = reader.readUint8();
  if (version != CONTENT_PACK_FORMAT_VERSION) {
    throw ContentPackVersionException(version);
  }
  const std::uint16_t count = reader.readUint16();
  if (count > MAXIMUM_CONTENT_PACK_FILES) {
    throw TooManyContentFilesException(count);
  }
  std::vector<identifiers::ContentFile> files;
  files.reserve(count);
  for (std::uint16_t index = 0; index < count; ++index) {
    files.push_back(readContentFile(reader));
  }
  if (reader.remaining() != 0) {
    throw ContentPackTrailingBytesException(reader.remaining());
  }
  return files;
}

}  // namespace

std::vector<std::byte> encodeContentPack(
    std::span<const identifiers::ContentFile> files) {
  checkContentFiles(files);
  ByteWriter writer;
  writer.writeUint8(CONTENT_PACK_FORMAT_VERSION);
  writer.writeUint16(static_cast<std::uint16_t>(files.size()));
  for (const identifiers::ContentFile& file : files) {
    if (file.bytes.size() > MAXIMUM_CONTENT_PACK_BYTES) {
      throw ContentPackTooLargeException(file.bytes.size());
    }
    writer.writeString(file.path);
    writer.writeUint32(static_cast<std::uint32_t>(file.bytes.size()));
    writer.writeBytes(bytesOf(file.bytes));
  }
  if (writer.bytes().size() > MAXIMUM_CONTENT_PACK_BYTES) {
    throw ContentPackTooLargeException(writer.bytes().size());
  }
  return writer.bytes();
}

std::vector<identifiers::ContentFile> decodeContentPack(
    std::span<const std::byte> pack) {
  if (pack.size() > MAXIMUM_CONTENT_PACK_BYTES) {
    throw ContentPackTooLargeException(pack.size());
  }
  std::vector<identifiers::ContentFile> files = readContentFiles(pack);
  checkContentFiles(files);
  return files;
}

ContentFingerprint contentFingerprintOf(std::span<const std::byte> pack) {
  return ContentFingerprint{.bytes = hashOf(pack, CONTENT_FINGERPRINT_CONTEXT)};
}

std::optional<std::vector<identifiers::ContentFile>> filesOfPack(
    std::span<const std::byte> pack, const ContentFingerprint& fingerprint) {
  if (contentFingerprintOf(pack) != fingerprint) {
    return std::nullopt;
  }
  try {
    return decodeContentPack(pack);
  } catch (const NetworkException&) {
    return std::nullopt;
  }
}

bool isContentPath(std::string_view path) {
  if (path.empty() || path.size() > MAXIMUM_CONTENT_PATH_LENGTH ||
      path.find_first_not_of(CONTENT_PATH_CHARACTERS) !=
          std::string_view::npos) {
    return false;
  }
  std::size_t start = 0;
  while (true) {
    const std::size_t end = path.find(CONTENT_PATH_SEPARATOR, start);
    if (!isPathPart(path.substr(start, end - start))) {
      return false;
    }
    if (end == std::string_view::npos) {
      return true;
    }
    start = end + 1;
  }
}

}  // namespace rtype::network

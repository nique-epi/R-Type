#include "ContentPackCache.hpp"
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <ios>
#include <optional>
#include <span>
#include <string>
#include <system_error>
#include <utility>
#include <vector>
#include "ByteText.hpp"
#include "ContentFile.hpp"
#include "ContentFingerprint.hpp"
#include "ContentFingerprintText.hpp"
#include "ContentLoadingConstants.hpp"
#include "ContentPack.hpp"
#include "ContentPackConstants.hpp"
#include "DataFileReading.hpp"
#include "EngineException.hpp"

namespace rtype::client {

namespace {

/** @returns Every byte of @p file, or nothing when it cannot be read or holds
 * more than a pack may. */
std::optional<std::string> bytesOf(const std::filesystem::path& file) {
  std::error_code error;
  const std::uintmax_t size = std::filesystem::file_size(file, error);
  if (error || size > network::MAXIMUM_CONTENT_PACK_BYTES) {
    return std::nullopt;
  }
  try {
    return engine::dataFileContent(file);
  } catch (const engine::EngineException&) {
    return std::nullopt;
  }
}

}  // namespace

ContentPackCache::ContentPackCache(std::filesystem::path folder)
    : folder_(std::move(folder)) {}

std::optional<std::vector<identifiers::ContentFile>> ContentPackCache::find(
    const network::ContentFingerprint& fingerprint) const {
  const std::filesystem::path file = fileOf(fingerprint);
  const std::optional<std::string> pack = bytesOf(file);
  if (!pack.has_value()) {
    return std::nullopt;
  }
  std::optional<std::vector<identifiers::ContentFile>> files =
      network::filesOfPack(network::bytesOf(*pack), fingerprint);
  std::error_code ignored;
  if (!files.has_value()) {
    std::filesystem::remove(file, ignored);
    return std::nullopt;
  }
  std::filesystem::last_write_time(
      file, std::filesystem::file_time_type::clock::now(), ignored);
  return files;
}

void ContentPackCache::keep(const network::ContentFingerprint& fingerprint,
                            std::span<const std::byte> pack) const {
  std::error_code error;
  std::filesystem::create_directories(folder_, error);
  const std::filesystem::path file = fileOf(fingerprint);
  std::filesystem::path written = file;
  written += PARTIAL_PACK_FILE_SUFFIX;
  {
    std::ofstream stream{written, std::ios::binary | std::ios::trunc};
    const std::string text = network::textOf(pack);
    stream.write(text.data(), static_cast<std::streamsize>(text.size()));
    if (!stream) {
      std::filesystem::remove(written, error);
      return;
    }
  }
  std::filesystem::rename(written, file, error);
  forgetTheOldest();
}

std::filesystem::path ContentPackCache::spriteFolderOf(
    const network::ContentFingerprint& fingerprint) const {
  return folder_ / network::fingerprintText(fingerprint);
}

std::filesystem::path ContentPackCache::fileOf(
    const network::ContentFingerprint& fingerprint) const {
  std::filesystem::path file = spriteFolderOf(fingerprint);
  file += PACK_FILE_EXTENSION;
  return file;
}

void ContentPackCache::forgetTheOldest() const {
  std::error_code error;
  std::vector<std::filesystem::directory_entry> packs;
  for (const std::filesystem::directory_entry& entry :
       std::filesystem::directory_iterator{folder_, error}) {
    if (entry.path().extension() == PACK_FILE_EXTENSION) {
      packs.push_back(entry);
    }
  }
  if (packs.size() <= MAXIMUM_CACHED_PACKS) {
    return;
  }
  std::ranges::sort(packs, [](const std::filesystem::directory_entry& first,
                              const std::filesystem::directory_entry& second) {
    return first.last_write_time() < second.last_write_time();
  });
  for (std::size_t index = 0; index < packs.size() - MAXIMUM_CACHED_PACKS;
       ++index) {
    std::filesystem::path sprites = packs[index].path();
    sprites.replace_extension();
    std::filesystem::remove_all(sprites, error);
    std::filesystem::remove(packs[index].path(), error);
  }
}

}  // namespace rtype::client

#pragma once

#include <cstddef>
#include <span>
#include <vector>
#include "ContentFile.hpp"
#include "ContentFingerprint.hpp"

namespace rtype::network {

/**
 * @brief Encodes the files of a content pack: CONTENT_PACK_FORMAT_VERSION as
 * a u8, the count of files as a u16, then for each file its path as a short
 * string, its size as a u32 and its bytes.
 *
 * @param files At most MAXIMUM_CONTENT_PACK_FILES files, in the strict order
 * of their paths, each path checked by isContentPath().
 * @returns The pack, MAXIMUM_CONTENT_PACK_BYTES at most.
 * @throws InvalidContentPathException, UnorderedContentFilesException or
 * TooManyContentFilesException when a rule of @p files is broken, and
 * ContentPackTooLargeException when the pack would be larger.
 */
[[nodiscard]] std::vector<std::byte> encodeContentPack(
    std::span<const identifiers::ContentFile> files);

/**
 * @brief Reads the files of a pack encodeContentPack() wrote, every size
 * bounded before it is read, so a pack from a server never makes the reader
 * read past it or allocate more than it holds.
 * @throws ContentPackTooLargeException when @p pack is larger than
 * MAXIMUM_CONTENT_PACK_BYTES; ContentPackVersionException for another format;
 * BufferUnderflowException when it is cut short;
 * ContentPackTrailingBytesException when bytes follow its last file; and what
 * encodeContentPack() throws for files it would refuse. Every one but the
 * underflow is an InvalidContentPackException.
 */
[[nodiscard]] std::vector<identifiers::ContentFile> decodeContentPack(
    std::span<const std::byte> pack);

/**
 * @returns The fingerprint of every byte of @p pack: hydro_hash_hash() of
 * libhydrogen, in CONTENT_FINGERPRINT_CONTEXT, without key.
 */
[[nodiscard]] ContentFingerprint contentFingerprintOf(
    std::span<const std::byte> pack);

/**
 * @returns Whether @p path may name a file of a pack: 1 to
 * MAXIMUM_CONTENT_PATH_LENGTH characters of CONTENT_PATH_CHARACTERS, made of
 * folder names separated by CONTENT_PATH_SEPARATOR, none of them empty,
 * CURRENT_FOLDER_NAME or PARENT_FOLDER_NAME, so the path stays below the
 * folder it is written in.
 */
[[nodiscard]] bool isContentPath(std::string_view path);

}  // namespace rtype::network

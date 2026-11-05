#include "ZstdCodec.hpp"
#include <zstd.h>
#include <cstddef>
#include <memory>
#include <optional>
#include <span>
#include <vector>
#include "NetworkException.hpp"
#include "ServerToClientDictionary.hpp"
#include "ZstdCodecConstants.hpp"

namespace rtype::network {

namespace {

using CompressionContext = std::unique_ptr<ZSTD_CCtx, decltype(&ZSTD_freeCCtx)>;
using DecompressionContext =
    std::unique_ptr<ZSTD_DCtx, decltype(&ZSTD_freeDCtx)>;
using CompressionDictionary =
    std::unique_ptr<ZSTD_CDict, decltype(&ZSTD_freeCDict)>;
using DecompressionDictionary =
    std::unique_ptr<ZSTD_DDict, decltype(&ZSTD_freeDDict)>;

/** @throws CompressionUnavailableException when @p result is a zstd error. */
void requireSuccess(std::size_t result) {
  if (ZSTD_isError(result) != 0) {
    throw CompressionUnavailableException();
  }
}

const ZSTD_CDict& compressionDictionary() {
  static const CompressionDictionary dictionary{
      ZSTD_createCDict(serverToClientDictionary().data(),
                       serverToClientDictionary().size(), COMPRESSION_LEVEL),
      &ZSTD_freeCDict};
  if (!dictionary) {
    throw CompressionUnavailableException();
  }
  return *dictionary;
}

const ZSTD_DDict& decompressionDictionary() {
  static const DecompressionDictionary dictionary{
      ZSTD_createDDict(serverToClientDictionary().data(),
                       serverToClientDictionary().size()),
      &ZSTD_freeDDict};
  if (!dictionary) {
    throw CompressionUnavailableException();
  }
  return *dictionary;
}

CompressionContext makeCompressionContext() {
  CompressionContext context{ZSTD_createCCtx(), &ZSTD_freeCCtx};
  if (!context) {
    throw CompressionUnavailableException();
  }
  requireSuccess(ZSTD_CCtx_setParameter(context.get(), ZSTD_c_contentSizeFlag,
                                        FRAME_OPTION_OFF));
  requireSuccess(ZSTD_CCtx_setParameter(context.get(), ZSTD_c_dictIDFlag,
                                        FRAME_OPTION_OFF));
  requireSuccess(ZSTD_CCtx_refCDict(context.get(), &compressionDictionary()));
  return context;
}

DecompressionContext makeDecompressionContext() {
  DecompressionContext context{ZSTD_createDCtx(), &ZSTD_freeDCtx};
  if (!context) {
    throw CompressionUnavailableException();
  }
  requireSuccess(ZSTD_DCtx_refDDict(context.get(), &decompressionDictionary()));
  return context;
}

}  // namespace

std::optional<std::vector<std::byte>> compressBody(
    std::span<const std::byte> body) {
  thread_local const CompressionContext context = makeCompressionContext();
  std::vector<std::byte> frame(ZSTD_compressBound(body.size()));
  const std::size_t size = ZSTD_compress2(
      context.get(), frame.data(), frame.size(), body.data(), body.size());
  if (ZSTD_isError(size) != 0) {
    return std::nullopt;
  }
  frame.resize(size);
  return frame;
}

std::optional<std::vector<std::byte>> decompressBody(
    std::span<const std::byte> coded, std::size_t maximumSize) {
  thread_local const DecompressionContext context = makeDecompressionContext();
  if (ZSTD_findFrameCompressedSize(coded.data(), coded.size()) !=
      coded.size()) {
    return std::nullopt;
  }
  std::vector<std::byte> body(maximumSize);
  const std::size_t size = ZSTD_decompressDCtx(
      context.get(), body.data(), body.size(), coded.data(), coded.size());
  if (ZSTD_isError(size) != 0) {
    return std::nullopt;
  }
  body.resize(size);
  return body;
}

}  // namespace rtype::network

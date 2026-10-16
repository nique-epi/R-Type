#include "SipHash.hpp"
#include <bit>
#include <cstddef>
#include <cstdint>
#include <span>
#include "HandshakeConstants.hpp"
#include "SerializationConstants.hpp"

namespace rtype::network {

namespace {

struct SipHashState {
  std::uint64_t v0;
  std::uint64_t v1;
  std::uint64_t v2;
  std::uint64_t v3;
};

void sipRound(SipHashState& state) {
  state.v0 += state.v1;
  state.v1 = std::rotl(state.v1, SIP_ROUND_FIRST_V1_ROTATION);
  state.v1 ^= state.v0;
  state.v0 = std::rotl(state.v0, SIP_ROUND_HALF_ROTATION);
  state.v2 += state.v3;
  state.v3 = std::rotl(state.v3, SIP_ROUND_FIRST_V3_ROTATION);
  state.v3 ^= state.v2;
  state.v0 += state.v3;
  state.v3 = std::rotl(state.v3, SIP_ROUND_SECOND_V3_ROTATION);
  state.v3 ^= state.v0;
  state.v2 += state.v1;
  state.v1 = std::rotl(state.v1, SIP_ROUND_SECOND_V1_ROTATION);
  state.v1 ^= state.v2;
  state.v2 = std::rotl(state.v2, SIP_ROUND_HALF_ROTATION);
}

void runRounds(SipHashState& state, std::size_t roundCount) {
  for (std::size_t round = 0; round < roundCount; ++round) {
    sipRound(state);
  }
}

/** @returns The up to eight bytes of @p bytes as a little-endian integer. */
std::uint64_t littleEndianWord(std::span<const std::byte> bytes) {
  std::uint64_t word = 0;
  for (std::size_t i = 0; i < bytes.size(); ++i) {
    word |= std::to_integer<std::uint64_t>(bytes[i]) << (i * BITS_PER_BYTE);
  }
  return word;
}

void compress(SipHashState& state, std::uint64_t block) {
  state.v3 ^= block;
  runRounds(state, SIPHASH_COMPRESSION_ROUNDS);
  state.v0 ^= block;
}

}  // namespace

std::uint64_t sipHash(const SipHashKey& key,
                      std::span<const std::byte> message) {
  const std::span<const std::byte> keyBytes{key};
  const std::uint64_t firstKeyWord =
      littleEndianWord(keyBytes.first(UINT64_WIRE_SIZE));
  const std::uint64_t secondKeyWord =
      littleEndianWord(keyBytes.last(UINT64_WIRE_SIZE));
  SipHashState state{.v0 = SIPHASH_INITIAL_V0 ^ firstKeyWord,
                     .v1 = SIPHASH_INITIAL_V1 ^ secondKeyWord,
                     .v2 = SIPHASH_INITIAL_V2 ^ firstKeyWord,
                     .v3 = SIPHASH_INITIAL_V3 ^ secondKeyWord};

  const std::size_t wholeBlockBytes =
      message.size() - (message.size() % UINT64_WIRE_SIZE);
  for (std::size_t offset = 0; offset < wholeBlockBytes;
       offset += UINT64_WIRE_SIZE) {
    compress(state,
             littleEndianWord(message.subspan(offset, UINT64_WIRE_SIZE)));
  }
  const std::uint64_t lengthByte =
      static_cast<std::uint64_t>(message.size()) & BYTE_MASK;
  compress(state, littleEndianWord(message.subspan(wholeBlockBytes)) |
                      (lengthByte << SIPHASH_LENGTH_SHIFT));

  state.v2 ^= SIPHASH_FINALIZATION_MARK;
  runRounds(state, SIPHASH_FINALIZATION_ROUNDS);
  return state.v0 ^ state.v1 ^ state.v2 ^ state.v3;
}

}  // namespace rtype::network

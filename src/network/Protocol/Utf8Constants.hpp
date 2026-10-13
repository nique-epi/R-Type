#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

namespace rtype::network {

/**
 * @brief The bytes from minimum to maximum, both included.
 */
struct ByteRange {
  std::uint8_t minimum;
  std::uint8_t maximum;
};

/**
 * @brief One row of the well-formed UTF-8 sequences: the bytes the sequence
 * may start with, how many bytes it takes, and the range of its second byte.
 */
struct Utf8SequenceRule {
  ByteRange lead;
  std::size_t length;
  ByteRange second;
};

/**
 * @brief The range of every byte of a sequence after its second one.
 */
constexpr ByteRange CONTINUATION_BYTES{.minimum = 0x80, .maximum = 0xBF};

/**
 * @brief The well-formed UTF-8 sequences, as RFC 3629 lists them in its
 * section 4. A text matching none of them holds an overlong form, a surrogate,
 * a code point above U+10FFFF, a stray continuation byte or a cut sequence.
 */
constexpr auto UTF8_SEQUENCE_RULES = std::to_array<Utf8SequenceRule>({
    {.lead = {.minimum = 0x00, .maximum = 0x7F},
     .length = 1,
     .second = CONTINUATION_BYTES},
    {.lead = {.minimum = 0xC2, .maximum = 0xDF},
     .length = 2,
     .second = CONTINUATION_BYTES},
    {.lead = {.minimum = 0xE0, .maximum = 0xE0},
     .length = 3,
     .second = {.minimum = 0xA0, .maximum = 0xBF}},
    {.lead = {.minimum = 0xE1, .maximum = 0xEC},
     .length = 3,
     .second = CONTINUATION_BYTES},
    {.lead = {.minimum = 0xED, .maximum = 0xED},
     .length = 3,
     .second = {.minimum = 0x80, .maximum = 0x9F}},
    {.lead = {.minimum = 0xEE, .maximum = 0xEF},
     .length = 3,
     .second = CONTINUATION_BYTES},
    {.lead = {.minimum = 0xF0, .maximum = 0xF0},
     .length = 4,
     .second = {.minimum = 0x90, .maximum = 0xBF}},
    {.lead = {.minimum = 0xF1, .maximum = 0xF3},
     .length = 4,
     .second = CONTINUATION_BYTES},
    {.lead = {.minimum = 0xF4, .maximum = 0xF4},
     .length = 4,
     .second = {.minimum = 0x80, .maximum = 0x8F}},
});

}  // namespace rtype::network

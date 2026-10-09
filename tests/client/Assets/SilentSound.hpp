#pragma once

#include <cstdint>
#include <filesystem>

/** @brief Writes a mono WAV file of silence holding the given number of
 * samples, creating its folders. */
void writeSilentSound(const std::filesystem::path& file,
                      std::uint64_t sampleCount);

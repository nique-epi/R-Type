#include "SilentSound.hpp"
#include <gtest/gtest.h>
#include <SFML/Audio/SoundBuffer.hpp>
#include <SFML/Audio/SoundChannel.hpp>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <vector>

namespace {

constexpr unsigned int MONO_CHANNEL_COUNT = 1;
constexpr unsigned int SAMPLE_RATE = 44100;

}  // namespace

void writeSilentSound(const std::filesystem::path& file,
                      std::uint64_t sampleCount) {
  std::filesystem::create_directories(file.parent_path());
  const std::vector<std::int16_t> samples(static_cast<std::size_t>(sampleCount),
                                          0);
  const sf::SoundBuffer sound(samples.data(), samples.size(),
                              MONO_CHANNEL_COUNT, SAMPLE_RATE,
                              {sf::SoundChannel::Mono});
  ASSERT_TRUE(sound.saveToFile(file));
}

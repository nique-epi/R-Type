#pragma once

#include <filesystem>
#include <functional>
#include <map>
#include <string>
#include <string_view>
#include <vector>
#include "WavePlan.hpp"

namespace rtype::server {

/**
 * @brief The waves of each difficulty, read by the server from its waves file.
 * Only the server reads it: a client only sees the Bydos it is sent.
 *
 * The file is TOML: a table `difficulty` holding one table per difficulty,
 * named after it, each with its enemies, a list of enemy names, and the six
 * integers of a WaveCurve, durations in milliseconds (keys, names and ranges in
 * WaveFileConstants.hpp). Every key is required and any other key is refused.
 * Every error names the file, then the line and the column of what is wrong,
 * and the dotted name of the key.
 */
class WaveCatalog {
 public:
  /**
   * @brief Reads the difficulties from @p text, the content of a waves file.
   * @param source Names the file in the errors.
   * @throws WaveFileSyntaxException when @p text is not TOML.
   * @throws MissingWaveFileKeyException, UnknownWaveFileKeyException,
   * WaveFileTableExpectedException, WaveFileValueOutOfRangeException,
   * InvalidEnemyListException or UnknownEnemyException when it does not
   * describe the difficulties as above.
   */
  [[nodiscard]] static WaveCatalog fromText(std::string_view text,
                                            std::string_view source);

  /**
   * @brief Reads the difficulties from the waves file at @p file, which the
   * errors name by its absolute path.
   * @throws UnreadableWaveFileException when the file cannot be opened, and
   * what fromText() throws.
   */
  [[nodiscard]] static WaveCatalog fromFile(const std::filesystem::path& file);

  /**
   * @returns The waves of @p difficulty.
   * @throws UnknownDifficultyException when the file describes no such
   * difficulty; the message lists those it describes.
   */
  [[nodiscard]] const WavePlan& planOf(std::string_view difficulty) const;

  /** @returns The names of the difficulties, in alphabetical order. */
  [[nodiscard]] std::vector<std::string> difficulties() const;

 private:
  explicit WaveCatalog(std::map<std::string, WavePlan, std::less<>> plans);

  std::map<std::string, WavePlan, std::less<>> plans_;
};

}  // namespace rtype::server

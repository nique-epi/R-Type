#pragma once

#include <cstdint>
#include <filesystem>
#include <functional>
#include <map>
#include <span>
#include <string>
#include <string_view>
#include <vector>
#include "EntitySheets.hpp"
#include "WaveFileText.hpp"
#include "WavePlan.hpp"

namespace rtype::server {

/**
 * @brief The waves of each difficulty, read by the server from its waves file.
 * Only the server reads it: a client only sees the Bydos it is sent.
 *
 * The file is TOML: a table `difficulty` holding one table per difficulty,
 * named after it, each with its enemies, a list of the names of entity sheets
 * of the side enemy, and the six
 * integers of a WaveCurve, durations in milliseconds (keys, names and ranges in
 * WaveFileConstants.hpp). Every key is required and any other key is refused.
 * Every error names the file, then the line and the column of what is wrong,
 * and the dotted name of the key.
 *
 * Several waves files are laid one over the other, in the order of the
 * content folders that hold them: a key a later file gives replaces the same
 * key of the difficulty in the earlier ones, a key it leaves out keeps their
 * value, and a difficulty only a later file names is added. A list, such as
 * `enemies`, is replaced whole. The difficulties are then read as one file
 * is, each value at the line of the file it comes from.
 */
class WaveCatalog {
 public:
  /**
   * @brief Reads the difficulties from @p text, the content of a waves file,
   * whose enemies name sheets of @p types of the side enemy.
   * @param source Names the file in the errors.
   * @throws engine::DataFileSyntaxException when @p text is not TOML.
   * @throws engine::MissingDataFileKeyException,
   * engine::UnknownDataFileKeyException,
   * engine::DataFileTableExpectedException,
   * engine::DataFileValueOutOfRangeException, InvalidEnemyListException,
   * UnknownEnemyException, InvalidLookItemListException or
   * UnknownLookItemException when it does not describe the difficulties as
   * above.
   */
  [[nodiscard]] static WaveCatalog fromText(std::string_view text,
                                            std::string_view source,
                                            const game::EntitySheets& types);

  /**
   * @brief Reads the difficulties of the waves files @p files, laid in their
   * order, as fromText() reads one; @p files is never empty.
   * @throws What fromText() throws, about the file at fault, and
   * TooManyDifficultiesException when the files together describe more than
   * network::MAXIMUM_DIFFICULTIES.
   */
  [[nodiscard]] static WaveCatalog fromTexts(
      std::span<const WaveFileText> files, const game::EntitySheets& types);

  /**
   * @brief Reads the difficulties of the waves files at @p files, laid in
   * their order, which the errors name by their absolute path, against the
   * sheets @p types; @p files is never empty.
   * @throws engine::UnreadableDataFileException when a file cannot be
   * opened, and what fromTexts() throws.
   */
  [[nodiscard]] static WaveCatalog fromFiles(
      std::span<const std::filesystem::path> files,
      const game::EntitySheets& types);

  /**
   * @returns The waves of @p difficulty.
   * @throws UnknownDifficultyException when the file describes no such
   * difficulty; the message lists those it describes.
   */
  [[nodiscard]] const WavePlan& planOf(std::string_view difficulty) const;

  /** @returns The names of the difficulties, in alphabetical order. */
  [[nodiscard]] std::vector<std::string> difficulties() const;

  /**
   * @returns SipHash-2-4 of the text the catalog was read from, under
   * WAVES_FINGERPRINT_KEY; of the texts of its files joined by
   * WAVE_FILE_SEPARATOR when it was read from several. A text has the same
   * fingerprint on every platform, and any change of it, a comment or a line
   * ending included, or a file laid over it, gives another one, unless it hits
   * a collision: one chance in 2^64 for a change made by chance.
   */
  [[nodiscard]] std::uint64_t fingerprint() const;

 private:
  WaveCatalog(std::map<std::string, WavePlan, std::less<>> plans,
              std::uint64_t fingerprint);

  std::map<std::string, WavePlan, std::less<>> plans_;
  std::uint64_t fingerprint_{};
};

}  // namespace rtype::server

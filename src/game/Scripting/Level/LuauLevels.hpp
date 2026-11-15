#pragma once

#include <functional>
#include <map>
#include <string>
#include <string_view>
#include <vector>
#include "LuauLevelEntry.hpp"
#include "LuauLevelStep.hpp"
#include "ScriptMachine.hpp"

namespace rtype::engine {

class ComponentRegistry;

}  // namespace rtype::engine

namespace rtype::game {

struct LevelProgress;

/**
 * @brief The levels the scripts of a machine register, and the table `level`
 * they are written and directed through.
 *
 * `level.register(name, stages, options)` keeps a level: its stages are
 * functions run one after the other, each in a thread a LuauLevelRun resumes,
 * and each one after the first starts at a checkpoint; the options may name
 * the LDtk file that places its enemies (`ldtk`). Inside a stage,
 * `level.wait(ms)` and `level.wait_scroll(position)` stop it until the time
 * passed or the level scrolled that far, and `level.spawn_placed()` asks that
 * the placed enemies appear as the scroll reaches them. During a game that
 * plays a level, any script may read `level.scroll()`, `level.enemies()` and
 * `level.placements()`, stop and restart the scroll with
 * `level.pause_scroll()` and `level.resume_scroll()`, and name the biome the
 * players see with `level.biome(name)`.
 *
 * Neither copied nor moved: the machine reaches it by its address.
 */
class LuauLevels {
 public:
  /** @brief Exposes the table `level` on @p machine, which must not have run
   * a script yet and must outlive this object. */
  explicit LuauLevels(engine::ScriptMachine& machine);
  LuauLevels(const LuauLevels&) = delete;
  LuauLevels& operator=(const LuauLevels&) = delete;
  LuauLevels(LuauLevels&&) = delete;
  LuauLevels& operator=(LuauLevels&&) = delete;
  ~LuauLevels() = default;

  /** @brief Keeps the level @p name as @p entry; a name already kept keeps
   * its first entry. */
  void declare(std::string name, LuauLevelEntry entry);

  /** @returns What the level @p name registered; nullptr when no script
   * registered it, never for a name of names(). */
  [[nodiscard]] const LuauLevelEntry* entryOf(std::string_view name) const;

  /** @returns The entry of the level @p name, to give it the enemies its
   * LDtk file places; nullptr when no script registered it. */
  [[nodiscard]] LuauLevelEntry* entryOf(std::string_view name);

  /** @returns The names of the levels registered, in alphabetical order. */
  [[nodiscard]] std::vector<std::string> names() const;

  /** @brief Makes @p progress the level the scripts read and direct, in the
   * world of @p components, @p entry being what they registered as that
   * level; all three must outlive the game. */
  void attach(LevelProgress& progress, engine::ComponentRegistry& components,
              const LuauLevelEntry& entry);

  /** @brief Forgets the level of @p progress, unless another took its
   * place. */
  void detach(const LevelProgress& progress) noexcept;

  /** @returns What the functions of `level` know of the game and of the step
   * in progress. */
  [[nodiscard]] LuauLevelStep& step();

 private:
  std::map<std::string, LuauLevelEntry, std::less<>> levels_;
  LuauLevelStep step_;
};

}  // namespace rtype::game

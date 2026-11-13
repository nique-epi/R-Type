#pragma once

#include <functional>
#include <map>
#include <string>
#include <string_view>
#include <vector>
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
 * `level.register(name, stages)` keeps a level: its stages are functions run
 * one after the other, each in a thread a LuauLevelRun resumes, and each one
 * after the first starts at a checkpoint. Inside a stage, `level.wait(ms)`
 * and `level.wait_scroll(position)` stop it until the time passed or the
 * level scrolled that far. During a game that plays a level, any script may
 * read `level.scroll()` and `level.enemies()`, stop and restart the scroll
 * with `level.pause_scroll()` and `level.resume_scroll()`, and name the biome
 * the players see with `level.biome(name)`.
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

  /** @brief Keeps the level @p name, whose stages are the functions of
   * @p stages in the registry of the machine; a name already kept keeps its
   * first stages. */
  void declare(std::string name, std::vector<int> stages);

  /** @returns The stages of the level @p name; nullptr when no script
   * registered it. */
  [[nodiscard]] const std::vector<int>* stagesOf(std::string_view name) const;

  /** @returns The names of the levels registered, in alphabetical order. */
  [[nodiscard]] std::vector<std::string> names() const;

  /** @brief Makes @p progress the level the scripts read and direct, in the
   * world of @p components; both must outlive the game. */
  void attach(LevelProgress& progress, engine::ComponentRegistry& components);

  /** @brief Forgets the level of @p progress, unless another took its
   * place. */
  void detach(const LevelProgress& progress) noexcept;

  /** @returns What the functions of `level` know of the game and of the step
   * in progress. */
  [[nodiscard]] LuauLevelStep& step();

 private:
  std::map<std::string, std::vector<int>, std::less<>> levels_;
  LuauLevelStep step_;
};

}  // namespace rtype::game

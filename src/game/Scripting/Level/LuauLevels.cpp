#include "LuauLevels.hpp"
#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <functional>
#include <limits>
#include <map>
#include <optional>
#include <ratio>
#include <string>
#include <string_view>
#include <utility>
#include <variant>
#include <vector>
#include "BiomeGround.hpp"
#include "BiomeName.hpp"
#include "ComponentRegistry.hpp"
#include "ContentName.hpp"
#include "EnemyCount.hpp"
#include "EngineException.hpp"
#include "GameScriptingConstants.hpp"
#include "GroundLookup.hpp"
#include "GroundPlace.hpp"
#include "LdtkField.hpp"
#include "LdtkFieldValue.hpp"
#include "LdtkPlacement.hpp"
#include "LevelCheckpoint.hpp"
#include "LevelConstants.hpp"
#include "LevelProgress.hpp"
#include "LevelTransition.hpp"
#include "LuauLevelConstants.hpp"
#include "LuauLevelEntry.hpp"
#include "LuauLevelStep.hpp"
#include "LuauLevelWait.hpp"
#include "PlayBand.hpp"
#include "PlayfieldConstants.hpp"
#include "ScriptFunction.hpp"
#include "ScriptMachine.hpp"
#include "ScriptWait.hpp"
#include "ShipControlConstants.hpp"
#include "TimeConstants.hpp"
#include "lua.h"
#include "lualib.h"

namespace rtype::game {

namespace {

/** @returns The LuauLevels in the upvalue of the running function. */
LuauLevels& levelsOf(lua_State* state) {
  return *static_cast<LuauLevels*>(lua_touserdata(state, lua_upvalueindex(1)));
}

/** @returns The progress of the level the game plays.
 * @throws engine::ScriptException outside a game that plays a level. */
LevelProgress& progressOf(lua_State* state) {
  LevelProgress* const progress = levelsOf(state).step().progress;
  if (progress == nullptr) {
    throw engine::ScriptException(NO_LEVEL_PLAYED_MESSAGE);
  }
  return *progress;
}

/** @returns The step of the stage whose thread is @p state.
 * @throws engine::ScriptException when @p state is not the thread of the
 * stage that steps. */
LuauLevelStep& stepOfStage(lua_State* state) {
  LuauLevelStep& step = levelsOf(state).step();
  if (step.resumedThread != state) {
    throw engine::ScriptException(LEVEL_WAIT_OUTSIDE_STAGE_MESSAGE);
  }
  return step;
}

/** @returns How many stages the table at LEVEL_STAGES_ARGUMENT lists.
 * @throws engine::ScriptException when it is not a list of 1 to
 * MAXIMUM_LEVEL_STAGES functions. */
int stageCountOf(lua_State* state) {
  luaL_checktype(state, LEVEL_STAGES_ARGUMENT, LUA_TTABLE);
  const int count = lua_objlen(state, LEVEL_STAGES_ARGUMENT);
  if (count < 1 || std::cmp_greater(count, MAXIMUM_LEVEL_STAGES)) {
    throw engine::ScriptException(WRONG_LEVEL_STAGES_MESSAGE);
  }
  for (int index = 1; index <= count; ++index) {
    lua_rawgeti(state, LEVEL_STAGES_ARGUMENT, index);
    const bool isFunction = lua_type(state, -1) == LUA_TFUNCTION;
    lua_pop(state, 1);
    if (!isFunction) {
      throw engine::ScriptException(WRONG_LEVEL_STAGES_MESSAGE);
    }
  }
  return count;
}

/** @returns A reference in the registry of the machine to each of the
 * @p count functions of the table at LEVEL_STAGES_ARGUMENT, in its order. */
std::vector<int> stageReferences(lua_State* state, int count) {
  std::vector<int> stages;
  stages.reserve(static_cast<std::size_t>(count));
  for (int index = 1; index <= count; ++index) {
    lua_rawgeti(state, LEVEL_STAGES_ARGUMENT, index);
    stages.push_back(lua_ref(state, -1));
    lua_pop(state, 1);
  }
  return stages;
}

/** @returns Whether @p file stays inside a content folder: a relative path
 * without `..`. */
bool staysInContent(const std::string& file) {
  const std::filesystem::path path{file};
  if (file.empty() || !path.is_relative()) {
    return false;
  }
  const std::filesystem::path parent{PARENT_FOLDER_PART};
  return std::ranges::none_of(
      path, [&parent](const std::filesystem::path& part) noexcept {
        return part == parent;
      });
}

/** @throws engine::ScriptException when the table at LEVEL_OPTIONS_ARGUMENT
 * holds a key no option has. */
void refuseUnknownOptions(lua_State* state) {
  lua_pushnil(state);
  while (lua_next(state, LEVEL_OPTIONS_ARGUMENT) != 0) {
    const bool known =
        lua_type(state, -2) == LUA_TSTRING &&
        std::ranges::find(LEVEL_OPTION_KEYS,
                          std::string_view{lua_tostring(state, -2)}) !=
            LEVEL_OPTION_KEYS.end();
    lua_pop(state, 1);
    if (!known) {
      lua_pop(state, 1);
      throw engine::ScriptException(WRONG_LEVEL_OPTIONS_MESSAGE);
    }
  }
}

/** @returns The LDtk file the options at LEVEL_OPTIONS_ARGUMENT name under
 * LEVEL_LDTK_KEY; nothing without options or without that key.
 * @throws engine::ScriptException when the options are no table, hold
 * another key, or name anything but a relative path inside the content. */
std::optional<std::string> ldtkFileOf(lua_State* state) {
  if (lua_isnoneornil(state, LEVEL_OPTIONS_ARGUMENT)) {
    return std::nullopt;
  }
  luaL_checktype(state, LEVEL_OPTIONS_ARGUMENT, LUA_TTABLE);
  refuseUnknownOptions(state);
  lua_rawgetfield(state, LEVEL_OPTIONS_ARGUMENT, LEVEL_LDTK_KEY);
  if (lua_isnil(state, -1)) {
    lua_pop(state, 1);
    return std::nullopt;
  }
  const bool isText = lua_type(state, -1) == LUA_TSTRING;
  std::string file = isText ? lua_tostring(state, -1) : std::string{};
  lua_pop(state, 1);
  if (!isText || !staysInContent(file)) {
    throw engine::ScriptException(WRONG_LDTK_FILE_MESSAGE);
  }
  return file;
}

/** @returns The number at @p key of the table on top of the stack, or
 * @p fallback without one.
 * @throws engine::ScriptException unless it is a finite number from 0. */
double startNumberOf(lua_State* state, const char* key, double fallback) {
  lua_rawgetfield(state, -1, key);
  const int type = lua_type(state, -1);
  const double value = type == LUA_TNUMBER ? lua_tonumber(state, -1) : fallback;
  lua_pop(state, 1);
  if ((type != LUA_TNUMBER && type != LUA_TNIL) || !std::isfinite(value) ||
      value < 0.0) {
    throw engine::ScriptException(WRONG_LEVEL_STARTS_MESSAGE);
  }
  return value;
}

/** @returns The biome at START_BIOME_KEY of the table on top of the stack,
 * empty without one.
 * @throws engine::ScriptException unless it is a biome name. */
std::string startBiomeOf(lua_State* state) {
  lua_rawgetfield(state, -1, START_BIOME_KEY);
  const int type = lua_type(state, -1);
  std::string biome = type == LUA_TSTRING ? lua_tostring(state, -1) : "";
  lua_pop(state, 1);
  if (type != LUA_TNIL && !identifiers::isBiomeName(biome)) {
    throw engine::ScriptException(WRONG_LEVEL_STARTS_MESSAGE);
  }
  return biome;
}

/** @returns Where stage @p stage, counted from 0, starts, as the table on top
 * of the stack declares it: its scroll, its biome, and the scroll that biome
 * began at, its own scroll when it gives none. */
LevelCheckpoint startDeclared(lua_State* state, std::size_t stage) {
  if (lua_type(state, -1) != LUA_TTABLE) {
    throw engine::ScriptException(WRONG_LEVEL_STARTS_MESSAGE);
  }
  const double scroll = startNumberOf(state, START_SCROLL_KEY,
                                      std::numeric_limits<double>::quiet_NaN());
  const double biomeSince = startNumberOf(state, START_BIOME_SINCE_KEY, scroll);
  return LevelCheckpoint{
      .stage = stage,
      .travelled = std::chrono::duration_cast<engine::Duration>(
          std::chrono::duration<double>(scroll / LEVEL_SCROLL_SPEED)),
      .biome = startBiomeOf(state),
      .biomeSince = static_cast<float>(biomeSince)};
}

/** @returns The stage, counted from 0, the key on top but one of the stack
 * names, from 1.
 * @throws engine::ScriptException unless it is a whole number from 2 to
 * @p count. */
std::size_t startedStageOf(lua_State* state, int count) {
  const double number =
      lua_type(state, -2) == LUA_TNUMBER ? lua_tonumber(state, -2) : 0.0;
  if (number < FIRST_STARTED_STAGE_NUMBER || number > count ||
      number != std::floor(number)) {
    throw engine::ScriptException(WRONG_LEVEL_STARTS_MESSAGE);
  }
  return static_cast<std::size_t>(number) - 1;
}

/** @returns The starts the options at LEVEL_OPTIONS_ARGUMENT declare under
 * LEVEL_STARTS_KEY for a level of @p count stages; none without options or
 * without that key.
 * @throws engine::ScriptException when they are no table, or a start is
 * wrong. */
std::map<std::size_t, LevelCheckpoint> startsOf(lua_State* state, int count) {
  std::map<std::size_t, LevelCheckpoint> starts;
  if (lua_isnoneornil(state, LEVEL_OPTIONS_ARGUMENT)) {
    return starts;
  }
  lua_rawgetfield(state, LEVEL_OPTIONS_ARGUMENT, LEVEL_STARTS_KEY);
  if (lua_isnil(state, -1)) {
    lua_pop(state, 1);
    return starts;
  }
  if (lua_type(state, -1) != LUA_TTABLE) {
    throw engine::ScriptException(WRONG_LEVEL_STARTS_MESSAGE);
  }
  lua_pushnil(state);
  while (lua_next(state, -2) != 0) {
    const std::size_t stage = startedStageOf(state, count);
    starts.emplace(stage, startDeclared(state, stage));
    lua_pop(state, 1);
  }
  lua_pop(state, 1);
  return starts;
}

/** @returns The row at @p key of the table on top of the stack.
 * @throws engine::ScriptException unless it is a finite number from 0 to
 * PLAY_AREA_HEIGHT. */
float bandRowOf(lua_State* state, const char* key) {
  lua_rawgetfield(state, -1, key);
  const double row = lua_type(state, -1) == LUA_TNUMBER
                         ? lua_tonumber(state, -1)
                         : std::numeric_limits<double>::quiet_NaN();
  lua_pop(state, 1);
  if (!std::isfinite(row) || row < 0.0 || row > PLAY_AREA_HEIGHT) {
    throw engine::ScriptException(WRONG_LEVEL_BANDS_MESSAGE);
  }
  return static_cast<float>(row);
}

/** @returns The band the table on top of the stack declares.
 * @throws engine::ScriptException unless it is a table whose bottom lies a
 * ship height or more below its top. */
PlayBand bandDeclared(lua_State* state) {
  if (lua_type(state, -1) != LUA_TTABLE) {
    throw engine::ScriptException(WRONG_LEVEL_BANDS_MESSAGE);
  }
  const PlayBand band{.top = bandRowOf(state, BAND_TOP_KEY),
                      .bottom = bandRowOf(state, BAND_BOTTOM_KEY)};
  if (band.bottom - band.top < SHIP_HEIGHT) {
    throw engine::ScriptException(WRONG_LEVEL_BANDS_MESSAGE);
  }
  return band;
}

/** @returns The bands the options at LEVEL_OPTIONS_ARGUMENT declare under
 * LEVEL_BANDS_KEY, by biome name; none without options or without that key.
 * @throws engine::ScriptException when they are no table, a key is no biome
 * name, or a band is wrong. */
std::map<std::string, PlayBand, std::less<>> bandsOf(lua_State* state) {
  std::map<std::string, PlayBand, std::less<>> bands;
  if (lua_isnoneornil(state, LEVEL_OPTIONS_ARGUMENT)) {
    return bands;
  }
  lua_rawgetfield(state, LEVEL_OPTIONS_ARGUMENT, LEVEL_BANDS_KEY);
  if (lua_isnil(state, -1)) {
    lua_pop(state, 1);
    return bands;
  }
  if (lua_type(state, -1) != LUA_TTABLE) {
    throw engine::ScriptException(WRONG_LEVEL_BANDS_MESSAGE);
  }
  lua_pushnil(state);
  while (lua_next(state, -2) != 0) {
    const bool named = lua_type(state, -2) == LUA_TSTRING &&
                       identifiers::isBiomeName(lua_tostring(state, -2));
    if (!named) {
      throw engine::ScriptException(WRONG_LEVEL_BANDS_MESSAGE);
    }
    bands.emplace(lua_tostring(state, -2), bandDeclared(state));
    lua_pop(state, 1);
  }
  lua_pop(state, 1);
  return bands;
}

/** @returns Whether @p name is a name a map may show: see LEVEL_MAP_KEY. */
bool isMapName(std::string_view name) {
  return !name.empty() && name.size() <= MAXIMUM_MAP_NAME_SIZE &&
         name.find_first_not_of(MAP_NAME_CHARACTERS) == std::string_view::npos;
}

/** @returns The text the options at LEVEL_OPTIONS_ARGUMENT give under
 * @p key, empty for a value that is no text; nothing without options or
 * without that key. */
std::optional<std::string> textOptionOf(lua_State* state, const char* key) {
  if (lua_isnoneornil(state, LEVEL_OPTIONS_ARGUMENT)) {
    return std::nullopt;
  }
  lua_rawgetfield(state, LEVEL_OPTIONS_ARGUMENT, key);
  const int type = lua_type(state, -1);
  std::string text = type == LUA_TSTRING ? lua_tostring(state, -1) : "";
  lua_pop(state, 1);
  if (type == LUA_TNIL) {
    return std::nullopt;
  }
  return text;
}

/** @returns The name the options give under LEVEL_MAP_KEY, nothing without it.
 * @throws engine::ScriptException unless it is a text isMapName() accepts. */
std::optional<std::string> mapNameOf(lua_State* state) {
  std::optional<std::string> name = textOptionOf(state, LEVEL_MAP_KEY);
  if (name.has_value() && !isMapName(*name)) {
    throw engine::ScriptException(WRONG_LEVEL_MAP_MESSAGE);
  }
  return name;
}

/** @returns The biome the options open the level in, under
 * LEVEL_OPENING_BIOME_KEY; nothing without it.
 * @throws engine::ScriptException unless it is a biome name. */
std::optional<std::string> openingBiomeOf(lua_State* state) {
  std::optional<std::string> biome =
      textOptionOf(state, LEVEL_OPENING_BIOME_KEY);
  if (biome.has_value() && !identifiers::isBiomeName(*biome)) {
    throw engine::ScriptException(WRONG_BIOME_NAME_MESSAGE);
  }
  return biome;
}

/** @returns The preview the options give under LEVEL_MAP_PREVIEW_KEY, nothing
 * without it.
 * @throws engine::ScriptException unless it is a content name, given with a
 * map. */
std::optional<std::string> mapPreviewOf(lua_State* state,
                                        const std::optional<std::string>& map) {
  std::optional<std::string> preview =
      textOptionOf(state, LEVEL_MAP_PREVIEW_KEY);
  if (preview.has_value() &&
      (!map.has_value() || !identifiers::isContentName(*preview))) {
    throw engine::ScriptException(WRONG_LEVEL_PREVIEW_MESSAGE);
  }
  return preview;
}

/** @brief `level.register(name, stages, options)`: an exception that leaves
 * it is an error of the script, as Luau makes of any exception a C function
 * throws. */
int registerLevel(lua_State* state) {
  LuauLevels& levels = levelsOf(state);
  std::string name = luaL_checkstring(state, LEVEL_NAME_ARGUMENT);
  if (name.empty()) {
    throw engine::ScriptException(EMPTY_LEVEL_NAME_MESSAGE);
  }
  if (levels.entryOf(name) != nullptr) {
    throw engine::ScriptException(DUPLICATE_LEVEL_MESSAGE);
  }
  const int count = stageCountOf(state);
  LuauLevelEntry entry{.stages = stageReferences(state, count),
                       .ldtkFile = ldtkFileOf(state),
                       .placements = {},
                       .starts = startsOf(state, count),
                       .bands = bandsOf(state),
                       .mapName = mapNameOf(state),
                       .openingBiome = openingBiomeOf(state),
                       .mapPreview = std::nullopt,
                       .grounds = {}};
  entry.mapPreview = mapPreviewOf(state, entry.mapName);
  levels.declare(std::move(name), std::move(entry));
  return 0;
}

/** @brief `level.wait(ms)`: stops the stage that steps for that time. */
int waitOfStage(lua_State* state) {
  LuauLevelStep& step = stepOfStage(state);
  const std::optional<std::uint32_t> ticks =
      engine::scriptWaitTicks(luaL_checknumber(state, LEVEL_WAIT_ARGUMENT));
  if (!ticks.has_value()) {
    throw engine::ScriptException(WRONG_LEVEL_WAIT_MESSAGE);
  }
  step.wait = LuauLevelWait{.ticks = *ticks};
  return lua_yield(state, 0);
}

/** @returns The ticks of a transition of @p milliseconds; nothing unless it
 * lasts SHORTEST_LEVEL_TRANSITION to LONGEST_LEVEL_TRANSITION. */
std::optional<std::uint32_t> transitionTicksOf(double milliseconds) {
  const double shortest =
      std::chrono::duration<double, std::milli>(SHORTEST_LEVEL_TRANSITION)
          .count();
  const double longest =
      std::chrono::duration<double, std::milli>(LONGEST_LEVEL_TRANSITION)
          .count();
  if (!std::isfinite(milliseconds) || milliseconds < shortest ||
      milliseconds > longest) {
    return std::nullopt;
  }
  return engine::scriptWaitTicks(milliseconds);
}

/** @brief `level.transition(ms)`: pauses the scroll, plays a transition of
 * that time, and stops the stage that steps until it ends. */
int playTransition(lua_State* state) {
  LuauLevelStep& step = stepOfStage(state);
  const std::optional<std::uint32_t> ticks =
      transitionTicksOf(luaL_checknumber(state, LEVEL_WAIT_ARGUMENT));
  if (!ticks.has_value()) {
    throw engine::ScriptException(WRONG_LEVEL_TRANSITION_MESSAGE);
  }
  LevelProgress& progress = progressOf(state);
  progress.scroll.pause();
  progress.transition = LevelTransition{.ticksLeft = *ticks};
  step.wait = LuauLevelWait{.ticks = *ticks};
  return lua_yield(state, 0);
}

/** @brief `level.wait_scroll(position)`: stops the stage that steps until
 * the level scrolled that far. */
int waitForScroll(lua_State* state) {
  LuauLevelStep& step = stepOfStage(state);
  const double position = luaL_checknumber(state, LEVEL_WAIT_ARGUMENT);
  if (!std::isfinite(position) || position < 0.0) {
    throw engine::ScriptException(WRONG_SCROLL_POSITION_MESSAGE);
  }
  step.wait =
      LuauLevelWait{.untilScrollPosition = static_cast<float>(position)};
  return lua_yield(state, 0);
}

/** @brief `level.scroll()`: how far the level scrolled, in logical units. */
int scrollPosition(lua_State* state) {
  lua_pushnumber(state, double{progressOf(state).scroll.position()});
  return 1;
}

/** @brief `level.pause_scroll()`: stops the scroll where it is. */
int pauseScroll(lua_State* state) {
  progressOf(state).scroll.pause();
  return 0;
}

/** @brief `level.resume_scroll()`: lets the scroll move on again. */
int resumeScroll(lua_State* state) {
  progressOf(state).scroll.resume();
  return 0;
}

/** @brief `level.biome(name)`: names the biome the players see from now; a
 * new name begins where the scroll stands, the same name changes nothing. */
int showBiome(lua_State* state) {
  LevelProgress& progress = progressOf(state);
  const std::string_view name = luaL_checkstring(state, LEVEL_BIOME_ARGUMENT);
  if (!identifiers::isBiomeName(name)) {
    throw engine::ScriptException(WRONG_BIOME_NAME_MESSAGE);
  }
  if (progress.biome != name) {
    progress.biome = name;
    progress.biomeSince = progress.scroll.position();
  }
  return 0;
}

/** @brief `level.enemies()`: how many enemies the world holds. */
int enemyCount(lua_State* state) {
  engine::ComponentRegistry* const components =
      levelsOf(state).step().components;
  if (components == nullptr) {
    throw engine::ScriptException(NO_LEVEL_PLAYED_MESSAGE);
  }
  lua_pushnumber(state, static_cast<double>(enemiesIn(*components)));
  return 1;
}

/** @brief `level.bosses()`: how many bosses the world holds, from their
 * warning to their death. */
int bossCount(lua_State* state) {
  engine::ComponentRegistry* const components =
      levelsOf(state).step().components;
  if (components == nullptr) {
    throw engine::ScriptException(NO_LEVEL_PLAYED_MESSAGE);
  }
  lua_pushnumber(state, static_cast<double>(bossesIn(*components)));
  return 1;
}

/** @brief Sets the field @p key of the table on top of the stack to
 * @p value. */
void setField(lua_State* state, const char* key, const LdtkFieldValue& value) {
  if (const auto* const flag = std::get_if<bool>(&value)) {
    lua_pushboolean(state, static_cast<int>(*flag));
  } else if (const auto* const number = std::get_if<double>(&value)) {
    lua_pushnumber(state, *number);
  } else {
    const auto& text = std::get<std::string>(value);
    lua_pushlstring(state, text.data(), text.size());
  }
  lua_setfield(state, -2, key);
}

/** @brief Pushes the table of @p placement: its entity, its sheet, the x and
 * y of its center, its width, and the table of its fields. */
void pushPlacement(lua_State* state, const LdtkPlacement& placement) {
  lua_createtable(state, 0, PLACEMENT_KEYS);
  setField(state, PLACEMENT_ENTITY_KEY, LdtkFieldValue{placement.entity});
  setField(state, PLACEMENT_SHEET_KEY, LdtkFieldValue{placement.sheet});
  setField(state, POINT_X_KEY, LdtkFieldValue{double{placement.position.x}});
  setField(state, POINT_Y_KEY, LdtkFieldValue{double{placement.position.y}});
  setField(state, PLACEMENT_WIDTH_KEY, LdtkFieldValue{double{placement.width}});
  lua_createtable(state, 0, static_cast<int>(placement.fields.size()));
  for (const LdtkField& field : placement.fields) {
    setField(state, field.name.c_str(), field.value);
  }
  lua_setfield(state, -2, PLACEMENT_FIELDS_KEY);
}

/** @brief `level.placements()`: the enemies the level places, sorted by x. */
int placementsOfLevel(lua_State* state) {
  static_cast<void>(progressOf(state));
  const std::vector<LdtkPlacement>& placements =
      levelsOf(state).step().entry->placements;
  lua_createtable(state, static_cast<int>(placements.size()), 0);
  int index = 1;
  for (const LdtkPlacement& placement : placements) {
    pushPlacement(state, placement);
    lua_rawseti(state, -2, index);
    ++index;
  }
  return 1;
}

/** @brief Pushes the row @p row of the ground, or nil when there is none. */
void pushRow(lua_State* state, std::optional<float> row) {
  if (row.has_value()) {
    lua_pushnumber(state, double{*row});
  } else {
    lua_pushnil(state);
  }
}

/** @returns The ground of the biome the level shows; nullptr when its LDtk
 * project gives none. */
const BiomeGround* shownGround(lua_State* state) {
  const LevelProgress& progress = progressOf(state);
  const LuauLevelEntry& entry = *levelsOf(state).step().entry;
  const auto found = entry.grounds.find(progress.biome);
  return found == entry.grounds.end() ? nullptr : &found->second;
}

/** @returns Where the column the first argument names lies on the ground of
 * the biome the level shows. */
GroundPlace groundPlaceOf(lua_State* state) {
  const LevelProgress& progress = progressOf(state);
  const double column = luaL_checknumber(state, LEVEL_GROUND_ARGUMENT);
  return GroundPlace{
      .x = static_cast<float>(column),
      .scrolled = progress.scroll.position() - progress.biomeSince};
}

/** @brief `level.ground(x)`: the row of the top of the floor under the
 * column x, in logical units; nil where there is none. */
int groundRow(lua_State* state) {
  const BiomeGround* const ground = shownGround(state);
  pushRow(state, ground == nullptr ? std::nullopt
                                   : floorRowAt(*ground, groundPlaceOf(state)));
  return 1;
}

/** @brief `level.ceiling(x)`: the row just under the ceiling over the column
 * x, in logical units; nil where there is none. */
int ceilingRow(lua_State* state) {
  const BiomeGround* const ground = shownGround(state);
  pushRow(state, ground == nullptr
                     ? std::nullopt
                     : ceilingRowAt(*ground, groundPlaceOf(state)));
  return 1;
}

/** @brief `level.spawn_placed()`: from now on, each placed enemy appears as
 * the scroll brings it to the right edge, restarts included. */
int spawnPlaced(lua_State* state) {
  progressOf(state).placedEnemiesAsked = true;
  return 0;
}

}  // namespace

LuauLevels::LuauLevels(engine::ScriptMachine& machine) {
  const std::array functions{
      engine::ScriptFunction{.name = LEVEL_REGISTER_FUNCTION_NAME,
                             .function = registerLevel},
      engine::ScriptFunction{.name = LEVEL_WAIT_FUNCTION_NAME,
                             .function = waitOfStage},
      engine::ScriptFunction{.name = LEVEL_WAIT_SCROLL_FUNCTION_NAME,
                             .function = waitForScroll},
      engine::ScriptFunction{.name = LEVEL_SCROLL_FUNCTION_NAME,
                             .function = scrollPosition},
      engine::ScriptFunction{.name = LEVEL_PAUSE_SCROLL_FUNCTION_NAME,
                             .function = pauseScroll},
      engine::ScriptFunction{.name = LEVEL_RESUME_SCROLL_FUNCTION_NAME,
                             .function = resumeScroll},
      engine::ScriptFunction{.name = LEVEL_BIOME_FUNCTION_NAME,
                             .function = showBiome},
      engine::ScriptFunction{.name = LEVEL_ENEMIES_FUNCTION_NAME,
                             .function = enemyCount},
      engine::ScriptFunction{.name = LEVEL_BOSSES_FUNCTION_NAME,
                             .function = bossCount},
      engine::ScriptFunction{.name = LEVEL_PLACEMENTS_FUNCTION_NAME,
                             .function = placementsOfLevel},
      engine::ScriptFunction{.name = LEVEL_SPAWN_PLACED_FUNCTION_NAME,
                             .function = spawnPlaced},
      engine::ScriptFunction{.name = LEVEL_TRANSITION_FUNCTION_NAME,
                             .function = playTransition},
      engine::ScriptFunction{.name = LEVEL_GROUND_FUNCTION_NAME,
                             .function = groundRow},
      engine::ScriptFunction{.name = LEVEL_CEILING_FUNCTION_NAME,
                             .function = ceilingRow}};
  machine.expose(LEVEL_TABLE, functions, this);
}

void LuauLevels::declare(std::string name, LuauLevelEntry entry) {
  levels_.emplace(std::move(name), std::move(entry));
}

const LuauLevelEntry* LuauLevels::entryOf(std::string_view name) const {
  const auto found = levels_.find(name);
  return found == levels_.end() ? nullptr : &found->second;
}

LuauLevelEntry* LuauLevels::entryOf(std::string_view name) {
  const auto found = levels_.find(name);
  return found == levels_.end() ? nullptr : &found->second;
}

std::vector<std::string> LuauLevels::names() const {
  std::vector<std::string> names;
  names.reserve(levels_.size());
  for (const auto& [name, entry] : levels_) {
    names.push_back(name);
  }
  return names;
}

void LuauLevels::attach(LevelProgress& progress,
                        engine::ComponentRegistry& components,
                        const LuauLevelEntry& entry) {
  step_.progress = &progress;
  step_.components = &components;
  step_.entry = &entry;
}

void LuauLevels::detach(const LevelProgress& progress) noexcept {
  if (step_.progress == &progress) {
    step_.progress = nullptr;
    step_.components = nullptr;
    step_.entry = nullptr;
  }
}

LuauLevelStep& LuauLevels::step() { return step_; }

}  // namespace rtype::game

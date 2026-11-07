#include "ScriptMachine.hpp"
#include <chrono>
#include <cstddef>
#include <cstdlib>
#include <memory>
#include <span>
#include <string>
#include <string_view>
#include "EngineException.hpp"
#include "ScriptBudget.hpp"
#include "ScriptFunction.hpp"
#include "ScriptLimits.hpp"
#include "ScriptSource.hpp"
#include "ScriptingConstants.hpp"
#include "lua.h"
#include "luacode.h"
#include "lualib.h"

namespace rtype::engine {

namespace {

/**
 * @brief The allocator of a machine: it counts what its scripts hold in the
 * ScriptBudget @p userData points to, and refuses a block that would pass its
 * limit, which Luau turns into a memory error of the script. Its parameters
 * are those of lua_Alloc, which Luau imposes.
 */
// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
void* allocate(void* userData, void* block, std::size_t oldSize,
               std::size_t newSize) {
  ScriptBudget& budget = *static_cast<ScriptBudget*>(userData);
  const std::size_t held = block == nullptr ? 0 : oldSize;
  if (newSize == 0) {
    // NOLINTNEXTLINE(cppcoreguidelines-no-malloc,cppcoreguidelines-owning-memory,hicpp-no-malloc)
    std::free(block);
    budget.memoryUsed -= held;
    return nullptr;
  }
  if (budget.memoryUsed - held + newSize > budget.limits.memoryBytes) {
    return nullptr;
  }
  // NOLINTNEXTLINE(cppcoreguidelines-no-malloc,cppcoreguidelines-owning-memory,hicpp-no-malloc)
  void* const grown = std::realloc(block, newSize);
  if (grown != nullptr) {
    budget.memoryUsed = budget.memoryUsed - held + newSize;
  }
  return grown;
}

/** @brief Stops the call in progress once it passed its deadline; Luau calls
 * it at the loops and calls of a script, @p collectionStep negative outside a
 * collection. */
void stopOverBudget(lua_State* state, int collectionStep) {
  if (collectionStep >= 0) {
    return;
  }
  const ScriptBudget& budget =
      *static_cast<const ScriptBudget*>(lua_callbacks(state)->userdata);
  if (budget.callInProgress &&
      std::chrono::steady_clock::now() > budget.deadline) {
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-vararg,hicpp-vararg)
    luaL_errorL(state, "%s", SCRIPT_OVER_BUDGET_MESSAGE);
  }
}

/** @returns A machine whose memory and time @p budget counts.
 * @throws ScriptException when Luau creates none. */
lua_State* newState(ScriptBudget& budget) {
  lua_State* const state = lua_newstate(allocate, &budget);
  if (state == nullptr) {
    throw ScriptException("Luau could not create a script machine");
  }
  lua_callbacks(state)->userdata = &budget;
  lua_callbacks(state)->interrupt = stopOverBudget;
  luaL_openlibs(state);
  for (const std::string_view name : UNSAFE_SCRIPT_GLOBALS) {
    lua_pushnil(state);
    lua_setglobal(state, std::string{name}.c_str());
  }
  return state;
}

/** @returns The error on top of the stack of @p thread, popped. */
std::string errorOf(lua_State* thread) {
  const char* const text = lua_tostring(thread, -1);
  std::string message = text == nullptr ? std::string{} : std::string{text};
  lua_pop(thread, 1);
  return message;
}

/** @brief Compiles @p source and pushes it on @p thread as a function.
 * @throws ScriptException when it does not compile. */
void loadChunk(lua_State* thread, const ScriptSource& source) {
  std::size_t size = 0;
  const std::unique_ptr<char, void (*)(void*)> bytecode{
      luau_compile(source.text.data(), source.text.size(), nullptr, &size),
      std::free};
  const std::string chunkName =
      std::string{SCRIPT_CHUNK_NAME_PREFIX} + source.name;
  if (luau_load(thread, chunkName.c_str(), bytecode.get(), size, 0) != 0) {
    throw ScriptException(errorOf(thread));
  }
}

}  // namespace

ScriptMachine::ScriptMachine(ScriptLimits limits)
    : budget_{.limits = limits,
              .memoryUsed = 0,
              .deadline = {},
              .callInProgress = false},
      state_(newState(budget_), lua_close) {}

ScriptMachine::~ScriptMachine() = default;

void ScriptMachine::expose(std::string_view table,
                           std::span<const ScriptFunction> functions,
                           void* context) {
  if (sealed_) {
    throw ScriptException("functions are exposed before the first script runs");
  }
  lua_State* const state = state_.get();
  lua_createtable(state, 0, static_cast<int>(functions.size()));
  for (const ScriptFunction& function : functions) {
    const std::string& name = exposedNames_.emplace_back(function.name);
    lua_pushlightuserdata(state, context);
    lua_pushcclosurek(state, function.function, name.c_str(), 1, nullptr);
    lua_setfield(state, -2, name.c_str());
  }
  lua_setglobal(state, std::string{table}.c_str());
}

void ScriptMachine::run(const ScriptSource& source) {
  seal();
  lua_State* const state = state_.get();
  lua_State* const thread = lua_newthread(state);
  luaL_sandboxthread(thread);
  try {
    loadChunk(thread, source);
    callOn(thread, 0, 0);
  } catch (const ScriptException&) {
    lua_pop(state, 1);
    throw;
  }
  lua_pop(state, 1);
}

void ScriptMachine::call(int arguments, int results) {
  callOn(state_.get(), arguments, results);
}

lua_State* ScriptMachine::state() const { return state_.get(); }

std::size_t ScriptMachine::memoryUsed() const { return budget_.memoryUsed; }

void ScriptMachine::seal() {
  if (!sealed_) {
    luaL_sandbox(state_.get());
    sealed_ = true;
  }
}

void ScriptMachine::callOn(lua_State* thread, int arguments, int results) {
  budget_.deadline =
      std::chrono::steady_clock::now() + budget_.limits.callBudget;
  budget_.callInProgress = true;
  const int status = lua_pcall(thread, arguments, results, 0);
  budget_.callInProgress = false;
  if (status != 0) {
    throw ScriptException(errorOf(thread));
  }
}

}  // namespace rtype::engine

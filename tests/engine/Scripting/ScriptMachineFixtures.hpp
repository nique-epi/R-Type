#pragma once

#include <array>
#include <string>
#include <string_view>
#include "ScriptFunction.hpp"
#include "ScriptMachine.hpp"
#include "ScriptSource.hpp"

/** @brief What the scripts of a test hand the program through the probe: the
 * last number recorded, and the reference of the last function kept. */
struct ScriptProbe {
  double recorded{0.0};
  int keptFunction{0};
};

/** @returns The functions of the probe: `record(number)` and
 * `keep(function)`, both writing into the ScriptProbe of their upvalue. */
[[nodiscard]] std::array<rtype::engine::ScriptFunction, 2> probeFunctions();

/** @returns A source named SCRIPT_NAME holding @p text. */
[[nodiscard]] rtype::engine::ScriptSource scriptOf(std::string_view text);

/** @returns The message of the ScriptException running @p source throws, or
 * an empty text when it runs. */
[[nodiscard]] std::string errorOfRunning(
    rtype::engine::ScriptMachine& machine,
    const rtype::engine::ScriptSource& source);

/** @returns What the function @p probe kept returns for @p argument. */
[[nodiscard]] double resultOfKeptFunction(rtype::engine::ScriptMachine& machine,
                                          const ScriptProbe& probe,
                                          double argument);

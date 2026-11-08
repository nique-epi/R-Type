#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <string>
#include "EngineException.hpp"
#include "ScriptLimits.hpp"
#include "ScriptMachine.hpp"
#include "ScriptMachineFixtures.hpp"
#include "ScriptMachineTestConstants.hpp"
#include "ScriptSource.hpp"
#include "ScriptingConstants.hpp"

using rtype::engine::SCRIPT_OVER_BUDGET_MESSAGE;
using rtype::engine::ScriptException;
using rtype::engine::ScriptLimits;
using rtype::engine::ScriptMachine;
using ::testing::HasSubstr;

/**
 * Given a machine that exposes the probe
 * When a script calls one of its functions
 * Then the program receives the number the script passed
 */
TEST(ScriptMachine, RunsAScriptThatCallsAnExposedFunction) {
  ScriptMachine machine{ScriptLimits{}};
  ScriptProbe probe;
  machine.expose(PROBE_TABLE, probeFunctions(), &probe);

  machine.run(scriptOf(RECORDS_THE_ANSWER));

  EXPECT_EQ(probe.recorded, THE_ANSWER);
}

/**
 * Given a script that handed the program a function doubling its argument
 * When the program calls that function
 * Then it gets the doubled value back
 */
TEST(ScriptMachine, CallsAFunctionAScriptHandedOver) {
  ScriptMachine machine{ScriptLimits{}};
  ScriptProbe probe;
  machine.expose(PROBE_TABLE, probeFunctions(), &probe);
  machine.run(scriptOf(HANDS_OVER_A_DOUBLING));

  const double result = resultOfKeptFunction(machine, probe, DOUBLED_ARGUMENT);

  EXPECT_EQ(result, DOUBLED_RESULT);
}

/**
 * Given a script that never ends
 * When it runs
 * Then it is stopped for its budget
 */
TEST(ScriptMachine, StopsAScriptThatRunsPastItsBudget) {
  ScriptMachine machine{ScriptLimits{}};

  EXPECT_THAT(errorOfRunning(machine, scriptOf(LOOPS_FOREVER)),
              HasSubstr(SCRIPT_OVER_BUDGET_MESSAGE));
}

/**
 * Given a machine whose scripts may hold 1 MiB
 * When a script asks for 4 MiB
 * Then it fails
 */
TEST(ScriptMachine, StopsAScriptThatHoldsTooMuchMemory) {
  ScriptMachine machine{
      ScriptLimits{.memoryBytes = ONE_MEBIBYTE, .callBudget = GENEROUS_BUDGET}};

  EXPECT_THROW(machine.run(scriptOf(HOLDS_FOUR_MEBIBYTES)), ScriptException);
}

/**
 * Given a sealed machine
 * When a script looks for the files, the processes, the modules and the
 * functions that reach another script's globals
 * Then none of them exists
 */
TEST(ScriptMachine, LetsAScriptReachNothingOfTheSystem) {
  ScriptMachine machine{ScriptLimits{}};

  EXPECT_NO_THROW(machine.run(scriptOf(CHECKS_THE_SANDBOX)));
}

/**
 * Given a script that sets a global
 * When another script runs on the same machine
 * Then it does not see that global
 */
TEST(ScriptMachine, KeepsTheGlobalsOfEachScriptApart) {
  ScriptMachine machine{ScriptLimits{}};
  machine.run(scriptOf(SETS_A_GLOBAL));

  EXPECT_NO_THROW(machine.run(scriptOf(EXPECTS_NO_SUCH_GLOBAL)));
}

/**
 * Given a sealed machine
 * When a script changes a standard library
 * Then it fails
 */
TEST(ScriptMachine, RefusesAChangeOfAStandardLibrary) {
  ScriptMachine machine{ScriptLimits{}};

  EXPECT_THROW(machine.run(scriptOf(CHANGES_THE_STRING_LIBRARY)),
               ScriptException);
}

/**
 * Given a script that does not compile on its second line
 * When it runs
 * Then the error names the script and that line
 */
TEST(ScriptMachine, NamesTheScriptAndTheLineOfAnError) {
  ScriptMachine machine{ScriptLimits{}};

  EXPECT_THAT(errorOfRunning(machine,
                             rtype::engine::ScriptSource{
                                 .name = std::string{BROKEN_SCRIPT_NAME},
                                 .text = std::string{DOES_NOT_COMPILE}}),
              HasSubstr(BROKEN_SCRIPT_PLACE));
}

/**
 * Given a machine that already ran a script
 * When the program exposes more functions
 * Then it is refused: the machine is sealed
 */
TEST(ScriptMachine, RefusesToExposeOnceSealed) {
  ScriptMachine machine{ScriptLimits{}};
  ScriptProbe probe;
  machine.run(scriptOf(SETS_A_GLOBAL));

  EXPECT_THROW(machine.expose(PROBE_TABLE, probeFunctions(), &probe),
               ScriptException);
}

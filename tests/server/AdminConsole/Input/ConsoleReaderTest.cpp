#include <gtest/gtest.h>
#include <string>
#include <vector>
#include "AdminConsoleConstants.hpp"
#include "AdminConsoleTestConstants.hpp"
#include "BoundedQueue.hpp"
#include "ConsoleReader.hpp"
#include "ConsoleReaderFixtures.hpp"
#include "ConsoleReaderTestConstants.hpp"
#include "ScriptedConsoleInput.hpp"

using rtype::concurrency::BoundedQueue;
using rtype::server::CONSOLE_LINE_CAPACITY;
using rtype::server::ConsoleReader;

/**
 * Given an input that hands "games" and half of "players", then the rest,
 * then waits
 * When a reader runs on it
 * Then the queue receives "games" then "players"
 */
TEST(ConsoleReader, HandsEachTypedLineToTheQueue) {
  ScriptedConsoleInput input{
      {std::string{GAMES_AND_HALF_OF_PLAYERS}, std::string{REST_OF_PLAYERS}},
      false};
  BoundedQueue<std::string> lines{CONSOLE_LINE_CAPACITY};

  const ConsoleReader reader{input, lines};

  EXPECT_EQ(linesWithin(lines, 2),
            (std::vector<std::string>{std::string{GAMES_LINE},
                                      std::string{PLAYERS_LINE}}));
}

/**
 * Given an input that ends after "network", with no end of line
 * When a reader runs on it
 * Then that last line still reaches the queue
 */
TEST(ConsoleReader, HandsTheLastLineWhenTheInputEnds) {
  ScriptedConsoleInput input{{std::string{NETWORK_LINE}}, true};
  BoundedQueue<std::string> lines{CONSOLE_LINE_CAPACITY};

  const ConsoleReader reader{input, lines};

  EXPECT_EQ(linesWithin(lines, 1),
            (std::vector<std::string>{std::string{NETWORK_LINE}}));
}

/**
 * Given a reader whose input handed one line, then waits for a line nobody
 * types
 * When the reader is destroyed
 * Then the destruction returns, and the read was woken by the stop
 */
TEST(ConsoleReader, WakesTheWaitingReadWhenDestroyed) {
  ScriptedConsoleInput input{{std::string{GAMES_ENDED}}, false};
  BoundedQueue<std::string> lines{CONSOLE_LINE_CAPACITY};
  {
    const ConsoleReader reader{input, lines};
    ASSERT_EQ(linesWithin(lines, 1).size(), 1U);
  }

  EXPECT_TRUE(input.wokeOnStop());
}

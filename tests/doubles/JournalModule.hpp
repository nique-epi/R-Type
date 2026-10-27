#pragma once

#include <string>
#include <vector>

/** @brief The calls the recording modules of one test received, in order,
 * shared by those modules: each appends a line such as "install first". */
struct JournalModule {
  std::vector<std::string> calls;
};

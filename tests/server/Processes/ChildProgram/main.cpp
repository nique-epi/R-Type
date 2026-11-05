#include <string>
#include <string_view>
#include <thread>
#include "ChildProgramConstants.hpp"

int main(int argumentCount, char** arguments) {
  if (argumentCount >= ARGUMENTS_OF_EXIT &&
      std::string_view{arguments[1]} == EXIT_COMMAND) {
    return std::stoi(arguments[2]);
  }
  std::this_thread::sleep_for(CHILD_SLEEP_DURATION);
  return 0;
}

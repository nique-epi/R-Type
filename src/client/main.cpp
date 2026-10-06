#include <exception>
#include <iostream>
#include "GameWindow.hpp"

int main() {
  try {
    rtype::client::GameWindow gameWindow;
    gameWindow.run();
  } catch (const std::exception& error) {
    std::cerr << "client stopped: " << error.what() << '\n';
    return 1;
  }
  return 0;
}

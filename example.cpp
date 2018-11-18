#include <iostream>
#include "argparse.h"

int main(int argc, char* argv[]) {
  ArgumentParser parser("Argument parser example", argc, argv);
  std::cout << parser.get<int>("a") << std::endl;
  std::cout << parser.get<bool>("flag") << std::endl;
  for (auto i : parser.getv<double>("v")) std::cout << i << " : ";
  std::cout << std::endl;
  return 0;
}
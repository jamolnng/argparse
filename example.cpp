#include <iostream>
#include "argparse.h"

int main(int argc, char* argv[]) {
  ArgumentParser parser("Argument parser example");
  parser.add_argument("-a", "an integer", true);
  parser.add_argument("--flag", "a flag", true);
  parser.add_argument("-v", "a vector", false);
  parser.parse(argc, argv);
  if (parser.is_help()) return 0;
  std::cout << parser.get<int>("a") << std::endl;
  std::cout << (parser.get<bool>("flag") ? "true" : "false") << std::endl;
  for (auto i : parser.getv<double>("v")) std::cout << i << " : ";
  std::cout << std::endl;
  return 0;
}
# argparse
[![Build Status](https://travis-ci.org/jamolnng/argparse.svg?branch=master)](https://travis-ci.org/jamolnng/argparse)

A simple header only command line argument parser

## Usage
Here is a simple example

```cpp
#include "argparse.h"

#include <iostream>
#include <iterator>

int main(int argc, char* argv[]) {
  // run as: [program name] "0 -c" abc -a 1 -sdfl --flag -v 1 2.7 3 4 9 8.12 87
  // [program name] -sdfv 1 -o "C:\Users\User Name\Directory - Name\file.dat" "C:\Users\User Name 2\Directory 2 - Name 2\file2.dat" C:/tmp/tmp.txt
  ArgumentParser parser("Argument parser example");
  parser.add_argument("-a", "an integer");
  parser.add_argument("-s", "an combined flag", true);
  parser.add_argument("-d", "an combined flag", true);
  parser.add_argument("-f", "an combined flag", true);
  parser.add_argument("--flag", "a flag");
  parser.add_argument("-v", "a vector", true);
  parser.add_argument("-l", "--long", "a long argument", false);
  parser.add_argument("--files", "input files", false);
  try {
    parser.parse(argc, argv);
  } catch (const ArgumentParser::ArgumentNotFound& ex) {
    std::cout << ex.what() << std::endl;
    return 0;
  }
  if (parser.is_help()) return 0;
  std::cout << "a: " << parser.get<int>("a") << std::endl;
  std::cout << "flag: " << std::boolalpha << parser.get<bool>("flag")
            << std::endl;
  std::cout << "d: " << std::boolalpha << parser.get<bool>("d") << std::endl;
  std::cout << "long flag: " << std::boolalpha << parser.get<bool>("l")
            << std::endl;
  auto v = parser.getv<double>("v");
  std::cout << "v: ";
  std::copy(v.begin(), v.end(), std::ostream_iterator<double>(std::cout, " "));
  double sum;
  for (auto& d : v) sum += d;
  std::cout << " sum: " << sum << std::endl;
  auto f = parser.getv<std::string>("files");
  std::cout << "files: ";
  std::copy(f.begin(), f.end(),
            std::ostream_iterator<std::string>(std::cout, " | "));
  std::cout << std::endl;
  f = parser.getv<std::string>("");
  std::cout << "free args: ";
  std::copy(f.begin(), f.end(),
            std::ostream_iterator<std::string>(std::cout, " "));
  std::cout << std::endl;
  return 0;
}
```

## Compiling
Just add `argparse.h` to your include path. No longer requires compiler support for `<regex>`

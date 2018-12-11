/**
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License v3 as published by
 * the Free Software Foundation.
 *
 * This Program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * Author: Jesse Laning
 */

#include <iostream>
#include "argparse.h"

int main(int argc, char* argv[]) {
  // run as: [program name] -a 1 -sdf --flag -v 1 2.7 3 4 9 8.12 87 6
  ArgumentParser parser("Argument parser example");
  parser.add_argument("-a", "an integer");
  parser.add_argument("-s", "an combined flag", true);
  parser.add_argument("-d", "an combined flag", true);
  parser.add_argument("-f", "an combined flag", true);
  parser.add_argument("--flag", "a flag");
  parser.add_argument("-v", "a vector", true);
  parser.add_argument("-l", "--long", "a long argument", false);
  try {
    parser.parse(argc, argv);
  } catch (const ArgumentParser::ArgumentNotFound& ex) {
    std::cout << ex.what() << std::endl;
    return 0;
  }
  if (parser.is_help()) return 0;
  std::cout << parser.get<int>("a") << std::endl;
  std::cout << std::boolalpha << parser.get<bool>("flag") << std::endl;
  std::cout << std::boolalpha << parser.get<bool>("d") << std::endl;
  auto v = parser.getv<double>("v");
  std::copy(v.begin(), v.end(), std::ostream_iterator<double>(std::cout, " "));
  if (v.size()) std::cout << std::endl;
  return 0;
}

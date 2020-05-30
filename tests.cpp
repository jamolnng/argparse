/**
 * License: Apache 2.0 with LLVM Exception or GPL v3
 *
 * Author: Jesse Laning
 */

#include <cmath>
#include <functional>
#include <iostream>
#include <string>
#include <unordered_map>

#include "argparse.h"

using namespace argparse;

struct result {
  bool pass;
  int line;
  std::string condition;
  std::string msg;
  std::string test;
};

#define TASSERT(condition, msg)                    \
  if (!(condition)) {                              \
    return {false, __LINE__, #condition, msg, ""}; \
  }

#define TEST(name, code, ...)                    \
  result name();                                 \
  result name() {                                \
    const char* argv[] = {#name, ##__VA_ARGS__}; \
    int argc = sizeof(argv) / sizeof(argv[0]);   \
    ArgumentParser parser(#name, #name);         \
    code;                                        \
    return {true, 0, "", "", ""};                \
  }

TEST(
    no_args, {
      auto err = parser.parse(argc, argv);
      TASSERT(!err, err.what())
    }, )

TEST(
    short_optional_flag_exists,
    {
      parser.add_argument("-f", "a flag", false);

      auto err = parser.parse(argc, argv);
      TASSERT(!err, err.what())

      TASSERT(parser.exists("f"), "flag not found")
    },
    "-f")

TEST(
    short_optional_flag_does_not_exist, {
      parser.add_argument("-f", "a flag", false);

      auto err = parser.parse(argc, argv);
      TASSERT(!err, err.what())

      TASSERT(!parser.exists("f"), "flag found")
    }, )

TEST(
    short_required_flag_exists,
    {
      parser.add_argument("-f", "a flag", true);

      auto err = parser.parse(argc, argv);
      TASSERT(!err, err.what())

      TASSERT(parser.exists("f"), "required flag not found")
    },
    "-f")

TEST(
    short_required_flag_does_not_exist, {
      parser.add_argument("-f", "a flag", true);

      auto err = parser.parse(argc, argv);
      TASSERT(err, err.what())
    }, )

TEST(
    long_optional_flag_exists,
    {
      parser.add_argument("--flag", "a flag", false);

      auto err = parser.parse(argc, argv);
      TASSERT(!err, err.what())

      TASSERT(parser.exists("flag"), "flag not found")
    },
    "--flag")

TEST(
    long_required_flag_exists,
    {
      parser.add_argument("--flag", "a flag", true);

      auto err = parser.parse(argc, argv);
      TASSERT(!err, err.what())

      TASSERT(parser.exists("flag"), "required flag not found")
    },
    "--flag")

TEST(
    long_required_flag_does_not_exist, {
      parser.add_argument("--flag", "a flag", true);

      auto err = parser.parse(argc, argv);
      TASSERT(err, err.what())
    }, )

TEST(
    long_short_optional_flag_pair_exists,
    {
      parser.add_argument("-f", "--flag", "a flag", false);
      parser.add_argument("-t", "--test", "a flag", false);

      auto err = parser.parse(argc, argv);
      TASSERT(!err, err.what())

      TASSERT(parser.exists("flag"), "flag not found")
      TASSERT(parser.exists("f"), "flag not found")
      TASSERT(parser.exists("test"), "flag not found")
      TASSERT(parser.exists("t"), "flag not found")
    },
    "--flag", "-t")

TEST(
    short_combined_flags,
    {
      parser.add_argument("-f", "a flag", false);
      parser.add_argument("-g", "a flag", false);

      parser.add_argument("-i", "a flag", false);
      parser.add_argument("-j", "a flag", false);

      auto err = parser.parse(argc, argv);
      TASSERT(!err, err.what())

      TASSERT(parser.exists("f"), "f flag not found")
      TASSERT(parser.exists("g"), "g flag not found")
      TASSERT(parser.exists("i"), "f flag not found")
      TASSERT(parser.exists("j"), "g flag not found")
    },
    "-fg", "-ji")

TEST(
    vector_flag_empty,
    {
      parser.add_argument("-v", "a flag", false);

      auto err = parser.parse(argc, argv);
      TASSERT(!err, err.what())

      TASSERT(parser.exists("v"), "flag not found")
      auto v = parser.get<std::vector<int>>("v");
      TASSERT(v.size() == 0, "wrong vector values")
    },
    "-v")

TEST(
    vector_flag,
    {
      parser.add_argument("-v", "a flag", false);

      auto err = parser.parse(argc, argv);
      TASSERT(!err, err.what())

      TASSERT(parser.exists("v"), "flag not found")
      auto v = parser.get<std::vector<int>>("v");
      TASSERT(v.size() == 5, "wrong vector values")
    },
    "-v", "1", "2", "3", "4", "5")

TEST(
    short_and_vector_flag,
    {
      parser.add_argument("-v", "a flag", false);
      parser.add_argument("-b", "a flag", false);
      parser.add_argument("-f", "a flag", false);

      auto err = parser.parse(argc, argv);
      TASSERT(!err, err.what())

      TASSERT(parser.exists("b"), "flag not found")
      TASSERT(parser.exists("v"), "flag not found")
      TASSERT(parser.exists("f"), "flag not found")
      auto v = parser.get<std::vector<int>>("v");
      TASSERT(v.size() == 5, "wrong vector values")
    },
    "-b", "0", "-v", "1", "2", "3", "4", "5", "-f", "6", "7", "8")

TEST(
    short_help_flag,
    {
      parser.enable_help();
      auto err = parser.parse(argc, argv);
      TASSERT(!err, err.what())
      TASSERT(parser.exists("h"), "help not found")
      parser.print_help();
    },
    "-h")

TEST(
    long_help_flag,
    {
      parser.enable_help();
      auto err = parser.parse(argc, argv);
      TASSERT(!err, err.what())
      TASSERT(parser.exists("help"), "help not found")
      parser.print_help();
    },
    "--help")

TEST(
    flag_values,
    {
      parser.add_argument("-f", "--flag", "a flag", true);
      parser.add_argument("-t", "--test", "a flag", true);
      parser.add_argument("-g", "a flag", true);

      auto err = parser.parse(argc, argv);
      TASSERT(!err, err.what())

      TASSERT(parser.exists("flag"), "flag not found")
      TASSERT(parser.get<int>("flag") == 2, "wrong flag value")
      TASSERT(parser.exists("t"), "flag not found")
      TASSERT(parser.get<int>("t") == 3, "wrong flag value")
      TASSERT(parser.exists("g"), "flag not found")
      TASSERT(std::abs(parser.get<double>("g") - 0.125) < 0.0000000001,
              "wrong flag value")
    },
    "-f", "2", "--test", "3", "-g", "0.125")

TEST(
    arg_count,
    {
      size_t c = 4;
      parser.add_argument("-f", "--flag", "a flag", true)
          .count(static_cast<int>(c));

      auto err = parser.parse(argc, argv);
      TASSERT(!err, err.what())

      TASSERT(parser.get<std::vector<int>>("f").size() == c,
              "Wrong vector count")
    },
    "-f", "a", "b", "1", "2")

TEST(
    arg_count_more,
    {
      // This test should pass because it will treat the last argument as a
      // free/positional argument
      size_t c = 4;
      parser.add_argument("-f", "--flag", "a flag", true)
          .count(static_cast<int>(c));

      auto err = parser.parse(argc, argv);
      TASSERT(!err, err.what())

      TASSERT(parser.get<std::vector<int>>("f").size() == c,
              "Wrong vector count")
    },
    "-f", "a", "b", "1", "2", "3")

TEST(
    arg_count_less,
    {
      size_t c = 4;
      parser.add_argument("-f", "--flag", "a flag", true)
          .count(static_cast<int>(c));
      parser.add_argument("-b", "--bbbb", "a flag", true).count(1);

      auto err = parser.parse(argc, argv);
      TASSERT(err, err.what())

      TASSERT(parser.get<std::vector<int>>("f").size() == c - 1 &&
                  parser.get<std::vector<int>>("b").size() == 0,
              "Wrong vector count")
    },
    "-f", "a", "b", "1", "-b")

TEST(
    arg_count_zero,
    {
      parser.add_argument("-f", "--flag", "a flag", true).count(0);
      parser.add_argument("-b", "--bbbb", "a flag", true).count(0);
      parser.add_argument("-c", "--cccc", "a flag", true).count(0);

      auto err = parser.parse(argc, argv);
      TASSERT(!err, err.what())

      TASSERT(parser.get<std::vector<int>>("f").size() == 0,
              "Wrong vector count")
      TASSERT(parser.get<std::vector<int>>("b").size() == 0,
              "Wrong vector count")
      TASSERT(parser.get<std::vector<int>>("c").size() == 0,
              "Wrong vector count")
    },
    "-f", "a", "b", "1", "-b", "a", "-c")

TEST(
    positional_argument_found,
    {
      parser.add_argument("-f", "--flag", "a flag", true).count(0);
      parser.add_argument().name("--file").position(3);

      auto err = parser.parse(argc, argv);
      TASSERT(!err, err.what())

      TASSERT(parser.get<std::string>("file") == "myfile",
              "Positional argument error")
    },
    "-f", "1", "2", "myfile", "asdf")

TEST(
    positional_argument_not_found,
    {
      parser.add_argument("-f", "--flag", "a flag", true).count(0);
      parser.add_argument().name("--file").position(3).required(true);

      auto err = parser.parse(argc, argv);
      TASSERT(err, err.what())
    },
    "-f", "1", "2")

TEST(
    positional_argument_overrun,
    {
      parser.add_argument("-f", "--flag", "a flag", true).count(3);
      parser.add_argument().name("--file").position(3).required(true);

      auto err = parser.parse(argc, argv);
      TASSERT(err, err.what())
    },
    "-f", "1", "2", "myfile", "asdf")

TEST(
    positional_argument_last,
    {
      parser.add_argument("-f", "--flag", "a flag", true).count(1);
      parser.add_argument()
          .name("--file")
          .position(ArgumentParser::Argument::Position::LAST)
          .required(true);

      auto err = parser.parse(argc, argv);
      TASSERT(!err, err.what())

      TASSERT(parser.get<std::string>("file") == "asdf",
              "Positional argument error")
    },
    "-f", "1", "2", "myfile", "asdf")

TEST(
    positional_argument_last_override,
    {
      parser.add_argument("-f", "--flag", "a flag", true).count(4);
      parser.add_argument()
          .name("--file")
          .position(ArgumentParser::Argument::Position::LAST)
          .required(true);

      auto err = parser.parse(argc, argv);
      TASSERT(err, err.what())

      TASSERT(parser.get<std::string>("file") == "asdf",
              "Positional argument error")
    },
    "-f", "1", "2", "myfile", "asdf")

#define TT(name) \
  { #name, name }
using test = std::function<result()>;

int main(int argc, const char* argv[]) {
  std::unordered_map<std::string, test> tests{
      TT(no_args),
      TT(short_optional_flag_exists),
      TT(short_optional_flag_does_not_exist),
      TT(short_required_flag_exists),
      TT(short_required_flag_does_not_exist),
      TT(long_optional_flag_exists),
      TT(long_short_optional_flag_pair_exists),
      TT(short_combined_flags),
      TT(vector_flag_empty),
      TT(vector_flag),
      TT(short_and_vector_flag),
      TT(long_required_flag_exists),
      TT(long_required_flag_does_not_exist),
      TT(short_help_flag),
      TT(long_help_flag),
      TT(flag_values),
      TT(arg_count),
      TT(arg_count_more),
      TT(arg_count_less),
      TT(arg_count_zero),
      TT(positional_argument_found),
      TT(positional_argument_not_found),
      TT(positional_argument_overrun),
      TT(positional_argument_last),
      TT(positional_argument_last_override)};

  std::vector<result> results;
  size_t passed = 0;
  if (argc > 1) {
    for (auto i = 1; i < argc; i++) {
      auto t = tests.find(argv[i]);
      if (t != tests.end()) {
        std::cout << t->first << std::endl;
        result r = t->second();
        r.test = t->first;
        if (!r.pass) {
          std::cout << "***********************FAILURE***********************"
                    << std::endl
                    << "Line " << r.line << ": " << r.condition << std::endl
                    << r.msg << std::endl
                    << "*****************************************************"
                    << std::endl
                    << std::endl;
        }
        results.push_back(r);
        passed += static_cast<size_t>(r.pass);
      }
    }
  } else {
    for (auto& t : tests) {
      std::cout << t.first << std::endl;
      result r = t.second();
      r.test = t.first;
      if (!r.pass) {
        std::cout << "***********************FAILURE***********************"
                  << std::endl
                  << "Line " << r.line << ": " << r.condition << std::endl
                  << r.msg << std::endl
                  << "*****************************************************"
                  << std::endl
                  << std::endl;
      }
      results.push_back(r);
      passed += static_cast<size_t>(r.pass);
    }
  }
  std::cout << "Passed: " << passed << "/" << results.size() << std::endl;
  return static_cast<int>(results.size() - passed);
}

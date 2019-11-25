#include "argparse.h"

#include <functional>
#include <iostream>
#include <map>
#include <string>

struct result {
  bool pass;
  int line;
  const char* condition;
  const char* msg;
  const char* test;
};

#define TASSERT(condition, msg)                    \
  if (!(condition)) {                              \
    return {false, __LINE__, #condition, msg, ""}; \
  }

#define TEST(name, code, ...)                  \
  result name() {                              \
    char* argv[] = {#name, ##__VA_ARGS__};     \
    int argc = sizeof(argv) / sizeof(argv[0]); \
    ArgumentParser parser(#name);              \
    code;                                      \
    return {true, 0, "", "", ""};              \
  }

TEST(
    no_args, {
      try {
        parser.parse(argc, argv);
      } catch (const ArgumentParser::ArgumentNotFound& ex) {
        TASSERT(false, ex.what())
      }
    }, )

TEST(
    short_optional_flag_exists,
    {
      parser.add_argument("-f", "a flag", false);
      try {
        parser.parse(argc, argv);
      } catch (const ArgumentParser::ArgumentNotFound& ex) {
        TASSERT(false, ex.what())
      }
      TASSERT(parser.exists("f"), "flag not found")
    },
    "-f")

TEST(
    short_optional_flag_does_not_exist, {
      parser.add_argument("-f", "a flag", false);
      try {
        parser.parse(argc, argv);
      } catch (const ArgumentParser::ArgumentNotFound& ex) {
        TASSERT(false, ex.what())
      }
      TASSERT(!parser.exists("f"), "flag found")
    }, )

TEST(
    short_required_flag_exists,
    {
      parser.add_argument("-f", "a flag", true);
      try {
        parser.parse(argc, argv);
      } catch (const ArgumentParser::ArgumentNotFound& ex) {
        TASSERT(false, ex.what())
      }
      TASSERT(parser.exists("f"), "required flag not found")
    },
    "-f")

TEST(
    short_required_flag_does_not_exist, {
      parser.add_argument("-f", "a flag", true);
      bool failed = false;
      try {
        parser.parse(argc, argv);
      } catch (const ArgumentParser::ArgumentNotFound&) {
        failed = true;
      }
      TASSERT(failed, "required flag found")
    }, )

TEST(
    long_optional_flag_exists,
    {
      parser.add_argument("--flag", "a flag", false);
      try {
        parser.parse(argc, argv);
      } catch (const ArgumentParser::ArgumentNotFound& ex) {
        TASSERT(false, ex.what())
      }
      TASSERT(parser.exists("flag"), "flag not found")
    },
    "--flag")

TEST(
    long_short_optional_flag_pair_exists,
    {
      parser.add_argument("-f", "--flag", "a flag", false);
      parser.add_argument("-t", "--test", "a flag", false);
      try {
        parser.parse(argc, argv);
      } catch (const ArgumentParser::ArgumentNotFound& ex) {
        TASSERT(false, ex.what())
      }
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
      try {
        parser.parse(argc, argv);
      } catch (const ArgumentParser::ArgumentNotFound& ex) {
        TASSERT(false, ex.what())
      }
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
      try {
        parser.parse(argc, argv);
      } catch (const ArgumentParser::ArgumentNotFound& ex) {
        TASSERT(false, ex.what())
      }
      TASSERT(parser.exists("v"), "flag not found")
      auto v = parser.getv<int>("v");
      TASSERT(v.size() == 0, "wrong vector values")
    },
    "-v")

TEST(
    vector_flag,
    {
      parser.add_argument("-v", "a flag", false);
      try {
        parser.parse(argc, argv);
      } catch (const ArgumentParser::ArgumentNotFound& ex) {
        TASSERT(false, ex.what())
      }
      TASSERT(parser.exists("v"), "flag not found")
      auto v = parser.getv<int>("v");
      TASSERT(v.size() == 5, "wrong vector values")
    },
    "-v", "1", "2", "3", "4", "5")

TEST(
    short_and_vector_flag,
    {
      parser.add_argument("-v", "a flag", false);
      parser.add_argument("-b", "a flag", false);
      parser.add_argument("-f", "a flag", false);
      try {
        parser.parse(argc, argv);
      } catch (const ArgumentParser::ArgumentNotFound& ex) {
        TASSERT(false, ex.what())
      }
      TASSERT(parser.exists("b"), "flag not found")
      TASSERT(parser.exists("v"), "flag not found")
      TASSERT(parser.exists("f"), "flag not found")
      auto v = parser.getv<int>("v");
      TASSERT(v.size() == 5, "wrong vector values")
    },
    "-b", "0", "-v", "1", "2", "3", "4", "5", "-f", "6", "7", "8")

TEST(
    short_help_flag,
    {
      try {
        parser.parse(argc, argv);
      } catch (const ArgumentParser::ArgumentNotFound& ex) {
        TASSERT(false, ex.what())
      }
      TASSERT(parser.is_help(), "help not found")
    },
    "-h")

TEST(
    long_help_flag,
    {
      try {
        parser.parse(argc, argv);
      } catch (const ArgumentParser::ArgumentNotFound& ex) {
        TASSERT(false, ex.what())
      }
      TASSERT(parser.is_help(), "help not found")
    },
    "--help")

#define TT(name) \
  { #name, name }
using test = std::function<result()>;
std::map<std::string, test> tests{TT(no_args),
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
                                  TT(short_help_flag),
                                  TT(long_help_flag)};

int main(int argc, const char* argv[]) {
  std::vector<result> results;
  if (argc > 1) {
    for (auto i = 1; i < argc; i++) {
      auto test = tests.find(argv[i]);
      if (test != tests.end()) {
        results.push_back(test->second());
        results.back().test = test->first.c_str();
      }
    }
  } else {
    for (auto& test : tests) {
      results.push_back(test.second());
      results.back().test = test.first.c_str();
    }
  }
  size_t passed = 0;
  for (auto& r : results) {
    passed += static_cast<size_t>(r.pass);
    std::cout << r.test << ": " << (r.pass ? "pass" : "fail") << std::endl;
    if (!r.pass) {
      std::cout << "****************************************************"
                << std::endl
                << "Line " << r.line << ": " << r.condition << std::endl
                << r.msg << std::endl
                << "****************************************************"
                << std::endl
                << std::endl;
    }
  }
  std::cout << "Passed: " << passed << "/" << results.size() << std::endl;
  return static_cast<int>(results.size() - passed);
}
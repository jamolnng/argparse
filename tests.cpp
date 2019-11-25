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
    code return {true, 0, "", "", ""};         \
  }

TEST(short_optional_flag_exists,
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

TEST(short_optional_flag_does_not_exist,
     {
       parser.add_argument("-f", "a flag", false);
       try {
         parser.parse(argc, argv);
       } catch (const ArgumentParser::ArgumentNotFound& ex) {
         TASSERT(false, ex.what())
       }
       TASSERT(!parser.exists("f"), "flag found")
     },
     "")

TEST(short_required_flag_exists,
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

TEST(short_required_flag_does_not_exist,
     {
       parser.add_argument("-f", "a flag", true);
       bool failed = false;
       try {
         parser.parse(argc, argv);
       } catch (const ArgumentParser::ArgumentNotFound& ex) {
         failed = true;
       }
       TASSERT(failed, "required flag found")
     },
     "")

#define TT(name) \
  { #name, name }
using test = std::function<result()>;
std::map<std::string, test> tests{
    TT(short_optional_flag_exists), TT(short_optional_flag_does_not_exist),
    TT(short_required_flag_exists), TT(short_required_flag_does_not_exist)};

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
  return 0;
}
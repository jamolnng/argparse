/**
 * argparse.h is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License v3 as published by
 * the Free Software Foundation.
 *
 * argparse.h is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with argparse.h.  If not, see <https://www.gnu.org/licenses/>.
 *
 * Author: Jesse Laning
 */

#ifndef ARGPARSE_H
#define ARGPARSE_H

#include <algorithm>
#include <cctype>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <locale>
#include <map>
#include <numeric>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

#if defined(__GNUC__) && __GNUC__ < 8
#include <experimental/filesystem>
namespace std {
namespace filesystem = std::experimental::filesystem;
}
#elif defined(_MSC_VER) && _MSVC_LANG < 201703L
#include <filesystem>
namespace std {
namespace filesystem = std::experimental::filesystem;
}  // namespace std
#else
#include <filesystem>
#endif

namespace argparse {
namespace detail {
static std::string _delimit(const std::string &name) {
  return std::string(std::min(name.size(), static_cast<size_t>(2)), '-')
      .append(name);
}
static std::string _strip(const std::string &name) {
  size_t begin = 0;
  begin += name.size() > 0 ? name[0] == '-' : 0u;
  begin += name.size() > 3 ? name[1] == '-' : 0u;
  return name.substr(begin);
}
static std::string _upper(const std::string &in) {
  std::string out(in);
  std::transform(out.begin(), out.end(), out.begin(), ::toupper);
  return out;
}
static std::string _escape(const std::string &in) {
  std::string out(in);
  if (in.find(' ') != std::string::npos)
    out = std::string("\"").append(out).append("\"");
  return out;
}
static bool _not_space(int ch) { return !std::isspace(ch); }
static inline void _ltrim(std::string &s, bool (*f)(int) = _not_space) {
  s.erase(s.begin(), std::find_if(s.begin(), s.end(), f));
}
static inline void _rtrim(std::string &s, bool (*f)(int) = _not_space) {
  s.erase(std::find_if(s.rbegin(), s.rend(), f).base(), s.end());
}
static inline void _trim(std::string &s, bool (*f)(int) = _not_space) {
  _ltrim(s, f);
  _rtrim(s, f);
}
static inline std::string _ltrim_copy(std::string s,
                                      bool (*f)(int) = _not_space) {
  _ltrim(s, f);
  return s;
}
static inline std::string _rtrim_copy(std::string s,
                                      bool (*f)(int) = _not_space) {
  _rtrim(s, f);
  return s;
}
static inline std::string _trim_copy(std::string s,
                                     bool (*f)(int) = _not_space) {
  _trim(s, f);
  return s;
}
template <typename InputIt>
static inline std::string _join(InputIt begin, InputIt end,
                                const std::string &separator = " ") {
  std::ostringstream ss;
  if (begin != end) {
    ss << *begin++;
  }
  while (begin != end) {
    ss << separator;
    ss << *begin++;
  }
  return ss.str();
}
static inline bool _is_number(const char *arg) {
  std::istringstream iss{std::string(arg)};
  float f;
  iss >> std::noskipws >> f;
  return iss.eof() && !iss.fail();
}
}  // namespace detail

class ArgumentParser {
 private:
 public:
  class Argument;

  class Result {
   public:
    Result() {}
    Result(std::string err) noexcept : _error(true), _what(err) {}

    operator bool() const { return _error; }

    const std::string &what() const { return _what; }

   private:
    bool _error{false};
    std::string _what{};
  };

  class Argument {
   public:
    enum Position : int { LAST = -1, DONT_CARE = -2 };

    Argument &name(const std::string &name) {
      _names.push_back(name);
      return *this;
    }

    Argument &names(std::vector<std::string> names) {
      std::copy(names.begin(), names.end(), names.end());
      return *this;
    }

    Argument &description(const std::string &description) {
      _desc = description;
      return *this;
    }

    Argument &required(bool req) {
      _required = req;
      return *this;
    }

    Argument &position(int position) { return *this; }

    template <typename T>
    std::vector<T> getv() {
      std::vector<std::string> argstr = getv<std::string>();
      std::vector<T> v;
      for (auto &s : argstr) {
        std::istringstream in(s);
        T t;
        in >> t;
        v.push_back(t);
      }
      return v;
    }

    template <typename T>
    T get() {
      std::istringstream in(get<std::string>());
      T t;
      in >> t >> std::ws;
      return t;
    }

   private:
    Argument(const std::string &name, const std::string &desc,
             bool required = false)
        : _desc(desc), _required(required) {
      _names.push_back(name);
    }

    Argument() {}

    friend class ArgumentParser;
    friend class ArgumentNotFound;
    int _position{Position::DONT_CARE};
    std::vector<std::string> _names{};
    std::string _desc{};
    bool _required{false};

    std::vector<std::string> _values{};
  };

  ArgumentParser(const std::string &desc)
      : _desc(desc)
  //, _help(false)
  {}

  Argument &add_argument() {
    _arguments.push_back({});
    return _arguments.back();
  }

  Argument &add_argument(const std::string &name, const std::string &long_name,
                         const std::string &desc, const bool required = false) {
    _arguments.push_back({name, desc, required});
    _pairs[name] = long_name;
    return _arguments.back();
  }

  Argument &add_argument(const std::string &name, const std::string &desc,
                         const bool required = false) {
    _arguments.push_back({name, desc, required});
    return _arguments.back();
  }

  // bool is_help() { return _help; }

  void print_help() {
    std::cout << "Usage: " << _bin << " [options]" << std::endl;
    std::cout << "Options:" << std::endl;
    for (auto &a : _arguments) {
      std::string name = a._names[0];
      auto i = _pairs.find(name);
      if (i != _pairs.end()) name.append(", " + i->second);
      std::cout << "    " << std::setw(23) << std::left << name << std::setw(23)
                << a._desc + (a._required ? " (Required)" : "") << std::endl;
    }
  }

  Result parse(int argc, const char *argv[]) {
    if (argc > 1) {
      // build name map
      for (auto i = 0u; i < _arguments.size(); ++i) {
        for (auto &n : _arguments[i]._names) {
          _name_map[n] = i;
        }
      }
      _bin = std::filesystem::path(argv[0]).filename().string();
      std::string name;
      std::vector<std::string> arg_parts;
      std::vector<std::string> free_args;
      auto push_arg = [&name, &arg_parts, this]() {
        if (!name.empty()) {
          if (name[0] == '-') {
            _add_variable(name, arg_parts);
          } else {
            for (char c : name) {
              _add_variable(std::string(1, c), arg_parts);
            }
          }
          arg_parts.clear();
        }
      };
      for (int i = 1; i < argc; i++) {
        size_t slen = std::strlen(argv[i]);
        if (slen == 0) {
          continue;
        } else if (slen >= 2 && argv[i][0] == '-' &&
                   !detail::_is_number(argv[i])) {
          push_arg();
          if (i == argc - 1) {
            name = &(argv[i][1]);
            push_arg();
          } else {
            name = &(argv[i][1]);
          }
        } else if (name.empty()) {
          free_args.push_back(argv[i]);
        } else {
          arg_parts.push_back(argv[i]);
          if (i == argc - 1) {
            push_arg();
          }
        }
      }
      _add_variable("", free_args);
    }
    // if (!_help) {
    for (auto &a : _arguments) {
      if (a._required) {
        if (_variables.find(a._names[0]) == _variables.end()) {
          // Check if a pair name does not exist. If it does exist check
          // that there is not a variable with that name
          if (_pairs.find(a._names[0]) == _pairs.end() ||
              _variables.find(_pairs.find(a._names[0])->second) ==
                  _variables.end()) {
            return Result(("Required argument not found: " + a._names[0] +
                           ((_pairs.find(a._names[0]) == _pairs.end())
                                ? ""
                                : " or " + _pairs.find(a._names[0])->second)));
          }
        }
      }
    }
    //}
    return Result();
  }

  bool exists(const std::string &name) {
    std::string t = detail::_delimit(name);
    if (_pairs.find(t) != _pairs.end()) t = _pairs[t];
    return _variables.find(t) != _variables.end();
  }

  template <typename T>
  std::vector<T> getv(const std::string &name) {
    std::string t = detail::_delimit(name);
    if (_pairs.find(t) != _pairs.end()) t = _pairs[t];
    auto v = _variables.find(t);
    if (v != _variables.end()) {
      return v->getv<T>();
    }
    return std::vector<T>();
    // std::vector<std::string> argstr = getv<std::string>(name);
    // std::vector<T> v;
    // for (auto &s : argstr) {
    //   std::istringstream in(s);
    //   T t;
    //   in >> t;
    //   v.push_back(t);
    // }
    // return v;
  }

  template <typename T>
  T get(const std::string &name) {
    std::string t = detail::_delimit(name);
    if (_pairs.find(t) != _pairs.end()) t = _pairs[t];
    auto v = _variables.find(t);
    if (v != _variables.end()) {
      return v->get<T>();
    }
    return T();
    // std::istringstream in(get<std::string>(name));
    // T t;
    // in >> t >> std::ws;
    // return t;
  }

 private:
  friend class ArgumentNotFound;
  inline void _add_variable(std::string name,
                            std::vector<std::string> &arg_parts) {
    // if (name == "h" || name == "-help") {
    //   _help = true;
    //   print_help();
    // }
    detail::_ltrim(name, [](int c) { return c != static_cast<int>('-'); });
    name = detail::_delimit(name);
    if (_pairs.find(name) != _pairs.end()) name = _pairs[name];
    _variables[name] = arg_parts;
  }

  std::string _desc;
  std::string _bin{};
  // bool _help{false};
  std::vector<Argument> _arguments{};
  std::unordered_map<std::string, std::vector<std::string>> _variables{};
  std::unordered_map<std::string, std::string> _pairs{};

  std::map<std::string, size_t> _name_map{};
};

template <>
inline std::string ArgumentParser::Argument::get<std::string>() {
  return detail::_join(_values.begin(), _values.end());
}
template <>
inline std::vector<std::string> ArgumentParser::Argument::getv<std::string>() {
  return _values;
}

template <>
inline bool ArgumentParser::get<bool>(const std::string &name) {
  return exists(name);
}

// template <>
// inline std::string ArgumentParser::get<std::string>(const std::string &name)
// {
//   std::string t = detail::_delimit(name);
//   if (_pairs.find(t) != _pairs.end()) t = _pairs[t];
//   auto v = _variables.find(t);
//   if (v != _variables.end()) {
//     return detail::_join(v->second.begin(), v->second.end());
//   }
//   return "";
// }
// template <>
// inline bool ArgumentParser::get<bool>(const std::string &name) {
//   return exists(name);
// }
// template <>
// inline std::vector<std::string> ArgumentParser::getv<std::string>(
//     const std::string &name) {
//   std::string t = detail::_delimit(name);
//   if (_pairs.find(t) != _pairs.end()) t = _pairs[t];
//   auto v = _variables.find(t);
//   if (v != _variables.end()) {
//     return v->second;
//   }
//   return std::vector<std::string>();
// }

}  // namespace argparse
#endif

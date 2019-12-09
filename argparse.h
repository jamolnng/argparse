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
static inline bool _is_number(const std::string &arg) {
  std::istringstream iss(arg);
  float f;
  iss >> std::noskipws >> f;
  return iss.eof() && !iss.fail();
}

static inline int _find_equal(const std::string &s) {
  for (size_t i = 0; i < s.length(); ++i) {
    // if find graph symbol before equal, end search
    // i.e. don't accept --asd)f=0 arguments
    // but allow --asd_f and --asd-f arguments
    if (std::ispunct(static_cast<int>(s[i]))) {
      if (s[i] == '=') {
        return static_cast<int>(i);
      } else if (s[i] == '_' || s[i] == '-') {
        continue;
      }
      return -1;
    }
  }
  return -1;
}

static inline size_t _find_name_end(const std::string &s) {
  size_t i;
  for (i = 0; i < s.length(); ++i) {
    if (std::ispunct(static_cast<int>(s[i]))) {
      break;
    }
  }
  return i;
}

namespace is_vector_impl {
template <typename T>
struct is_vector : std::false_type {};
template <typename... Args>
struct is_vector<std::vector<Args...>> : std::true_type {};
}  // namespace is_vector_impl

// type trait to utilize the implementation type traits as well as decay the
// type
template <typename T>
struct is_vector {
  static constexpr bool const value =
      is_vector_impl::is_vector<typename std::decay<T>::type>::value;
};
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

    friend std::ostream &operator<<(std::ostream &os, const Result &dt);

    const std::string &what() const { return _what; }

   private:
    bool _error{false};
    std::string _what{};
  };

  class Argument {
   public:
    enum Position : int { LAST = -1, DONT_CARE = -2 };
    enum Count : int { ANY = -1 };

    Argument &name(const std::string &name) {
      _names.push_back(name);
      return *this;
    }

    Argument &names(std::vector<std::string> names) {
      _names.insert(_names.end(), names.begin(), names.end());
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

    Argument &position(int position) {
      _position = position;
      return *this;
    }

    Argument &count(int count) {
      _count = count;
      return *this;
    }

    bool found() const { return _found; }

    template <typename T>
    typename std::enable_if<detail::is_vector<T>::value, T>::type get() {
      T t = T();
      typename T::value_type vt;
      for (auto &s : _values) {
        std::istringstream in(s);
        in >> vt;
        t.push_back(vt);
      }
      return t;
    }

    template <typename T>
    typename std::enable_if<!detail::is_vector<T>::value, T>::type get() {
      std::istringstream in(get<std::string>());
      T t = T();
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
    int _position{Position::DONT_CARE};
    int _count{Count::ANY};
    std::vector<std::string> _names{};
    std::string _desc{};
    bool _found{false};
    bool _required{false};

    std::vector<std::string> _values{};
  };

  ArgumentParser(const std::string &desc) : _desc(desc) {}

  Argument &add_argument() {
    _arguments.push_back({});
    return _arguments.back();
  }

  Argument &add_argument(const std::string &name, const std::string &long_name,
                         const std::string &desc, const bool required = false) {
    _arguments.push_back(Argument(name, desc, required));
    _arguments.back()._names.push_back(long_name);
    return _arguments.back();
  }

  Argument &add_argument(const std::string &name, const std::string &desc,
                         const bool required = false) {
    _arguments.push_back({name, desc, required});
    return _arguments.back();
  }

  void print_help() {
    std::cout << "Usage: " << _bin << " [options] " << std::endl;
    std::cout << "Options:" << std::endl;
    for (auto &a : _arguments) {
      std::string name = a._names[0];
      for (size_t n = 1; n < a._names.size(); ++n) {
        name.append(", " + a._names[n]);
      }
      std::cout << "    " << std::setw(23) << std::left << name << std::setw(23)
                << a._desc;
      if (a._required) {
        std::cout << " (Required)";
      }
      std::cout << std::endl;
    }
  }

  Result parse(int argc, const char *argv[]) {
    if (argc > 1) {
      // build name map
      for (size_t i = 0u; i < _arguments.size(); ++i) {
        for (auto &n : _arguments[i]._names) {
          std::string name = detail::_ltrim_copy(
              n, [](int c) -> bool { return c != static_cast<int>('-'); });
          if (_name_map.find(name) != _name_map.end()) {
            return Result("Duplicate of argument name: " + n);
          }
          _name_map[name] = i;
        }
      }
      _bin = std::filesystem::path(argv[0]).filename().string();

      // parse
      std::string current_arg;
      size_t arg_len;
      for (int argv_index = 1; argv_index < argc; ++argv_index) {
        current_arg = std::string(argv[argv_index]);
        arg_len = current_arg.length();
        if (arg_len == 0) {
          continue;
        }
        if (arg_len >= 2 &&
            !detail::_is_number(
                current_arg)) {  // ignores the case if the arg is just a -
          // look for -a (short) or --arg (long) args
          if (current_arg[0] == '-') {
            // look for --arg (long) args
            if (current_arg[1] == '-') {
              Result err = _begin_argument(current_arg.substr(2), true);
              if (err) {
                return err;
              }
            } else {  // short args
              Result err = _begin_argument(current_arg.substr(1), false);
              if (err) {
                return err;
              }
            }
          } else {  // argument value
            _add_value(current_arg);
          }
        } else {  // argument value
          _add_value(current_arg);
        }
      }
    }
    if (_help_enabled && exists("help")) {
      return Result();
    }
    for (auto &a : _arguments) {
      if (a._required && !a._found) {
        return Result("Required argument not found: " + a._names[0]);
      }
    }
    return Result();
  }

  void enable_help() {
    add_argument("-h", "--help", "Shows this page", false);
    _help_enabled = true;
  }

  bool exists(const std::string &name) const {
    std::string n = detail::_ltrim_copy(
        name, [](int c) -> bool { return c != static_cast<int>('-'); });
    auto it = _name_map.find(n);
    if (it != _name_map.end()) {
      return _arguments[it->second]._found;
    }
    return false;
  }

  template <typename T>
  T get(const std::string &name) {
    auto t = _name_map.find(name);
    if (t != _name_map.end()) {
      return _arguments[t->second].get<T>();
    }
    return T();
  }

 private:
  Result _begin_argument(const std::string &arg, bool longarg) {
    size_t name_end = detail::_find_name_end(arg);
    std::string arg_name = arg.substr(0, name_end);
    if (longarg) {
      int equal_pos = detail::_find_equal(arg);
      auto nmf = _name_map.find(arg_name);
      if (nmf == _name_map.end()) {
        return Result("Unrecognized command line option '" + arg_name + "'");
      }
      _current = static_cast<int>(nmf->second);
      _arguments[nmf->second]._found = true;
      if (equal_pos == 0 ||
          (equal_pos < 0 &&
           arg_name.length() < arg.length())) {  // malformed argument
        return Result("Malformed argument: " + arg);
      } else if (equal_pos > 0) {
        std::string arg_value = arg.substr(name_end + 1);
        _add_value(arg_value);
      }
    } else {
      Result r;
      if (arg_name.length() == 1) {
        return _begin_argument(arg, true);
      } else {
        for (char &c : arg_name) {
          r = _begin_argument(std::string(1, c), true);
          if (r) {
            return r;
          }
          r = _end_argument();
          if (r) {
            return r;
          }
        }
      }
    }
    return Result();
  }

  Result _add_value(const std::string &value) {
    if (_current >= 0) {
      size_t c = static_cast<size_t>(_current);
      if (_arguments[c]._count != Argument::Count::ANY &&
          static_cast<int>(_arguments[c]._values.size()) >
              _arguments[c]._count) {
        _end_argument();
        goto unnamed;
      }
      _arguments[c]._values.push_back(value);
      if (_arguments[c]._count != Argument::Count::ANY &&
          static_cast<int>(_arguments[c]._values.size()) >=
              _arguments[c]._count) {
        _end_argument();
      }
      return Result();
    } else {
    unnamed:
      // TODO
      return Result();
    }
  }

  Result _end_argument() {
    _current = -1;
    return Result();
  }

  bool _help_enabled{false};
  int _current{-1};
  std::string _desc{};
  std::string _bin{};
  std::vector<Argument> _arguments{};
  std::map<std::string, size_t> _name_map{};
};

std::ostream &operator<<(std::ostream &os, const ArgumentParser::Result &r) {
  os << r.what();
  return os;
}
template <>
inline std::string ArgumentParser::Argument::get<std::string>() {
  return detail::_join(_values.begin(), _values.end());
}
template <>
inline std::vector<std::string>
ArgumentParser::Argument::get<std::vector<std::string>>() {
  return _values;
}

}  // namespace argparse
#endif

#ifndef FANCY_ASSERT_FANCY_ASSERT_CPP_EDWHCKSBS
#define FANCY_ASSERT_FANCY_ASSERT_CPP_EDWHCKSBS

#include "fancy_assert/detail/hedley.h"
#include "fancy_assert/assert.hpp"

#include <vector>
#include <string>
#include <initializer_list>
#include <exception> // std::terminate
#include <utility>

namespace ns_assertions {
namespace _assert {

struct color_t {
  std::uint8_t r;
  std::uint8_t g;
  std::uint8_t b;
};

constexpr color_t olive = {128, 128, 0};
constexpr color_t orange_red = {255, 69, 0};
constexpr color_t azure = {240, 255, 255};
constexpr color_t gray = {128, 128, 128};

auto with_color(color_t c, std::string s) -> std::string {
  s = "\x1b[38;2;000;000;000m" + std::move(s) + "\x1b[0m";

  size_t i = 7;
  s[i++] = '0' + c.r / 100;
  s[i++] = '0' + (c.r % 100) / 10;
  s[i++] = '0' + (c.r % 10);
  ++i;

  s[i++] = '0' + c.g / 100;
  s[i++] = '0' + (c.g % 100) / 10;
  s[i++] = '0' + (c.g % 10);
  ++i;

  s[i++] = '0' + c.b / 100;
  s[i++] = '0' + (c.b % 100) / 10;
  s[i++] = '0' + (c.b % 10);

  return s;
}

struct assertion_data {
  string_view expr;
  callback_t callback;
  string_view op;
  std::string lhs;
  std::string rhs;
};

thread_local std::vector<assertion_data>
    failed_asserts // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
    = {};

template <typename T>
struct finally_t {
  T callback;

  finally_t // NOLINT(hicpp-explicit-conversions)
      (T callable)
      : callback{static_cast<T&&>(callable)} {}
  finally_t(finally_t const&) = delete;
  finally_t(finally_t&&) = delete;
  auto operator=(finally_t const&) -> finally_t& = delete;
  auto operator=(finally_t&&) -> finally_t& = delete;
  ~finally_t() { callback(); }
};

auto split_at(string_view& text, std::size_t n) -> string_view {
  string_view token = text.substr(0, n);
  text = text.substr(n);
  return token;
}

auto starts_tk1(string_view text, bool extended_char_set = false) -> bool {
  return (
      (not extended_char_set and //
       (text.starts_with("<") or //
        text.starts_with(">") or //
        text.starts_with("{") or //
        text.starts_with("}") or //
        text.starts_with("(") or //
        text.starts_with(")") or //
        text.starts_with(" ") or //
        false)) or               //
      text.starts_with("[") or   //
      text.starts_with("]") or   //
      text.starts_with("=") or   //
      text.starts_with(",") or   //
      text.starts_with(";") or   //
      text.starts_with("*") or   //
      text.starts_with("&") or   //
      false);
}

auto starts_tk2(string_view text) -> bool {
  return (
      text.starts_with("->") or //
      text.starts_with("&&") or //
      text.starts_with("::"));
}

enum kind_e {
  whitespace,
  keyword,
  ident,
  qual,
  other,
};

struct token_t { // NOLINT
  string_view text;
  kind_e kind;
};
} // namespace _assert
} // namespace ns_assertions

namespace ns_assertions {
namespace _assert {

auto split_if_starts_with(string_view& code_str, string_view word)
    -> string_view {
  if (code_str.starts_with(word)) {
    return split_at(code_str, word.size());
  }
  return {};
}

auto next_tk(string_view& code_str, bool extended_char_set = false) -> token_t {
  // trim initial spaces
  while (code_str.starts_with(" ")) {
    code_str = code_str.substr(1);
  }

  string_view matching = " ";
  if (extended_char_set) {
    if (code_str.starts_with("<")) {
      matching = ">";
    } else if (code_str.starts_with("{")) {
      matching = "}";
    } else if (code_str.starts_with("(")) {
      matching = ")";
    } else {
      extended_char_set = false;
    }
  }

  if (code_str.empty()) {
    return {code_str.substr(0), other};
  }
  if (starts_tk2(code_str)) {
    return {split_at(code_str, 2), other};
  }

  if (starts_tk1(code_str, extended_char_set)) {
    return {split_at(code_str, 1), other};
  }

  string_view word;
#define Z_FANCY_ASSERT_TOKEN_RETURN(Word, Kind)                                \
  if (not(word = split_if_starts_with(code_str, #Word)).empty()) {             \
    return {word, Kind};                                                       \
  }                                                                            \
  (void)0

  Z_FANCY_ASSERT_TOKEN_RETURN(noexcept, keyword);

  if (code_str.starts_with("__cdecl")) {
    code_str = code_str.substr(7);
    return {"", other};
  }

  Z_FANCY_ASSERT_TOKEN_RETURN(const, qual);
  Z_FANCY_ASSERT_TOKEN_RETURN(volatile, qual);
  Z_FANCY_ASSERT_TOKEN_RETURN(mutable, qual);
  Z_FANCY_ASSERT_TOKEN_RETURN(struct, qual);

  Z_FANCY_ASSERT_TOKEN_RETURN(static, keyword);
  Z_FANCY_ASSERT_TOKEN_RETURN(inline, keyword);
  Z_FANCY_ASSERT_TOKEN_RETURN(extern, keyword);

  Z_FANCY_ASSERT_TOKEN_RETURN(constexpr, keyword);
  Z_FANCY_ASSERT_TOKEN_RETURN(virtual, keyword);

  Z_FANCY_ASSERT_TOKEN_RETURN(decltype(auto), ident);
  Z_FANCY_ASSERT_TOKEN_RETURN(auto, ident);

  Z_FANCY_ASSERT_TOKEN_RETURN(long double, ident);

  Z_FANCY_ASSERT_TOKEN_RETURN(unsigned long long int, ident);
  Z_FANCY_ASSERT_TOKEN_RETURN(unsigned long int, ident);
  Z_FANCY_ASSERT_TOKEN_RETURN(unsigned short int, ident);
  Z_FANCY_ASSERT_TOKEN_RETURN(unsigned long long, ident);
  Z_FANCY_ASSERT_TOKEN_RETURN(unsigned long, ident);
  Z_FANCY_ASSERT_TOKEN_RETURN(unsigned short, ident);

  Z_FANCY_ASSERT_TOKEN_RETURN(long long unsigned int, ident);
  Z_FANCY_ASSERT_TOKEN_RETURN(long unsigned int, ident);
  Z_FANCY_ASSERT_TOKEN_RETURN(short unsigned int, ident);
  Z_FANCY_ASSERT_TOKEN_RETURN(long long unsigned, ident);
  Z_FANCY_ASSERT_TOKEN_RETURN(long unsigned, ident);
  Z_FANCY_ASSERT_TOKEN_RETURN(short unsigned, ident);

  Z_FANCY_ASSERT_TOKEN_RETURN(long long int, ident);
  Z_FANCY_ASSERT_TOKEN_RETURN(long int, ident);
  Z_FANCY_ASSERT_TOKEN_RETURN(short int, ident);
  Z_FANCY_ASSERT_TOKEN_RETURN(long long, ident);
  Z_FANCY_ASSERT_TOKEN_RETURN(long, ident);
  Z_FANCY_ASSERT_TOKEN_RETURN(short, ident);

#undef Z_FANCY_ASSERT_TOKEN_RETURN

  string_view new_str = code_str;
  while (
      not(new_str.empty() or starts_tk1(new_str, extended_char_set) or
          starts_tk2(new_str))) {
    if (new_str.starts_with(matching)) {
      extended_char_set = false;
    }
    new_str = new_str.substr(1);
  }

  string_view token = {code_str.begin(), new_str.begin()};
  code_str = new_str;
  return {token, ident};
}

auto peek_next_tk(string_view code_str, bool extended_char_set = false)
    -> token_t {
  return next_tk(code_str, extended_char_set);
}

auto merge_tk(string_view code_str, token_t prev_tk) -> string_view {
  return {prev_tk.text.begin(), code_str.end()};
}

auto one_of(string_view token, std::initializer_list<string_view> tokens)
    -> bool {
  for (auto tk : tokens) { // NOLINT
    if (token == tk) {
      return true;
    }
  }
  return false;
}

using std::size_t;
enum scope_e { lparen, rparen, lsquare, rsquare, langle, rangle };

struct parse_type_result_t {
  std::vector<token_t> tokens = {};
  size_t max_nesting_count = 0;
  size_t num_nested = 0;
  bool multiline = false;
};

auto parse_type(
    string_view& code_str, size_t indent_level = 0, size_t num_nested = 0)
    -> parse_type_result_t {

  char const* begin = code_str.begin();

  constexpr token_t newline = {"\n", whitespace};
  constexpr token_t indent = {"│ ", whitespace};

  parse_type_result_t res;
  res.num_nested = num_nested;

  auto add_ln = [&] {
    res.multiline = true;
    res.tokens.push_back(newline);
    for (size_t i = 0; i < indent_level; ++i) {
      res.tokens.push_back(indent);
    }
  };

  token_t previous_token{{}, other};
  auto token = next_tk(code_str, true);

  for (; not token.text.empty();
       (previous_token = token, token = next_tk(code_str))) {

    if (token.text == "<") {
      // if token comes after space, assume it's an identifier instead of
      // template syntax

      if (token.text.begin() > begin and *(token.text.begin() - 1) == ' ') {
        code_str = merge_tk(code_str, token);
        ++res.num_nested;
        return res;
      }

      res.tokens.push_back(token);

      bool need_newline = false;
      ++indent_level;
      while (not(token.text.empty() or token.text == ">")) {

        auto nested = parse_type(code_str, indent_level, 0);
        res.multiline = res.multiline or nested.multiline;

        if (nested.tokens.empty()) {
          return {}; // must find type in this context
        }

        res.max_nesting_count =
            std::max(res.max_nesting_count, nested.max_nesting_count + 1);
        res.num_nested += nested.num_nested;

        token = next_tk(code_str);

        if (res.num_nested > 1 or one_of(token.text, {",", ";"})) {
          need_newline = true;
          add_ln();
        }

        res.tokens.insert(
            res.tokens.end(), nested.tokens.begin(), nested.tokens.end());

        if (one_of(token.text, {",", ";"})) {
          res.tokens.push_back(token);
        }
      }
      --indent_level;

      if (token.text.empty()) {
        return {}; // could not find matchine '>'
      }

      if (need_newline) {
        add_ln();
      }
      res.tokens.push_back(token);
      if (peek_next_tk(code_str).text == "::") {
        continue;
      }
      return res;

    } else if (
        one_of(token.text, {"{", "}", "(", "[", ")", "]", ",", ";", ">"}) or
        (token.kind == ident and
         (previous_token.kind == ident or
          one_of(previous_token.text, {"*", " ", "&", "&&"})))) {

      code_str = merge_tk(code_str, token);
      ++res.num_nested;
      return res;
    }
    if (token.text == "::") {
      res.num_nested = 0;
    }
    res.tokens.push_back(token);
  }
  return res;
}

void print_type(
    std::string& output, parse_type_result_t const& type, color_t c) {

  token_t prev_tk = {{}, other};
  for (auto tk : type.tokens) {
    if (tk.text == "::") {
      output += with_color(c, {tk.text.begin(), tk.text.end()});
    } else {
      auto needs_space = [](token_t tok) -> bool {
        return tok.kind == qual or tok.kind == ident or tok.kind == keyword;
      };
      bool add_space = needs_space(tk) and needs_space(prev_tk);

      if (add_space) {
        output += ' ';
      }
      output += {tk.text.begin(), tk.text.end()};
    }
    prev_tk = tk;
  }
}

void parse_func_signature(std::string& output, string_view func) {
  auto color = azure;

  string_view code_str = func;

  token_t token{};
  while (peek_next_tk(code_str).kind == keyword) {
    token = next_tk(code_str);
  }

  bool trailing_return_type = false;

  if (peek_next_tk(code_str).text == "auto") {
    auto copy = code_str;
    next_tk(copy);
    if (not(peek_next_tk(copy).kind == qual or
            one_of(peek_next_tk(copy).text, {"&", "&&", "*"}))) {
      trailing_return_type = true;
      code_str = copy;
      (void)trailing_return_type;
    }
  }

  parse_type_result_t return_type;
  std::vector<parse_type_result_t> fn_params;
  std::vector<token_t> fn_qual;

  std::vector<parse_type_result_t> tp_param_types;
  std::vector<token_t> tp_param_ids;
  std::vector<parse_type_result_t> tp_param_values;

  if (not trailing_return_type) {
    return_type = parse_type(code_str);
  }

  auto fn_name = parse_type(code_str);

  token = next_tk(code_str);
  if (not(token.text == "(")) {
    return;
  }

  // function parameters
  bool first_param = true;
  while (not(token.text.empty_or_matches(")"))) {
    auto res = parse_type(code_str);
    if (res.tokens.empty() and not first_param) {
      return;
    }
    if (not res.tokens.empty()) {
      fn_params.push_back(static_cast<parse_type_result_t&&>(res));
    }
    token = next_tk(code_str);
    first_param = false;
  }

  if (token.text.empty()) {
    return;
  }

  // member function qualifiers
  token = next_tk(code_str);
  while (token.kind == qual or token.text == "noexcept") {
    fn_qual.push_back(token);
    token = next_tk(code_str);
  }

  // return type
  if (trailing_return_type) {
    if (token.text == "->") {
      return_type = parse_type(code_str);
    }
    token = next_tk(code_str);
  }

  // template arguments
  if (token.text == "[") {
    if (peek_next_tk(code_str, true).text == "with") {
      token = next_tk(code_str);
    }

    first_param = true;
    while (not(token.text.empty_or_matches("]"))) {
      bool id_and_eq = false;
      {
        string_view copy = code_str;
        next_tk(copy, true);
        auto eq = next_tk(copy);

        if (eq.text == "=") {
          id_and_eq = true;
        }
      }

      if (id_and_eq) {
        tp_param_types.push_back({}); // NOLINT
      } else {
        // assume type prefix
        auto res = parse_type(code_str);
        if (res.tokens.empty()) {
          return;
        }
        tp_param_types.push_back(static_cast<parse_type_result_t&&>(res));
      }

      // id
      token = next_tk(code_str, true);
      tp_param_ids.push_back(token);

      // '=' sign
      token = next_tk(code_str);

      auto res = parse_type(code_str);
      if (res.tokens.empty() and not first_param) {
        return;
      }
      tp_param_values.push_back(static_cast<parse_type_result_t&&>(res));
      token = next_tk(code_str);
      first_param = false;
    }
  }

  if (tp_param_ids.size() != tp_param_types.size() or
      tp_param_ids.size() != tp_param_values.size()) {
    return;
  }

  output += '\n';

  output += with_color(color, "function name:");
  output += fn_name.multiline ? '\n' : ' ';
  print_type(output, fn_name, color);
  output += '\n';

  output += with_color(color, "return type =");
  output += return_type.multiline ? '\n' : ' ';
  print_type(output, return_type, color);
  output += '\n';

  if (not fn_qual.empty()) {
    output += with_color(color, "function qualifiers: ");
    char const* sep = "";
    for (auto tk : fn_qual) {
      output +=
          with_color(color, sep + std::string{tk.text.begin(), tk.text.end()});
      sep = ", ";
    }
    output += '\n';
  }

  for (auto const& param : fn_params) {
    output += with_color(color, "parameter type =");
    output += param.multiline ? '\n' : ' ';
    print_type(output, param, color);
    output += '\n';
  }

  for (size_t i = 0; i < tp_param_ids.size(); ++i) {
    auto& id = tp_param_ids[i];
    auto& type = tp_param_types[i];
    auto& value = tp_param_values[i];

    output += with_color(color, "template parameter");
    output += ' ';
    output += {id.text.begin(), id.text.end()};
    if (type.tokens.empty()) {
    } else {
      output += with_color(color, " with type =");
      output += type.multiline ? '\n' : ' ';
      print_type(output, type, color);
      output += type.multiline ? '\n' : ' ';
      output += with_color(color, "and value");
    }
    output += with_color(color, " =");
    output += value.multiline ? '\n' : ' ';

    print_type(output, value, color);
    output += '\n';
  }
  output += '\n';
}

auto find(string_view sv, char c) -> char const* {
  char const* it = sv.begin();
  for (; it < sv.end(); ++it) {
    if (*it == c) {
      break;
    }
  }
  return it;
}

void on_fail(long line, char const* file, char const* func, bool is_fatal) {
  auto _clear = [&] { failed_asserts.clear(); };
  finally_t<decltype(_clear)> clear(_clear);

  std::string output;

  output += with_color(
      olive,
      "========================================"
      "=======================================");
  output += '\n';
  output += with_color(azure, std::to_string(failed_asserts.size()));
  output += " assertion";
  if (failed_asserts.size() > 1) {
    output += 's';
  }
  output += ' ';
  output += with_color(orange_red, "failed");
  output += "\nin function:\n";
  output += func;
  output += '\n';
  output += with_color(gray, file + (':' + std::to_string(line)) + ':');
  output += '\n';
  parse_func_signature(output, func);

  char const* separator = "";

  for (auto const& a : failed_asserts) {
    std::string _msg = a.callback.get_str();

    string_view msg{_msg};
    char const* newline = find(msg, '\n');
    bool multiline = newline != msg.end();

    output += separator;

    if (is_fatal) {
      output += with_color(orange_red, "fatal ");
    }
    output += "assertion ";
    output += '`';
    output += with_color(azure, {a.expr.begin(), a.expr.end()});
    output += '\'';
    output += with_color(orange_red, " failed:");

    if (not multiline) {
      output += ' ';
      output += {msg.begin(), msg.end()};
    } else {
      char const* b = msg.begin();
      char const* e = newline;

      while (b != nullptr) {

        output += "\n > ";
        output += {b, e};

        if (e == msg.end()) {
          b = nullptr;
        } else {
          b = e + 1;
          e = find({b, msg.end()}, '\n');
        }
      }
    }

    if (a.callback.m_call != nullptr) {
      std::fprintf(
          stderr,
          "executing callback of assertion `%s'\n",
          (std::string(a.expr.begin(), a.expr.end())).c_str());
      a.callback.call();
    }

    output += "\nassertion expands to: `";
    output += with_color(
        azure, a.lhs + std::string{a.op.begin(), a.op.end()} + a.rhs);
    output += '\'';
    output += '\n';
    separator = "\n";
  }

  output += with_color(
      olive,
      "========================================"
      "=======================================");
  output += '\n';

  std::fprintf(stderr, "%s", output.c_str());
}

void on_expect_fail(long line, char const* file, char const* func) {
  on_fail(line, file, func, false);
}

[[noreturn]] void
on_assert_fail(long line, char const* file, char const* func) {
  on_fail(line, file, func, true);
  std::terminate();
}

void set_assert_params(     //
    string_view expression, //
    callback_t callback,    //
    string_view op,         //
    std::string lhs,        //
    std::string rhs         //
) {
  failed_asserts.push_back({
      expression,
      callback,
      op,
      static_cast<std::string&&>(lhs),
      static_cast<std::string&&>(rhs),
  });
}

} // namespace _assert
} // namespace ns_assertions
#endif /* end of include guard FANCY_ASSERT_FANCY_ASSERT_CPP_EDWHCKSBS */

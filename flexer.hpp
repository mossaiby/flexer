#pragma once

#include <cstddef>
#include <cstdint>
#include <cctype>
#include <cstring>
#include <vector>
#include <utility>
#include <format>
#include <string>
#include <string_view>

namespace flexer
{

constexpr const char *default_filename = "<input>";

class location_t
{
  public:

  location_t() : _filename(nullptr), _row(0), _col(0)
  {
    // nothing to do here!
  }

  location_t(const char *filename, std::size_t row, std::size_t col) : _filename(filename), _row(row), _col(col)
  {
    // nothing to do here!
  }

  const char *filename() const noexcept
  {
    return _filename;
  }

  std::size_t row() const noexcept
  {
    return _row;
  }

  std::size_t col() const noexcept
  {
    return _col;
  }

  std::string to_string() const
  {
    return std::format("{}:{}:{}", _filename, _row, _col);
  }

  private:

  const char *_filename;
  std::size_t _row;
  std::size_t _col;
};

enum class token_kind_t
{
  invalid,
  eof,
  integer,
  symbol,
  keyword,
  punctuation,
  string,
};

class token_t
{
  public:

  token_t() : _kind(token_kind_t::invalid), _location(), _begin(nullptr), _end(nullptr), _index(0), _value_integer(0)
  {
    // nothing to do here!
  }

  void set_kind(const token_kind_t kind)
  {
    _kind = kind;
  }

  token_kind_t get_kind() const noexcept
  {
    return _kind;
  }

  void set_location(const location_t &location) noexcept
  {
    _location = location;
  }

  location_t get_location() const noexcept
  {
    return _location;
  }

  void set_begin(const char *begin) noexcept
  {
    _begin = begin;
  }

  const char *get_begin() const noexcept
  {
    return _begin;
  }

  void set_end(const char *end) noexcept
  {
    _end = end;
  }

  const char *get_end() const noexcept
  {
    return _end;
  }

  void set_index(const std::size_t index) noexcept
  {
    _index = index;
  }

  std::size_t get_index() const noexcept
  {
    return _index;
  }

  void set_value_integer(const std::ptrdiff_t value_integer) noexcept
  {
    _value_integer = value_integer;
  }

  std::ptrdiff_t get_value_integer() const noexcept
  {
    return _value_integer;
  }

  std::string to_string() const
  {
    switch (_kind)
    {
      case token_kind_t::invalid:
      {
        return std::format("invalid `{}`", std::string_view{ _begin, static_cast<std::size_t>(_end - _begin) });
      }

      case token_kind_t::eof:
      {
        return "eof";
      }

      case token_kind_t::integer:
      {
        return std::format("integer `{}`", _value_integer);
      }

      case token_kind_t::symbol:
      {
        return std::format("symbol `{}`", std::string_view{ _begin, static_cast<std::size_t>(_end - _begin) });
      }

      case token_kind_t::keyword:
      {
        return std::format("keyword({}) `{}`", _index, std::string_view{ _begin, static_cast<std::size_t>(_end - _begin) });
      }

      case token_kind_t::punctuation:
      {
        return std::format("punctuation({}) `{}`", _index, std::string_view{ _begin, static_cast<std::size_t>(_end - _begin) });
      }

      case token_kind_t::string:
      {
        return std::format("string({}) `{}`", _index, std::string_view{ _begin, static_cast<std::size_t>(_end - _begin) });
      }

      default:
      {
        std::unreachable();
      }
    }
  }

  private:

  token_kind_t _kind;
  location_t _location;
  const char *_begin;
  const char *_end;
  std::size_t _index;
  std::ptrdiff_t _value_integer;
};

struct state_t // TODO: convert to class with proper encapsulation
{
  state_t() : cur(0), bol(0), row(0)
  {
    // nothing to do here!
  }

  std::size_t cur;
  std::size_t bol;
  std::size_t row;
};

struct _comment_delimiter_t
{
  const char *opening;
  const char *closing;
};

struct string_delimiter_t
{
  const char *opening;
  const char *closing;
};

struct string_escape_sequence_t
{
  const char *escaped;
  const char *unescaped;
};

struct config_t
{
  public:

  config_t() = default;
  
  void configure_as_ansi_c()
  {
    _symbol_starts = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_";
    _symbol_continuations = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_0123456789";

    _keywords.insert(_keywords.end(), { "auto", "break", "case", "char", "const", "continue", "default", "do", "double", "else", "enum", "extern", "float", "for", "goto", "if", "int", "long", "register", "return", "short", "signed", "sizeof", "static", "struct", "switch", "typedef", "union", "unsigned", "void", "volatile", "while" });
    _punctuations.insert(_punctuations.end(), { "(", ")", "[", "]", "{", "}", "...", "*=", "/=", "%=", "+=", "-=", "<<=", ">>=", "&=", "^=", "|=", "->", "++", "--", "&", "*", "+", "-", "~", "!", "/", "%", "<<", ">>", "<=", ">=", "<", ">", "==", "!=", "^", "|", "&&", "||", "?", ":", ";", ".", "=", "," });
    
    _string_delimiters.insert(_string_delimiters.end(), { { "\"", "\"" }, { "\'", "\'" } });
    _string_escape_sequences.insert(_string_escape_sequences.end(), { { "\\\"", "\"" }, { "\\\'", "\'" }, { "\\\\", "\\" }, { "\\a", "\a" }, { "\\b", "\b" }, { "\\f", "\f" }, { "\\n", "\n" }, { "\\r", "\r" }, { "\\t", "\t" }, { "\\v", "\v" } });
    
    _comment_delimiters.insert(_comment_delimiters.end(), { { "/*", "*/" }, { "//", "\n" } });
  }

  void configure_as_c99()
  {
    configure_as_ansi_c();
    _keywords.insert(_keywords.end(), { "inline", "restrict", "_Bool", "_Complex", "_Imaginary" });
  }

  void configure_as_c11()
  {
    configure_as_c99();
    _keywords.insert(_keywords.end(), { "_Alignas", "_Alignof", "_Atomic", "_Generic", "_Noreturn", "_Static_assert", "_Thread_local" });
  }

  void configure_as_c23()
  {
    configure_as_c11();
    _keywords.insert(_keywords.end(), { "alignas", "alignof", "bool", "constexpr", "false", "nullptr", "static_assert", "thread_local", "true", "typeof", "typeof_unqual", "_BitInt", "_Decimal128", "_Decimal32", "_Decimal64" });
  }

  const char *get_symbol_starts() const
  {
    return _symbol_starts;
  }

  void set_symbol_starts(const char *symbol_starts)
  {
    _symbol_starts = symbol_starts;
  }

  const char *get_symbol_continuations() const
  {
    return _symbol_continuations;
  }

  void set_symbol_continuations(const char *symbol_continuations)
  {
    _symbol_continuations = symbol_continuations;
  }

  const std::vector<const char *> get_punctuations() const
  {
    return _punctuations;
  }

  void set_punctuations(std::vector<const char *> &punctuations)
  {
    _punctuations = punctuations;
  }

  const std::vector<const char *> get_keywords() const
  {
    return _keywords;
  }

  void set_keywords(std::vector<const char *> &keywords)
  {
    _keywords = keywords;
  }

  const std::vector<string_delimiter_t> get_string_delimiters() const
  {
    return _string_delimiters;
  }

  void set_string_delimiters(std::vector<string_delimiter_t> &strings)
  {
    _string_delimiters = strings;
  }

  const std::vector<string_escape_sequence_t> get_string_escape_sequences() const
  {
    return _string_escape_sequences;
  }

  void set_string_escape_sequences(std::vector<string_escape_sequence_t> &string_escape_sequences)
  {
    _string_escape_sequences = string_escape_sequences;
  }

  const std::vector<_comment_delimiter_t> get_comment_delimiters() const
  {
    return _comment_delimiters;
  }

  void set_comment_delimiters(std::vector<_comment_delimiter_t> &comment_delimiters)
  {
    _comment_delimiters = comment_delimiters;
  }

  // private: // TODO: make these private

  std::vector<const char *> _keywords; // if one of the keywords is a prefix of another one, the longer one should come first.
  std::vector<const char *> _punctuations; // if one of the punctuations is a prefix of another one, the longer one should come first.

  const char *_symbol_starts;
  const char *_symbol_continuations;

  std::vector<string_delimiter_t> _string_delimiters; // better naming
  std::vector<string_escape_sequence_t> _string_escape_sequences; // better naming
  std::vector<_comment_delimiter_t> _comment_delimiters; // better naming
};

class flexer
{
  public:

  flexer(config_t &config, const char *content, const char *filename = default_filename) : _content(content), _size(std::strlen(content)), _filename(filename), 
  _symbol_starts(config._symbol_starts),
  _symbol_continuations(config._symbol_continuations),
  _punctuations(config._punctuations),
  _keywords(config._keywords),
  _string_delimiters(config._string_delimiters),
  _string_escape_sequences(config._string_escape_sequences),
  _comment_delimiters(config._comment_delimiters)
  {
    // nothing to do here!
  }

  bool get_current_char(char &c)
  {
    if (_state.cur >= _size)
    {
      return false;
    }

    c = _content[_state.cur];
    return true;
  }

  bool chop_character()
  {
    if (_state.cur < _size)
    {
      const char c = _content[_state.cur++];

      if (c == '\n')
      {
        _state.bol = _state.cur;
        _state.row += 1;
      }

      return true;
    }

    return false;
  }

  bool chop_characters(std::size_t n)
  {
    while (n-- > 0)
    {
      if (!chop_character())
      {
        return false;
      }
    }

    return true;
  }

  bool chop_until_eol()
  {
    while (_state.cur < _size && _content[_state.cur] != '\n')
    {
      if (!chop_character())
      {
        return false;
      }
    }

    return true;
  }

  bool chop_until_prefix(const char *prefix)
  {
    while (!starts_with(prefix))
    {
      if (!chop_character())
      {
        return false;
      }
    }

    return true;
  }

  bool chop_until_prefix_eol(const char *prefix)
  {
    while (!starts_with(prefix))
    {
      if (_state.cur >= _size || _content[_state.cur] == '\n')
      {
        return false;
      }

      if (!chop_character())
      {
        return false;
      }
    }

    return true;
  }

  bool trim_left()
  {
    while (_state.cur < _size && std::isspace(_content[_state.cur]))
    {
      if (!chop_character())
      {
        return false;
      }
    }

    return true;
  }

  location_t get_location() const
  {
    return location_t(_filename, _state.row + 1, _state.cur - _state.bol + 1);
  }

  bool is_symbol_start(const char c)
  {
    return std::strchr(_symbol_starts, c) != nullptr;
  }

  bool is_symbol_continuation(const char c)
  {
    return std::strchr(_symbol_continuations, c) != nullptr;
  }

  bool starts_with(const char *prefix)
  {
    for (std::size_t i = 0; prefix[i] != '\0'; i++)
    {
      if (_content[_state.cur + i] != prefix[i] || _state.cur + i >= _size)
      {
        return false;
      }
    }

    return true;
  }

  bool get_token(token_t &t)
  {
    while (_state.cur < _size)
    {
      trim_left();

      bool removed_comment = false;

      // comments
      for (std::size_t i = 0; i < _comment_delimiters.size(); i++)
      {
        const char *opening = _comment_delimiters[i].opening;
        const char *closing = _comment_delimiters[i].closing;

        if (starts_with(opening))
        {
          chop_characters(strlen(opening));
          chop_until_prefix(closing);
          chop_characters(strlen(closing));

          removed_comment = true;
          break; // restart trim
        }
      }

      if (!removed_comment)
      {
        break;
      }
    }

    t.set_location(get_location());
    t.set_begin(_content + _state.cur);
    t.set_end(_content + _state.cur);

    // eof
    if (_state.cur >= _size)
    {
      t.set_kind(token_kind_t::eof);
      return true;
    }

    // punctuations
    for (std::size_t i = 0; i < _punctuations.size(); i++)
    {
      if (starts_with(_punctuations[i]))
      {
        size_t n = std::strlen(_punctuations[i]);
        t.set_kind(token_kind_t::punctuation);
        t.set_index(i);
        t.set_end(t.get_end() + n);
        chop_characters(n);
    
        return true;
      }
    }

    // integer
    if (std::isdigit(_content[_state.cur]))
    {
      t.set_kind(token_kind_t::integer);
      while (_state.cur < _size && std::isdigit(_content[_state.cur]))
      {
        t.set_value_integer(t.get_value_integer() * 10 + _content[_state.cur] - '0');
        t.set_end(t.get_end() + 1);
        chop_character();
      }
    
      return true;
    }

    // symbol
    if (is_symbol_start(_content[_state.cur]))
    {
      t.set_kind(token_kind_t::symbol);
      while (_state.cur < _size && is_symbol_continuation(_content[_state.cur]))
      {
        t.set_end(t.get_end() + 1);
        chop_character();
      }

      // keyword
      for (std::size_t i = 0; i < _keywords.size(); i++)
      {
        size_t n = std::strlen(_keywords[i]);
        if (n == static_cast<std::size_t>(t.get_end() - t.get_begin()) && std::memcmp(_keywords[i], t.get_begin(), n) == 0)
        {
          t.set_kind(token_kind_t::keyword);
          t.set_index(i);
          break;
        }
      }
      
      return true;
    }

    // string
    for (std::size_t i = 0; i < _string_delimiters.size(); i++)
    {
      const char *opening = _string_delimiters[i].opening;
      const char *closing = _string_delimiters[i].closing;

      if (starts_with(opening))
      {
        chop_characters(strlen(opening));

        // TODO: support string escaping!
        if (!chop_until_prefix_eol(closing))
        {
          return false;
        }

        chop_characters(strlen(closing)); // TODO: What happens in case of non-terminated strings?!

        t.set_kind(token_kind_t::string);
        t.set_index(i);
        t.set_end(_content + _state.cur);

        return true;
      }
    }

    chop_character();
    t.set_end(t.get_end() + 1);
    
    return false;
  }

  state_t get_state() const
  {
    return _state;
  }

  void set_state(const state_t state)
  {
    _state = state;
  }

  private:

  const char *_content;
  std::size_t _size;

  const char *_filename;

  state_t _state;

  const char *_symbol_starts;
  const char *_symbol_continuations;

  std::vector<const char *> &_punctuations; // If one of the punctuations is a prefix of another one, the longer one should come first.
  std::vector<const char *> &_keywords; // If one of the keywords is a prefix of another one, the longer one should come first.

  std::vector<string_delimiter_t> &_string_delimiters;
  std::vector<string_escape_sequence_t> &_string_escape_sequences;
  std::vector<_comment_delimiter_t> &_comment_delimiters;
};

}
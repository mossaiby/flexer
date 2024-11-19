#include <cstddef>
#include <cstdint>
#include <cctype>
#include <cstring>
#include <vector>
#include <utility>

namespace flexer
{

constexpr const char *default_filename = "<input stream>"; // Any better idea?

struct location_t
{
  location_t() : filename(nullptr), row(0), col(0)
  {
    //
  }

  location_t(const char *filename, std::size_t row, std::size_t col) : filename(filename), row(row), col(col)
  {
    //
  }

  const char *filename;
  std::size_t row;
  std::size_t col;
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

struct token_t
{
  token_kind_t kind;
  location_t location;
  const char *begin;
  const char *end;
  union
  {
    std::size_t index;
    std::ptrdiff_t value_integer;
  };
};

struct multiline_comment_t
{
  const char *opening;
  const char *closing;
};

struct state_t
{
  state_t() : cur(0), bol(0), row(0)
  {
    //
  }

  std::size_t cur;
  std::size_t bol;
  std::size_t row;
};

const char *token_kind_name(token_kind_t k) // Replace this with flexer::decode_token or similar
{
  switch (k)
  {
    case token_kind_t::invalid:
    {
      return "invalid";
    }

    case token_kind_t::eof:
    {
      return "end";
    }

    case token_kind_t::integer:
    {
      return "integer";
    }

    case token_kind_t::symbol:
    {
      return "symbol";
    }

    case token_kind_t::keyword:
    {
      return "keyword";
    }

    case token_kind_t::punctuation:
    {
      return "punctuation";
    }

    case token_kind_t::string:
    {
      return "string";
    }

    default:
    {
      std::unreachable();
    }
  }
}

class flexer
{
  public:

  flexer(const char *content, const char *filename = default_filename) : _content(content), _size(std::strlen(content)), _filename(filename)
  {
    //
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

  void chop_characters(std::size_t n)
  {
    while (n-- > 0 && chop_character());
  }

  void trim_left()
  {
    while (_state.cur < _size && std::isspace(_content[_state.cur]))
    {
      chop_character();
    }
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
    for (std::size_t i = 0; _state.cur + i < _size && prefix[i] != '\0'; i++)
    {
      if (_content[_state.cur + i] != prefix[i])
      {
        return false;
      }
    }

    return true;
  }

  void drop_until_eol()
  {
    while (_state.cur < _size)
    {
      const char c = _content[_state.cur];
      chop_character();
      
      if (c == '\n')
      {
        break;
      }
    }
  }

  void chop_until_prefix(const char *prefix)
  {
    while (_state.cur < _size && !starts_with(prefix))
    {
      chop_character();
    }
  }

  bool get_token(token_t &t)
  {
    another_trim_round:
    while (_state.cur < _size)
    {
      trim_left();

      // Single-line comments
      for (std::size_t i = 0; i < _singleline_comments.size(); i++)
      {
        if (starts_with(_singleline_comments[i]))
        {
          drop_until_eol();
          goto another_trim_round;
        }
      }

      // Multi-line comments
      for (std::size_t i = 0; i < _multiline_comments.size(); i++)
      {
        const char *opening = _multiline_comments[i].opening;
        const char *closing = _multiline_comments[i].closing;
        if (starts_with(opening))
        {
          chop_characters(strlen(opening));
          chop_until_prefix(closing);
          chop_characters(strlen(closing));
        
          goto another_trim_round;
        }
      }

      break;
  }

  t.location = get_location();
  t.begin = t.end = _content + _state.cur;

  // End
  if (_state.cur >= _size)
  {
    t.kind = token_kind_t::eof;
    return true;
  }

  // Punctuations
  for (std::size_t i = 0; i < _punctuations.size(); i++)
  {
    if (starts_with(_punctuations[i]))
    {
        size_t n = std::strlen(_punctuations[i]);
        t.kind = token_kind_t::punctuation;
        t.index = i;
        t.end += n;
        chop_characters(n);
        
        return true;
    }
  }

  // Integer
  if (std::isdigit(_content[_state.cur]))
  {
    t.kind = token_kind_t::integer;
    while (_state.cur < _size && std::isdigit(_content[_state.cur]))
    {
      t.value_integer = t.value_integer * 10 + _content[_state.cur] - '0';
      t.end += 1;
      chop_character();
    }

    return true;
  }

  // Symbol
  if (is_symbol_start(_content[_state.cur]))
  {
      t.kind = token_kind_t::symbol;
      while (_state.cur < _size && is_symbol_continuation(_content[_state.cur]))
      {
        t.end += 1;
        chop_character();
      }

      // Keyword
      for (std::size_t i = 0; i < _keywords.size(); i++)
      {
        size_t n = std::strlen(_keywords[i]);
        if (n == static_cast<std::size_t>(t.end - t.begin) && std::memcmp(_keywords[i], t.begin, n) == 0)
        {
          t.kind = token_kind_t::keyword;
          t.index = i;

          break;
        }
      }

      return true;
    }

    chop_character();
    t.end += 1;

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

  std::vector<const char *> get_punctuations() const
  {
    return _punctuations;
  }

  void set_punctuations(std::vector<const char *> punctuations)
  {
    _punctuations = punctuations;
  }

  std::vector<const char *> get_keywords() const
  {
    return _keywords;
  }

  void set_keywords(std::vector<const char *> keywords)
  {
    _keywords = keywords;
  }

  std::vector<const char *> get_singleline_comments() const
  {
    return _singleline_comments;
  }

  void set_singleline_comments(std::vector<const char *> singleline_comments)
  {
    _singleline_comments = singleline_comments;
  }

  std::vector<multiline_comment_t> get_multiline_comments() const
  {
    return _multiline_comments;
  }

  void set_multiline_comments(std::vector<multiline_comment_t> multiline_comments)
  {
    _multiline_comments = multiline_comments;
  }

  private:

  const char *_content;
  std::size_t _size;

  const char *_filename;

  state_t _state;

  const char *_symbol_starts = "_abcdefghijklmnopqrstuvwxyz";
  const char *_symbol_continuations = "_abcdefghijklmnopqrstuvwxyz0123456789";

  std::vector<const char *> _punctuations; // If one of the punctuations is a prefix of another one, the longer one should come first.
  std::vector<const char *> _keywords; // If one of the keywords is a prefix of another one, the longer one should come first.

  std::vector<const char *> _singleline_comments; // Better name
  std::vector<multiline_comment_t> _multiline_comments; // Better name
};

}
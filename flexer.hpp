#include <cstddef>
#include <cstdint>
#include <cctype>
#include <cstring>
#include <vector>
#include <utility>

namespace flexer
{

constexpr const char *default_filename = ""; // Any better idea?

struct location_t
{
  const char *filename;
  std::size_t row;
  std::size_t col;
};

enum class token_kind_t : std::ptrdiff_t
{
  invalid = -1,
  end,
  integer,
  symbol,
  keyword,
  punctuation,
  string,
};

typedef struct
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
} token_t;

typedef struct
{
  const char *opening;
  const char *closing;
} lexer_multiline_comment_t;

typedef struct {
  std::size_t cur;
  std::size_t bol;
  std::size_t row;
} lexer_state_t;

const char *token_kind_name(token_kind_t k)
{
  switch (k)
  {
    case token_kind_t::invalid:
    {
      return "invalid";
    }

    case token_kind_t::end:
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

void diagnostic(location_t location, const char *level, const char *fmt, ...)
{
  //
}

class lexer
{
  public:

  lexer(const char *content, const char *filename = default_filename) : _content(content), _size(std::strlen(content)), _filename(filename)
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

  bool chop_char()
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

  void chop_chars(std::size_t n)
  {
    while (n-- > 0 && chop_char());
  }

  void trim_left_ws()
  {
    while (_state.cur < _size && std::isspace(_content[_state.cur]))
    {
      chop_char();
    }
  }

  location_t get_location() const
  {
    return { _filename, _state.row + 1, _state.cur - _state.bol + 1 };
  }

  bool is_symbol_start(const char c)
  {
    return std::strchr(_symbol_starts, c) != nullptr;
  }

  bool is_symbol_continuation(const char c)
  {
    return std::strchr(_symbol_continuations, c) != nullptr;
  }

  bool starts_with_cstr(const char *prefix)
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

  void drop_until_endline()
  {
    while (_state.cur < _size)
    {
      const char c = _content[_state.cur];
      chop_char();
      
      if (c == '\n')
      {
        break;
      }
    }
  }

  void chop_until_prefix(const char *prefix)
  {
    while (_state.cur < _size && !starts_with_cstr(prefix))
    {
      chop_char();
    }
  }

  bool get_token(token_t &t)
  {
    another_trim_round:
    while (_state.cur < _size)
    {
      trim_left_ws();

      // Single-line comments
      for (std::size_t i = 0; i < _singleline_comments.size(); i++)
      {
        if (starts_with_cstr(_singleline_comments[i]))
        {
          drop_until_endline();
          goto another_trim_round;
        }
      }

      // Multi-line comments
      for (std::size_t i = 0; i < _multiline_comments.size(); i++)
      {
        const char *opening = _multiline_comments[i].opening;
        const char *closing = _multiline_comments[i].closing;
        if (starts_with_cstr(opening))
        {
          chop_chars(strlen(opening));
          chop_until_prefix(closing);
          chop_chars(strlen(closing));
        
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
    t.kind = token_kind_t::end;
    return true;
  }

  // Punctuations
  for (std::size_t i = 0; i < _punctuations.size(); i++)
  {
    if (starts_with_cstr(_punctuations[i]))
    {
        size_t n = std::strlen(_punctuations[i]);
        t.kind = token_kind_t::punctuation;
        t.index = i;
        t.end += n;
        chop_chars(n);
        
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
      chop_char();
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
        chop_char();
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

    chop_char();
    t.end += 1;

    return false;
  }

  lexer_state_t get_state() const
  {
    return _state;
  }

  void set_state(const lexer_state_t state)
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

  std::vector<const char *> get_singleline_comments() const
  {
    return _singleline_comments;
  }

  void set_singleline_comments(std::vector<const char *> singleline_comments)
  {
    _singleline_comments = singleline_comments;
  }

  std::vector<lexer_multiline_comment_t> get_multiline_comments() const
  {
    return _multiline_comments;
  }

  void set_multiline_comments(std::vector<lexer_multiline_comment_t> multiline_comments)
  {
    _multiline_comments = multiline_comments;
  }

  private:

  const char *_content;
  std::size_t _size;

  const char *_filename;

  lexer_state_t _state;

  const char *_symbol_starts = "_abcdefghijklmnopqrstuvwxyz";
  const char *_symbol_continuations = "_abcdefghijklmnopqrstuvwxyz0123456789";

  std::vector<const char *> _punctuations; // If one of the punctuations is a prefix of another one, the longer one should come first.
  std::vector<const char *> _keywords; // If one of the keywords is a prefix of another one, the longer one should come first.

  std::vector<const char *> _singleline_comments; // Better name
  std::vector<lexer_multiline_comment_t> _multiline_comments; // Better name
};

}
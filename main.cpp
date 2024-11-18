#include <iostream>

#include "flexer.hpp"

int main()
{
  std::vector<const char *> punctuations { "(", ")", "{", "}", ",", ";" };

  const char *content = "int test() { return 42; }";
  flexer::lexer lexer(content);

  lexer.set_punctuations(punctuations);

  flexer::token_t t;
  do
  {
    if (!lexer.get_token(t))
    {
      std::cout << "Invalid character: '";
      std::cout.write(t.begin, static_cast<std::size_t>(t.end - t.begin));
      std::cout << "' at " << t.location.filename << ":" << t.location.row << ":" << t.location.col << "\n";

      break;
    }

    std::cout << flexer::token_kind_name(t.kind) << ": '";
    std::cout.write(t.begin, static_cast<std::size_t>(t.end - t.begin));
    std::cout << "' at " << t.location.filename << ":" << t.location.row << ":" << t.location.col << "\n";
  } while (t.kind != flexer::token_kind_t::end);

  return 0;
}
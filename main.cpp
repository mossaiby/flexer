#include <iostream>

#include "flexer.hpp"

int main()
{
  std::vector<const char *> punctuations { "::", ":", "?", "(", ")", "{", "}", ",", ";", "==", ">>", "<<", "||", "=", ">", "<", "|" };
  std::vector<const char *> keywords { "for", "while", "do", "if", "else", "return", "break", "continue" };

  const char *content = "int test() { if (x == 5) return 42; else y = 5; }";
  flexer::flexer flexer(content);

  flexer.set_punctuations(punctuations);
  flexer.set_keywords(keywords);

  flexer::token_t t;
  do
  {
    if (!flexer.get_token(t))
    {
      std::cout << "Invalid character: '";
      std::cout.write(t.begin, static_cast<std::size_t>(t.end - t.begin));
      std::cout << "' at " << t.location.filename << ":" << t.location.row << ":" << t.location.col << "\n";

      break;
    }

    std::cout << flexer::token_kind_name(t.kind) << ": '";
    std::cout.write(t.begin, static_cast<std::size_t>(t.end - t.begin));
    std::cout << "' at " << t.location.filename << ":" << t.location.row << ":" << t.location.col << "\n";
  } while (t.kind != flexer::token_kind_t::eof);

  return 0;
}
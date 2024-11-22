#include <iostream>

#include "flexer.hpp"

int main()
{
  std::vector<const char *> punctuations { "::", ":", "?", "(", ")", "{", "}", ",", ";", "==", ">>", "<<", "||", "=", ">", "<", "|" };
  std::vector<const char *> keywords { "for", "while", "do", "if", "else", "return", "break", "continue", "void" };

  std::vector<flexer::string_t> strings { { "\"", "\"" }};
  std::vector<flexer::string_escape_t> string_escapes { { "\\\"", "\"" }, { "\\\n", "\n" } }; // Complete this!

  std::vector<const char *> single_line_comments { "//" };
  std::vector<flexer::multi_line_comment_t> multi_line_comments { { "/*", "*/" } };

  const char *content = 
    "int test() { if (x == 5) return /* you know... */ 42; else y = \"5\"; } // Wow! This is nice ;-)\n"
    "\n"
    "// Another comment\n"
    "/*\n"
    " * multi-line comment!\n"
    "*/\n"
    "void f() { return \"test completed!\\n\"; }";
  flexer::flexer flexer(content);

  flexer.set_punctuations(punctuations);
  flexer.set_keywords(keywords);

  flexer.set_strings(strings);
  flexer.set_string_escapes(string_escapes);

  flexer.set_single_line_comments(single_line_comments);
  flexer.set_multi_line_comments(multi_line_comments);

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
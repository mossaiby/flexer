#include <iostream>

#include "flexer.hpp"

int main()
{
  const char *content = 
    "int test(volatile bool x) { if (x == 5) return /* you know... */ 42; else y = \"5\"; } // Wow! This is nice ;-)\n"
    "\n"
    "// Another comment\n"
    "/*\n"
    " * multi-line comment!\n"
    "*/\n"
    "void f() { return x == 0 ? 'a' : \"test completed!\\n\"; }";


  flexer::config_t config;
  config.configure_as_c23();
  
  flexer::flexer flexer(config, content);

  while (true)
  {
    flexer::token_t t;

    flexer.get_token(t);
    std::cout << t.to_string() << " at " << t.get_location().to_string() << "\n";

    if (t.get_kind() == flexer::token_kind_t::invalid)
    {
      std::cout << "*** invalid token detected ***\n";
      break;
    }

    if (t.get_kind() == flexer::token_kind_t::eof)
    {
      break;
    }
  }

  return 0;
}
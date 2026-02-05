#include <iostream>
#include <fstream>

#include "flexer.hpp"

int main(int argc, const char *argv[])
{
  if (argc != 2)
  {
    std::cout << "usage: " << argv[0] << " <file>\n";
    return 1;
  }

  // read the file into `content`
  const char *filename = argv[1];
  std::ifstream file(filename);
  if (!file.is_open())
  {
    std::cout << "error reading file: " << filename << "\n";
    return 1;
  }

  std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
  file.close();

  flexer::config_t config;
  config.configure_as_c23();
  
  flexer::flexer flexer(config, content.c_str(), filename);

  flexer::token_t t;

  while (true)
  {
    flexer.get_token(t);
    std::cout << t.to_string() << " at " << t.get_location().to_string() << "\n";

    if (t.get_kind() == flexer::token_kind_t::invalid)
    {
      std::cout << "*** invalid token detected ***\n";
      return 1;
    }

    if (t.get_kind() == flexer::token_kind_t::eof)
    {
      break;
    }
  }

  return 0;
}
#include <cstring>

#include <fstream>
#include <string>
#include <stdexcept>

#include <peg_parser/generator.h>
typedef peg_parser::ParserGenerator<std::string> Parser;


int main(int argc, char **argv) {
  std::string inputFile;
  for (int i = 1; i < argc; ++i) {
    if (!strcmp(argv[i], "-f")) {
      if (i + 1 < argc) inputFile = argv[i + 1];
      else throw std::invalid_argument("format error");
    }
  }
  return 0;
}

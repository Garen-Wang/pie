#include <cstring>
#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include <fstream>
#include <sstream>
#include "parser.cpp"

Parser generate() {
  Parser g;
  config(g);
  return g;
}

void io(Parser &g, const std::string &inputFile) {
  std::string str, input;
  if (!inputFile.empty()) {
    std::ifstream ifs(inputFile);
    std::ostringstream temp;
    temp << ifs.rdbuf();
    input = temp.str();
    input += '\n';
  } else {
    while (true) {
      std::getline(std::cin, str);
      if (str == "q" || str == "quit") break;
      // WARNING: check input string to prevent injection attack
      if (str != "") input += str + '\n';
      else break;
    }
  }
  // DEBUG
  // std::cout << input << std::endl;
  try {
    auto output = g.run(input);
    std::cout << "Parsing result: " << output << std::endl;
    input.clear();
  } catch (peg_parser::SyntaxError &error) {
    std::cout << "Syntax error when parsing " << error.syntax->rule->name << std::endl;
  }
}

int main(int argc, char **argv) {
  std::string inputFile;
  for (int i = 1; i < argc; ++i) {
    // std::cout << argv[i] << std::endl;
    if (!strcmp(argv[i], "-f")) {
      if (i + 1 < argc) inputFile = argv[i + 1];
      else throw std::invalid_argument("format error");
    }
  }
  auto g = generate();
  io(g, inputFile);
  return 0;
}
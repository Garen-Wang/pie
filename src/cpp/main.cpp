#include <iostream>
#include <string>
#include <vector>

#include "parser.cpp"

Parser generate() {
  Parser g;
  config(g);
  return g;
}

void io(Parser &g) {
  std::string str, input;
  while (true) {
    std::getline(std::cin, str);
    if (str == "q" || str == "quit") break;
    // WARNING: check input string to prevent injection attack
    if (str != "") input += str + '\n';
    else break;
  }
  try {
    auto output = g.run(input);
    std::cout << "Parsing result: " << output << std::endl;
    input.clear();
  } catch (peg_parser::SyntaxError &error) {
    std::cout << "Syntax error when parsing " << error.syntax->rule->name << std::endl;
  }
}

int main() {
  auto g = generate();
  io(g);
  return 0;
}
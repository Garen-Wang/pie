#include <peg_parser/generator.h>

#include <iostream>
#include <unordered_set>

int main() {
  // using namespace std;
  using std::cin, std::cout, std::endl;

  peg_parser::ParserGenerator<std::string> typeChecker;
  std::unordered_set<std::string> types;

  auto &g = typeChecker;
  g.setSeparator(g["Whitespace"] << "[\t ]");
  g.setStart(g["Expression"] << "Typedef | Vardef | Multiplication");
  g["Typedef"] << "'type' Name" >> [&](auto e) {
    types.emplace(e[0].string());
    return "type definition";
  };

  g["Multiplication"] << "Variable '*' Variable" >> [](auto) {
    return "multiplication";
  };

  g["Vardef"] << "Type Name" >> [](auto) {
    return "variable definition";
  };

  g["Typename"] << "Name" << [&](auto s) -> bool {
    auto name = s->inner[0]->string();
    return types.find(name) != types.end();
  };

  g["Type"] << "Typename '*'?";
  g["Variable"] << "Name";
  g["Atomic"] << "Variable";
  g["Name"] << "[a-zA-Z] [a-zA-Z0-9]*";

  while (true) {
    std::string str;
    cout << "> ";
    getline(cin, str);
    if (str == "q" || str == "quit") {
      break;
    }
    try {
      auto result = typeChecker.run(str);
      std::cout << str<< " = " << result << std::endl;
    } catch (peg_parser::SyntaxError &error) {
      auto syntax = error.syntax;
      cout << "  ";
      cout << std::string(syntax->begin, ' ');
      cout << std::string(syntax->length(), '~');
      cout << "^\n";
      cout << "  ";
      cout << "Syntax error while parsing" << syntax->rule->name << endl;
    }
  }
  return 0;
}
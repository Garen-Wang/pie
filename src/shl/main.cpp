#include <cstring>
#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>
#include <map>
#include <string_view>

#include <peg_parser/generator.h>
typedef peg_parser::ParserGenerator<std::string> Parser;

bool debug = false;
std::map<std::string, Parser> parsers;
std::map<std::string, int> colors;
bool isColor(const std::string& color) {
  return colors.find(color) != colors.end();
}

Parser generateParserForParser() {
  if (parsers.find("parser") != parsers.end()) return parsers["parser"];
  Parser g;
  g["Debug"] << "'#debug'" >> [&](auto) {
    debug = true;
    return "debug";
  };
  g["Grammar"] << "Identifier ':' Indent GrammarExpr Block" >> [&](auto s) {
    return "grammar";
  };
  g["Identifier"] << "[a-zA-Z_] [a-zA-Z0-9_]*" >> [](auto) {
    return "Identifier";
  };
  g["GrammarExpr"] << "('(' (!'\n' .)* ')')" >> [](auto) {
    return "GrammarExpr";
  };
  g["Block"] << "('{' (color | (dollar num equals Attr)*) '}')" >> [](auto) {
    return "Block";
  };
  g["Attr"] << "(bold? italic? underlined? color?)" >> [&](auto s) {
    for (int i = 0; i < s.size(); ++i) {
      std::cout << s[i].string() << std::endl;
    }
    return "Attr";
  };
  g.setSeparator(g["Separators"] << "['\t''\n' ]");
  g["Program"] << "((Grammar | Debug)*)";
  g.setStart(g["Program"]);
  parsers["parser"] = g;
  return g;
}

std::shared_ptr<Parser> ParseParser(std::string_view shlFile) {
  auto g = generateParserForParser();
  try {
    auto output = g.run(shlFile);
    std::cout << output << std::endl;
  } catch (peg_parser::SyntaxError &e) {
    std::cout << "Syntax error when parsing " << e.syntax->rule->name << std::endl;
  }
  return nullptr;
}

std::shared_ptr<Parser> generateParserFromShl(const std::string& shlFileName) {
  std::ifstream is;
  is.open(shlFileName);
  is.seekg(0, std::ifstream::end);
  int len = is.tellg();
  is.seekg(0, std::ifstream::beg);
  // auto buffer = new char[len];
  auto buffer = std::make_shared<char[]>(len);
  is.read(buffer.get(), len);
  is.close();
  std::string_view shlFile {buffer.get()};
  return ParseParser(shlFile);
}

std::shared_ptr<Parser> generateCppParser() {
  return generateParserFromShl("../src/shl/cpp.shl");
}

std::shared_ptr<Parser> generateJavaParser() {
  return generateParserFromShl("../src/shl/java.shl");
}

std::shared_ptr<Parser> generatePython3Parser() {
  return generateParserFromShl("../src/shl/python3.shl");
}

std::shared_ptr<Parser> generateJsonParser() {
  return generateParserFromShl("../src/shl/json.shl");
}

std::shared_ptr<Parser> generateLanguageParser(const std::string& fileName) {
  auto pos = fileName.find_last_of('.');
  auto suffix = fileName.substr(pos);
  if (suffix == ".cpp") {
    return generateCppParser();
  } else if (suffix == ".java") {
    return generateJavaParser();
  } else if (suffix == ".py") {
    return generatePython3Parser();
  } else if (suffix == "json") {
    return generateJsonParser();
  } else {
    std::cout << "Not implemented yet" << std::endl;
    return nullptr;
  }
}

int main(int argc, char **argv) {
  std::string fileName;
  for (int i = 1; i < argc; ++i) {
    if (!strcmp(argv[i], "-f")) {
      if (i + 1 < argc) fileName = argv[i + 1];
      else throw std::invalid_argument("format error");
    }
  }
//  auto g = generateLanguageParser("./main.cpp");
  generateCppParser();

  return 0;
}
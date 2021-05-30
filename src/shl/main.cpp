//
// Created by garen on 5/23/21.
//
#include "shl/presets.h"

int main(int argc, char **argv) {
  std::string shlFileName, testFileName;
  for (int i = 1; i < argc; ++i) {
    if (!strcmp(argv[i], "-f")) {
      if (i + 1 < argc) shlFileName = argv[i + 1];
      else throw std::invalid_argument("Arguments format error.");
    } else if (!strcmp(argv[i], "-t")) {
      if (i + 1 < argc) testFileName = argv[i + 1];
      else throw std::invalid_argument("Arguments format error.");
    }
  }
//  auto result = generateParserFromSHL(shlFileName);

  auto result = generateParserFromSHL("../src/python/python3.shl");
  auto gen = result.second;
  gen["Program"] << "Statement | Expr";
//  gen["Program"] << "DefHead";
  gen.setStart(gen["Program"]);

//  auto temp = generateLanguageParser(LanguageType::JAVA);
//  assert(temp.first);
//  auto gen = temp.second;

  testFileName = "../src/python/tests/test.py";
//  testFileName = "../src/java/tests/test.java";
  if (!testFileName.empty()) {
    std::ifstream ifs(testFileName);
    std::string testInput((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
    try {
      auto testOutput = gen.run(testInput);
      std::cout << "Parsing result: " << testOutput << std::endl;
    } catch (peg_parser::SyntaxError &e) {
      std::cout << "Syntax error when parsing " << e.syntax->rule->name << std::endl;
    }
  }
  return 0;
}
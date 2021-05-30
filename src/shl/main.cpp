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
  testFileName = "../src/python/tests/test.py";
  std::ifstream ifs(testFileName);
  std::string testInput((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
//  auto result = generateParserFromSHL(shlFileName);

  /*
  auto result = generateParserFromSHL("../src/python/python3.shl");
  auto gen = result.second;



  std::vector<int> indentDepth;
  std::vector<std::pair<long, long> > records;
  gen["InitBlocks"] << "''" << [&](auto&) -> bool {
    indentDepth.resize(0);
    return true;
  } >> [](auto) { return "InitBlocks"; };
  gen["SameIndentation"] << "Indentation" << [&](auto &s) -> bool {
    return s->length() == indentDepth.back();
  } >> [](auto) { return "SameIndentation"; };
  gen["SameIndentation"]->cacheable = false;

  gen["DeepIndentation"] << "Indentation" << [&](auto &s) -> bool {
    return s->length() > indentDepth.back();
  } >> [](auto) { return "DeepIndentation"; };
  gen["DeepIndentation"]->cacheable = false;

  gen["EnterBlock"] << "Indentation" << [&](auto &s) -> bool {
    if (indentDepth.empty() || s->length() > indentDepth.back()) {
      indentDepth.push_back(s->length());
      return true;
    } else return false;
  } >> [](auto) { return "EnterBlock"; };
  gen["EnterBlock"]->cacheable = false;

  gen["Line"] << "SameIndentation Grammar '\n'" >> [](auto) { return "Line"; };
  gen.getRule("Line")->cacheable = false;

  gen["EmptyLine"] << "Indentation '\n'" >> [](auto) { return "EmptyLine"; };

  gen["ExitBlock"] << "''" << [&](auto&) -> bool {
    indentDepth.pop_back();
    return true;
  } >> [](auto) { return "ExitBlock"; };
  gen.getRule("ExitBlock")->cacheable = false;

  gen["Block"] << "&EnterBlock Line (EmptyLine | Block | Line)* &ExitBlock" >> [&](auto e) {
    for (auto x : e) x.evaluate();
//    blocks.push_back(Block{e.position(), e.length()});
//    std::cout << "position: from " << e.position() << " to " << e.position() + e.length() << std::endl;
    auto temp = getLineNumber(testInput, e.position(), e.length());
    records.push_back(temp);
//    std::cout << "line from " << temp.first << " to " << temp.second << std::endl;
    return "Block";
  };

  gen["Grammar"] << "(Heads | Statement | Expr)";

  gen["Start"] << "InitBlocks (ImportStatement '\n')* Block";
  gen.setStart(gen["Start"]);
  */




//  gen["Program"] << "Statement | Expr";
//  gen["Program"] << "DefHead";
//  gen.setStart(gen["Program"]);

  auto temp = generateLanguageParser(LanguageType::PYTHON);
  assert(temp.first);
  auto gen = temp.second;

  //  testFileName = "../src/java/tests/test.java";
  try {
    auto testOutput = gen.run(testInput);
    std::cout << "Parsing result: " << testOutput << std::endl;
    std::cout << blockRecords.size() << std::endl;
    for (auto x : blockRecords) {
      std::cout << x.first << ", " << x.second << std::endl;
      auto t = getLineNumber(testInput, x);
      std::cout << t.first << ", " << t.second << std::endl;
    }
  } catch (peg_parser::SyntaxError &e) {
    std::cout << "Syntax error when parsing " << e.syntax->rule->name << std::endl;
  }
  return 0;
}
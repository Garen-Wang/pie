//
// Created by garen on 5/23/21.
//
#include <peg_parser/generator.h>

#include <iostream>
#include <fstream>
#include <string>
#include <string_view>
#include <unordered_set>


typedef peg_parser::ParserGenerator<std::string> Parser;

class Colors {
private:
  std::unordered_set<std::string> colors;
public:
  std::string getExpr() {
    std::string expr = "(";
    for (const std::string& color : colors) {
      expr += "'" + color + "'|";
    }
    expr.pop_back();
    expr += ")";
    return expr;
  }
  // hope all string in `colors` are lowercase
  void append(const std::string& color) {
    colors.insert(color);
  }
};
Colors color;

int main() {
  color.append("black");
  color.append("grey");
  color.append("shallow_yellow");
  color.append("blue");
  color.append("cyan");
  color.append("purple");
  Parser g;
  g.setSeparator(g["Separators"] << "['\t''\n' ]");
  g["Identifier"] << "(([a-zA-Z] | '_') ([a-zA-Z0-9] | '_')*)" >> [](auto s) {
    return s.string();
  };
  g["GrammarExpr"] << "('\"' (!'\"' .)* '\"')" >> [](auto) {
    return "GrammarExpr";
  };
  g["Indent"] << "(['\t''\n' ]*)" >> [](auto) {
    return "Indent";
  };
  g["EmptyBlock"] << "('{' Indent '}')" >> [](auto) {
    return "EmptyBlock";
  };
  g["OnlyBlock"] << "('{' Indent Attr Indent '}')";
  g["equals"] << "(Indent '=' Indent)" >> [](auto) {
    return "equals";
  };
  g["MultiBlock"] << "('{' ['\t''\n' ]* ('$' (Index equals Attr ['\t''\n' ]*))* '}')" >> [](auto s) {
    for (int i = 0; i < s.size(); i += 3) {
      std::cout << "$" << s[i].evaluate() << " = ";
      s[i + 2].evaluate();
    }
    return "MultiBlock";
  };
  g["Block"] << "(EmptyBlock | OnlyBlock | MultiBlock)";
  g["Index"] << "(('0') | ([1-9][0-9]*))" >> [](auto s) {
    return s.string();
  };
  g["underlined"] << "'underlined'" >> [](auto) {
    return "underlined";
  };
  g["bold"] << "'bold'" >> [](auto) {
    return "bold";
  };
  g["italic"] << "'italic'" >> [](auto) {
    return "italic";
  };
  g["Color"] << color.getExpr() >> [](auto s) {
    return s.string();
  };
  g["Attr"] << "(((underlined | bold | italic) Indent)* Color)" >> [](auto s) {
//  g["Attr"] << "(bold? italic? underlined? color?)" >> [&](auto s) {
    for (int i = 0; i < s.size(); i += 2) {
      std::cout << s[i].evaluate() << " ";
    }
    std::cout << std::endl;
    return "Attr";
  };
  g["Comment"] << "('//' (!'\n' .)*)" >> [](auto) {
    return "Comment";
  };
  // integrated
  g["lparen"] << "('(')" >> [](auto) {
    return "lparen";
  };
  g["rparen"] << "('(')" >> [](auto) {
    return "rparen";
  };
  g["langle"] << "('<')" >> [](auto) {
    return "langle";
  };
  g["rangle"] << "('>')" >> [](auto) {
    return "rangle";
  };
  g["comma"] << "(',')" >> [](auto) {
    return "comma";
  };
  g["single_quote"] << "([\'])" >> [](auto) {
    return "single_quote";
  };
  g["double_quote"] << "('\"')" >> [](auto) {
    return "double_quote";
  };
  g["backslash"] << "('\\\\')" >> [](auto) {
    return "backslash";
  };
  g["single_equal"] << "('=')" >> [](auto) {
    return "single_equal";
  };
  g["double_equal"] << "('==')" >> [](auto) {
    return "double_equal";
  };
  g["newline"] << "('\n')" >> [](auto) {
    return "newline";
  };
  g["tab"] << "('\t')" >> [](auto) {
    return "tab";
  };
  g["space"] << "(' ')" >> [](auto) {
    return "space";
  };


  g["Grammar"] << "(Identifier ':' GrammarExpr Block)";
  g["Program"] << "((Grammar | Comment)*)";
  g["Test"] << "Identifier ':' GrammarExpr MultiBlock";
  g.setStart(g["Program"]);
//  g.setStart(g["Program"]);

  std::ifstream ifs("../src/shl/cpp_test.shl");
  std::string shlFile((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
//  std::cout << shlFile << std::endl;
  try {
    auto output = g.run(shlFile);
    std::cout << "Parsing result: " << output << std::endl;
  } catch (peg_parser::SyntaxError &e) {
    std::cout << "Syntax error when parsing " << e.syntax->rule->name << std::endl;
  }
  return 0;
}
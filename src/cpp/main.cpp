#include <peg_parser/generator.h>
#include <iostream>
#include <vector>

// TODO:
enum class LexType {

};

typedef std::vector<std::vector<LexType>> LexResult;

void io(peg_parser::ParserGenerator<std::string> &g) {
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

void config(peg_parser::ParserGenerator<std::string> &g) {
  // separator
  g.setSeparator(g["Separators"] << "[\t ]");

  // identifier
  g["Identifier"] << "([a-zA-Z] | '_') ([a-zA-Z0-9] | '_')*" >> [](auto) { return "Identifier"; };

  // comment
  g["Comment"] << "SingleLineComment | MultiLineComment";
  g["SingleLineComment"] << "'//' (!'\n' .)* '\n'" >> [](auto) { return "SingleLineComment"; };
  g["MultiLineComment"] << "'/*'  (!'*' .)* '*/' '\n'" >> [](auto) { return "MultiLineComment"; }; // DEBUG:

  // preprocessing
  g["Preprocessing"] << "(Include | Ifdef | Ifndef | Define | Pragma)";                      // tested
  g["Include"] << "Include_bracket | Include_quote" >> [](auto) { return "Include"; };       // tested
  g["Ifdef"] << "'#ifdef' Identifier '\n'" >> [](auto) { return "Ifdef"; };                  // tested
  g["Ifndef"] << "'#ifndef' Identifier '\n'" >> [](auto) { return "Ifndef"; };               // tested
  g["Define"] << "'#define' Identifier (Identifier)? '\n'" >> [](auto) { return "Define"; }; // tested
  g["Pragma"] << "'#pragma' (!'\n' !';' .)+ '\n'" >> [](auto) { return "Pragma"; };          // WARNING: roughly tested
  g["Include_bracket"] << "'#include' ' '* '<' ([a-zA-Z] | '.' | '/' | '_')+ '>' '\n'" >> [](auto) { return "Include_bracket"; };
  g["Include_quote"] << "'#include' ' '* '\"' ([a-zA-Z] | '.' | '/' | '_')+ '\"' '\n'" >> [](auto) { return "Include_quote"; };

  // function definition
  // g["Function"] << "('inline')? Type Identifier ArgumentList (';' | Block)" >> [](auto) { return "Function"; };
  g["Function"] << "('inline')? Type Identifier ArgumentList ';'" >> [](auto) { return "Function"; }; // tested

  // argument list (with lparen and rparent)
  // g["ArgumentList"] << "'(' Type Identifier? (',' Type Identifier?)* ')'" >> [](auto) { return "ArgumentList"; };
  g["ArgumentList"] << "'(' (Type Identifier? (',' Type Identifier?)*)? ')'" >> [](auto) { return "ArgumentList"; }; // tested

  // type
  g["Type"] << "PrimitiveType";
  g["PrimitiveType"] << " (t_const ' ')? (t_static ' ')?"
                        "(t_int | t_short | t_long | t_char | t_double | t_float | t_long_double"
                        "| t_signed_int | t_signed_short | t_signed_long | t_signed_char"
                        "| t_unsigned_int | t_unsigned_short | t_unsigned_long | t_unsigned_char"
                        "| t_auto | t_size_t)"
                        "(' '? ['*''&'])?"; // tested

  // primitive data types
  g["t_double"] << "'double'" >> [](auto) { return "double"; };
  g["t_float"] << "'float'" >> [](auto) { return "float"; };
  g["t_int"] << "'int'" >> [](auto) { return "int"; };
  g["t_short"] << "'short'" >> [](auto) { return "short"; };
  g["t_long"] << "'long'" >> [](auto) { return "long"; };
  g["t_char"] << "'char'" >> [](auto) { return "char"; };
  g["t_signed"] << "'signed'" >> [](auto) { return "signed"; };
  g["t_unsigned"] << "'unsigned'" >> [](auto) { return "unsigned"; };
  g["t_const"] << "'const'" >> [](auto) { return "const"; };
  g["t_static"] << "'static'" >> [](auto) { return "static"; };

  g["t_signed_short"] << "'signed short'" >> [](auto) { return "signed_short"; };
  g["t_signed_int"] << "'signed int'" >> [](auto) { return "signed_int"; };
  g["t_signed_long"] << "'signed long'" >> [](auto) { return "signed_long"; };
  g["t_signed_char"] << "'signed char'" >> [](auto) { return "signed_char"; };
  g["t_unsigned_short"] << "'unsigned short'" >> [](auto) { return "unsigned_short"; };
  g["t_unsigned_int"] << "'unsigned int'" >> [](auto) { return "unsigned_int"; };
  g["t_unsigned_long"] << "'unsigned long'" >> [](auto) { return "unsigned_long"; };
  g["t_unsigned_char"] << "'unsigned char'" >> [](auto) { return "unsigned_char"; };
  g["t_long_double"] << "'long double'" >> [](auto) { return "long double"; };

  g["t_auto"] << "'auto'" >> [](auto) { return "auto"; };
  g["t_size_t"] << "'std::'? 'size_t'" >> [](auto) { return "size_t"; };

  // block: supported both versions of coding style
  // g["Block"] << "('\n' '{' | '{' '\n') Statement* '}'" >> [](auto) { return "Block"; };
  // DEBUG: no statement inside block!!!
  g["Block"] << "'\n'? '{' Indent '\n'? '}'" >> [](auto) { return "Block"; };

  // statement
  g["Statement"] << "DeclarationStatement | ReturnStatement | LoopStatement | ControlStatement | Typedef | Class | Struct | Block" >> [](auto) { return "Statement"; };

  g["ReturnStatement"] << "'return' Expression ';'" >> [](auto) { return "ReturnStatement"; };
  g["LoopStatement"] << "WhileLoop | DoWhileLoop | ForLoop"; // tested
  g["WhileLoop"] << "'while' Indent '(' Condition ')' Indent (('\n'? Statement? ';') | Block)"
  >> [](auto) {
    return "WhileLoop";
  }; // tested

  g["DoWhileLoop"] << "'do' Indent (('\n'? Statement? ';') | Block)"
                      "['\n' ]* 'while' Indent '(' Condition ')' ' '? ';'"
  >> [](auto) {
    return "DoWhileLoop";
  }; // tested

  g["ForLoop"] << "'for' Indent '(' Declaration ';' ' '? Condition? ';' ' '? Expression? ')' Indent (('\n'? Indent Statement? ';') | Block)"
  >> [](auto) {
    return "ForLoop";
  }; // tested

  // TODO
  // g["Declaration"] << "Type Identifier ('=' (Identifier | MemberVariable | MemberMethod | Literal))?" >> [](auto) { return "Declaration"; };
  g["Declaration"] << "Type Identifier Indent '=' Indent '-'? [0-9]+" >> [](auto) { return "Declaration"; };

  // TODO
  g["MemberVariable"];
  // TODO
  g["MemberMethod"];
  // TODO
  g["Literal"];

  // condition, not in parentheses
  // TODO
  g["Condition"] << "Expression" >> [](auto) { return "Condition"; };

  // expression with built-in operators
  // TODO
  g["Expression"] << "'a'" >> [](auto) { return "Expression"; };

  g["Indent"] << "(' ')*" >> [](auto) { return "Indent"; };

  // g["ControlStatement"] << "'if' '(' Condition ')' ('\n'? Statement? ';' | Block) ('else' 'if' '(' Condition ')' ('\n'? Statement? ';' | Block))* ('else' ('\n'? Statement? ';' | Block))?";
  g["ControlStatement"] << "'if' Indent '(' Condition ')' (('\n'? Indent Statement? ';') | Block)"
                           " (['\n' ]* 'else if' Indent '(' Condition ')' (('\n'? Indent Statement? ';') | Block))*"
                           " (['\n' ]* 'else' Indent (('\n'? Indent Statement? ';') | Block))+"
  >> [](auto) {
    return "ControlStatement";
  }; // tested

  g["Typedef"] << "'typedef' Type Identifier ';'" >> [](auto) { return "Typedef"; }; // tested

  // TODO: do not support declaration after class block
  g["Class"] << "'class' Identifier ClassBlock? ';'" >> [](auto) { return "Class"; }; // tested
  g["Struct"] << "'struct' Identifier ClassBlock? ';'" >> [](auto) { return "Struct"; }; // tested

  // TODO: allow *private*, *protected* and *public*
  g["ClassBlock"] << "Block" >> [](auto) { return "ClassBlock"; };

  // starter
  g.setStart(g["Program"] << "LoopStatement '\n'");
}
peg_parser::ParserGenerator<std::string> generate() {
  peg_parser::ParserGenerator<std::string> g;
  config(g);
  return g;
}

int main() {
  auto g = generate();
  io(g);
  return 0;
}

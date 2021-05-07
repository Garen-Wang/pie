#include <string>
#include <peg_parser/generator.h>

#include "table.cpp"

// TODO: pass result in the from of LexemeResult
enum class LexemeType {
  OrdinaryCharacter,
  BuiltInOperator,
  StringLiteral,
  Comment,
  Keywords,
};
typedef std::vector<std::vector<LexemeType>> LexResult;

VariableTable variableTable;
TypeTable typeTable;

void initTypeTable(Parser &g) {
  typeTable.append("double", 1, g);
  typeTable.append("float", 1, g);
  typeTable.append("long double", 1, g);
  typeTable.append("char", 1, g);
  typeTable.append("int", 1, g);
  typeTable.append("short", 1, g);
  typeTable.append("long", 1, g);
  typeTable.append("long long", 1, g);
  typeTable.append("signed", 1, g);
  typeTable.append("signed char", 1, g);
  typeTable.append("signed int", 1, g);
  typeTable.append("signed short", 1, g);
  typeTable.append("signed long", 1, g);
  typeTable.append("signed long long", 1, g);
  typeTable.append("unsigned", 1, g);
  typeTable.append("unsigned char", 1, g);
  typeTable.append("unsigned int", 1, g);
  typeTable.append("unsigned short", 1, g);
  typeTable.append("unsigned long", 1, g);
  typeTable.append("unsigned long long", 1, g);
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

void detail(Parser &g) {
  initTypeTable(g);
  // comment
  g["SingleLineComment"] << "'//' (!'\n' .)* '\n'" >> [](auto) { return "SingleLineComment"; };
  g["MultiLineComment"] << "'/*'  (!'*' .)* '*/' '\n'" >> [](auto) { return "MultiLineComment"; };

  g["Include"] << "Include_bracket | Include_quote" >> [](auto) { return "Include"; };       // tested
  g["Ifdef"] << "'#ifdef' Identifier '\n'" >> [](auto) { return "Ifdef"; };                  // tested
  g["Ifndef"] << "'#ifndef' Identifier '\n'" >> [](auto) { return "Ifndef"; };               // tested
  g["Define"] << "'#define' Identifier (Identifier)? '\n'" >> [](auto) { return "Define"; }; // tested
  g["Pragma"] << "'#pragma' (!'\n' !';' .)+ '\n'" >> [](auto) { return "Pragma"; };          // WARNING: roughly tested
  g["Include_bracket"] << "'#include' ' '* '<' ([a-zA-Z] | '.' | '/' | '_')+ '>' '\n'" >> [](auto) { return "Include_bracket"; };
  g["Include_quote"] << "'#include' ' '* '\"' ([a-zA-Z] | '.' | '/' | '_')+ '\"' '\n'" >> [](auto) { return "Include_quote"; };

  // TODO: typedef will generate new type
  // TODO: store newly generated type to typeTable
  g["Typedef"] << "'typedef' Type Identifier ';'" >> [](auto) { return "Typedef"; }; // tested

  g["k_const"] << "'const'" >> [](auto) { return "const"; };
  g["k_static"] << "'static'" >> [](auto) { return "static"; };

  // TODO: allow *private*, *protected* and *public*
  g["ClassBlock"] << "Block" >> [](auto) { return "ClassBlock"; };

  g["t_auto"] << "'auto'" >> [](auto) { return "auto"; };
  g["t_size_t"] << "'std::'? 'size_t'" >> [](auto) { return "size_t"; };

  g["ReturnStatement"] << "'return' Expression ';'" >> [](auto) { return "ReturnStatement"; };

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

  g["StringLiteral"] << "'\"' (!'\"' .)* '\"'" >> [](auto) { return "StringLiteral"; };

  // TODO: char literal has bug here so temporarily deprecated
  // g["CharLiteral"] << "'\''" >> [](auto) { return "CharLiteral"; };
 
  g["DecLiteral"] << "'-'? ('0' | ([1-9][0-9]*)) ('.' [0-9]+)?" >> [](auto) { return "DecLiteral"; }; // including fraction, tested
  g["HexLiteral"] << "'-'? '0x' ('0' | ([1-9a-fA-F][0-9a-fA-F]*))" >> [](auto) { return "HexLiteral"; }; // tested
  g["OctLiteral"] << "'-'? '0' ('0' | ([1-7][0-7]*))" >> [](auto) { return "OctLiteral"; }; // tested
}

void config(Parser &g) {
  detail(g);
  // separator
  g.setSeparator(g["Separators"] << "[\t ]");

  // identifier
  g["Identifier"] << "([a-zA-Z] | '_') ([a-zA-Z0-9] | '_')*" >> [](auto) { return "Identifier"; };

  // comment
  g["Comment"] << "SingleLineComment | MultiLineComment";

  // preprocessing
  g["Preprocessing"] << "(Include | Ifdef | Ifndef | Define | Pragma)"; // tested

  // function declaration and definition
  g["Function"] << "('inline')? Type Identifier ArgumentList (';' | Block)" >> [](auto) { return "Function"; }; // tested

  // argument list (with lparen and rparent)
  g["ArgumentList"] << "'(' (Type Argument? (',' Type Argument?)*)? ')'" >> [](auto) { return "ArgumentList"; }; // tested
  g["Argument"] << "Identifier";

  g["FunctionCall"] << "Identifier ParameterList" >> [](auto) { return "FunctionCall"; }; // tested
  // TODO: do not support literal as parameter
  g["ParameterList"] << "'('( Parameter (',' Parameter)* )? ')'" >> [](auto) { return "ParameterList"; }; // tested
  g["Parameter"] << "Expression";

  // type
  // TODO: waiting to expand type, not only the primitive data types
  g["Type"] << "PrimitiveType";
  // WARNING: potential bugs like auto&, auto*
  g["PrimitiveType"] << " (k_const)? (k_static)?"
                        "(t_long_long | t_long_double | t_int | t_short | t_long | t_char | t_double | t_float"
                        "| t_signed_long_long | t_signed_int | t_signed_short | t_signed_long | t_signed_char"
                        "| t_unsigned_long_long | t_unsigned_int | t_unsigned_short | t_unsigned_long | t_unsigned_char"
                        "| t_auto | t_size_t)"
                        "(' '? ['*''&'])?"; // tested


  // block: supported both versions of coding style
  // DEBUG: no statement inside block!!!
  g["Block"] << "'\n'? '{' Indent '\n'? '}'" >> [](auto) { return "Block"; };

  // TODO: do not support declaration after class block
  g["Class"] << "'class' Identifier ClassBlock? ';'" >> [](auto) { return "Class"; }; // tested
  g["Struct"] << "'struct' Identifier ClassBlock? ';'" >> [](auto) { return "Struct"; }; // tested

  // statement
  g["Statement"] << "DeclarationStatement | ReturnStatement | LoopStatement | ControlStatement | Typedef | Class | Struct | Block" >> [](auto) { return "Statement"; };


  g["LoopStatement"] << "WhileLoop | DoWhileLoop | ForLoop"; // tested

  // g["Declaration"] << "Type Identifier ('=' (Identifier | MemberVariable | MemberMethod | Literal))?" >> [](auto) { return "Declaration"; };
  // declaration is where new variables are created
  // TODO: interact with variableTable in action
  g["Declaration"] << "Type Identifier Indent '=' Indent Expression" >> [&](auto s) {
    // DEBUG
    std::cout << s[1].string() << std::endl;

    variableTable.append(s[1].string(), 3, g);
    std::cout << "in Declaration" << std::endl;
    variableTable.debug();
    return "Declaration";
  };
  g["DeclarationStatement"] << "Declaration ';'";

  // TODO: add CharLiteral into Literal
  g["Literal"] << "StringLiteral | HexLiteral | OctLiteral | DecLiteral ";

  // condition, not in parentheses
  // TODO
  g["Condition"] << "Expression" >> [](auto) { return "Condition"; };

  // expression with built-in operators
  // expression without semicolon
  // TODO: simplification that expression = 'a'
  g["Expression"] << "'a'" >> [](auto) { return "Expression"; };

  g["Indent"] << "(' ')*" >> [](auto) { return "Indent"; };

  // g["ControlStatement"] << "'if' '(' Condition ')' ('\n'? Statement? ';' | Block) ('else' 'if' '(' Condition ')' ('\n'? Statement? ';' | Block))* ('else' ('\n'? Statement? ';' | Block))?";
  g["ControlStatement"] << "'if' Indent '(' Condition ')' (('\n'? Indent Statement? ';') | Block)"
                           " (['\n' ]* 'else if' Indent '(' Condition ')' (('\n'? Indent Statement? ';') | Block))*"
                           " (['\n' ]* 'else' Indent (('\n'? Indent Statement? ';') | Block))+"
  >> [](auto) {
    return "ControlStatement";
  }; // tested


  // TODO: check whether the identifier is in variableTable through action
  g["DefinedVariable"] << "Identifier" << [&](auto s) -> bool {
    auto name = s->inner[0]->string();

    // DEBUG
    std::cout << "in DefinedVariable" << std::endl;
    std::cout << name << std::endl;
    variableTable.debug();

    return variableTable.check(name);
  } >> [](auto) { return "DefinedVariable"; };
  /**
   * some problems here
   * if we want to record declared variable, we should parse each statement one by one
   * rather than take the whole file into parsing
   **/

  // starter
  g.setStart(g["Program"] << "DeclarationStatement '\n'");
  // int a = a;
}

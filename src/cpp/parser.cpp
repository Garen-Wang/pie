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
  g["SingleLineComment"] << "('//' (!'\n' .)*)" >> [](auto) { return "SingleLineComment"; };
  g["MultiLineComment"] << "('/*'  (!'*' .)* '*/')" >> [](auto) { return "MultiLineComment"; };

  g["Include"] << "(Include_bracket | Include_quote)" >> [](auto) { return "Include"; };
  g["Ifdef"] << "('#ifdef' Indent Identifier)" >> [](auto) { return "Ifdef"; };
  g["Ifndef"] << "('#ifndef' Indent Identifier)" >> [](auto) { return "Ifndef"; };
  g["Define"] << "('#define' Indent Identifier (Indent Identifier)?)" >> [](auto) { return "Define"; };
  g["Pragma"] << "('#pragma' Indent (!'\n' !';' .)+)" >> [](auto) { return "Pragma"; };
  g["Include_bracket"] << "('#include' Indent '<' ([a-zA-Z] | '.' | '/' | '_')+ '>')" >> [](auto) { return "Include_bracket"; };
  g["Include_quote"] << "('#include' Indent '\"' ([a-zA-Z] | '.' | '/' | '_')+ '\"')" >> [](auto) { return "Include_quote"; };

  // TODO: typedef will generate new type
  // TODO: store newly generated type to typeTable
  g["Typedef"] << "('typedef' Type Identifier ';')" >> [](auto) { return "Typedef"; }; // tested

  g["k_const"] << "('const')" >> [](auto) { return "const"; };
  g["k_static"] << "('static')" >> [](auto) { return "static"; };

  // argument list (with lparen and rparent)
  g["ArgumentList"] << "('(' (Type Argument? (',' Type Argument?)*)? ')')" >> [](auto) { return "ArgumentList"; };
  g["Argument"] << "(Identifier)";

  g["ParameterList"] << "('('( Parameter (',' Parameter)* )? ')')" >> [](auto) { return "ParameterList"; };
  // g["Parameter"] << "(Expr_Level15)"; // in expression test

  g["t_auto"] << "('auto')" >> [](auto) { return "auto"; };
  g["t_size_t"] << "('std::'? 'size_t')" >> [](auto) { return "size_t"; };

  // TODO: These three types should be created dynamically 
  g["PrimitiveType"] << "( (k_const Indent)? (k_static Indent)?"
                        "( t_long_long | t_long_double | t_int | t_short | t_long | t_char | t_double | t_float"
                        "| t_signed_long_long | t_signed_int | t_signed_short | t_signed_long | t_signed_char"
                        "| t_unsigned_long_long | t_unsigned_int | t_unsigned_short | t_unsigned_long | t_unsigned_char)"
                        "(' '? ['*''&'])? )";
  g["STLType"] << "(('std::')? t_size_t)";
  g["AutoType"] << "(t_auto (' '? '&')?)";

  g["ReturnStatement"] << "('return' Indent Expr ';')" >> [](auto) { return "ReturnStatement"; };

  g["WhileLoop"] << "('while' Indent '(' Condition ')' Indent (('\n'? Statement? ';') | Block))"
  >> [](auto) {
    return "WhileLoop";
  }; // tested
  g["DoWhileLoop"] << "('do' Indent (('\n'? Statement? ';') | Block)"
                      "['\n' ]* 'while' Indent '(' Condition ')' ' '? ';')"
  >> [](auto) {
    return "DoWhileLoop";
  }; // tested
  g["ForLoop"] << "('for' Indent '(' Declaration? ';' ' '? Condition? ';' ' '? Expr? ')' Indent (('\n'? Indent Statement? ';') | Block))"
  >> [](auto) {
    return "ForLoop";
  }; // tested

  g["StringLiteral"] << "('\"' (!'\"' .)* '\"')" >> [](auto) { return "StringLiteral"; };
  // TODO: potential bug, cannot recognize '\n', '\b', etc.
  g["CharLiteral"] << "([\'] ((!'\n' .) | (['\\']) ([a-z])) [\'])" >> [](auto) { return "CharLiteral"; };

  // TODO: char literal has bug here so temporarily deprecated
  // g["CharLiteral"] << "('\'')" >> [](auto) { return "CharLiteral"; };
 
  g["DecLiteral"] << "('-'? ('0' | ([1-9][0-9]*)) ('.' [0-9]+)?)" >> [](auto) { return "DecLiteral"; }; // including fraction, tested
  g["HexLiteral"] << "('-'? '0x' ('0' | ([1-9a-fA-F][0-9a-fA-F]*)))" >> [](auto) { return "HexLiteral"; }; // tested
  g["OctLiteral"] << "('-'? '0' ('0' | ([1-7][0-7]*)))" >> [](auto) { return "OctLiteral"; }; // tested
}

void config(Parser &g) {
  detail(g);
  // separator
  g.setSeparator(g["Separators"] << "[\t ]");

  // identifier
  g["Identifier"] << "([a-zA-Z] | '_') ([a-zA-Z0-9] | '_')*" >> [](auto) { return "Identifier"; }; // tested twice

  // comment
  g["Comment"] << "(SingleLineComment | MultiLineComment)"; // tested twice

  // preprocessing
  g["Preprocessing"] << "(Include | Ifdef | Ifndef | Define | Pragma)"; // tested twice

  // function declaration and definition
  g["Function"] << "(('inline')? Type Identifier ArgumentList (';' | Block))" >> [](auto) { return "Function"; }; // tested twice

  g["FunctionCall"] << "(Identifier ParameterList)" >> [](auto) { return "FunctionCall"; }; // tested twice

  // type
  g["Type"] << "(PrimitiveType | STLType | AutoType)"; // tested twice

  // block: supported both versions of coding style
  // DEBUG: no statement inside block now!!!
  g["Block"] << "('\n'? '{' Indent '\n'? '}')" >> [](auto) { return "Block"; };

  // TODO: allow *private*, *protected* and *public*
  g["ClassBlock"] << "(Block)" >> [](auto) { return "ClassBlock"; };

  // TODO: do not support declaration after class block
  g["Class"] << "('class' Identifier ClassBlock? ';')" >> [](auto) { return "Class"; }; // tested
  g["Struct"] << "('struct' Identifier ClassBlock? ';')" >> [](auto) { return "Struct"; }; // tested

  // statement
  g["Statement"] << "(DeclarationStatement | ReturnStatement | LoopStatement | ControlStatement | Typedef | Class | Struct | Block)"; // tested twice

  g["LoopStatement"] << "(WhileLoop | DoWhileLoop | ForLoop)"; // tested twice

  // g["Declaration"] << "Type Identifier ('=' (Identifier | MemberVariable | MemberMethod | Literal))?" >> [](auto) { return "Declaration"; };
  // declaration is where new variables are created
  // TODO: interact with variableTable in action
  g["Declaration"] << "(Type Identifier Indent '=' Indent Expr)" >> [&](auto s) {
    // DEBUG
    std::cout << s[1].string() << std::endl;

    variableTable.append(s[1].string(), 3, g);
    std::cout << "in Declaration" << std::endl;
    variableTable.debug();
    return "Declaration";
  };
  g["DeclarationStatement"] << "(Declaration ';')";

  // condition, not in parentheses
  // TODO
  g["Condition"] << "(Expr)" >> [](auto) { return "Condition"; };

  // expression with built-in operators
  // expression without semicolon
  // TODO: simplification that expression = 'a'
  // WARNING: deprecated because it is substituded now
  g["Expr"] << "('a')" >> [](auto) { return "Expr"; };

  g["Indent"] << "((' ')*)" >> [](auto) { return "Indent"; };

  // g["ControlStatement"] << "'if' '(' Condition ')' ('\n'? Statement? ';' | Block) ('else' 'if' '(' Condition ')' ('\n'? Statement? ';' | Block))* ('else' ('\n'? Statement? ';' | Block))?";
  g["ControlStatement"] << "('if' Indent '(' Condition ')' (('\n'? Indent Statement? ';') | Block)"
                           " (['\n' ]* 'else if' Indent '(' Condition ')' (('\n'? Indent Statement? ';') | Block))*"
                           " (['\n' ]* 'else' Indent (('\n'? Indent Statement? ';') | Block))+)"
  >> [](auto) {
    return "ControlStatement";
  }; // tested


  // TODO: check whether the identifier is in variableTable through action
  g["DefinedVariable"] << "(Identifier)" << [&](auto s) -> bool {
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

  g["Literal"] << "(StringLiteral | CharLiteral | HexLiteral | OctLiteral | DecLiteral)";
  g["QualifiedConstant"] << "((Identifier '::')* Identifier)" >> [](auto) { return "QualifiedConstant"; };
  g["Expr_Level16"] << "(Literal | QualifiedConstant)";

  g["Indexing"] << "(QualifiedConstant '[' Expr_Level15 ']')" >> [](auto) { return "Indexing"; };
  g["Parameter"] << "(Expr_Level15)"; // overload line 85
  g["Expr_Level15"] << "(Indexing | FunctionCall | Expr_Level16)";

  g["PostfixIncrement"] << "(Expr_Level14 '++')" >> [](auto) { return "PostfixIncrement"; };
  g["PostfixDecrement"] << "(Expr_Level14 '--')" >> [](auto) { return "PostfixDecrement"; };

  g["StructureProjection"] << "(Expr_Level14 ('.' | '->') Expr_Level14)" >> [](auto) { return "StructureProjection"; };
  g["Expr_Level14"] << "(StructureProjection | PostfixIncrement | PostfixDecrement | Expr_Level15)";

  g["PrefixIncrement"] << "('++' Expr_Level14)" >> [](auto) { return "PrefixIncrement"; };
  g["PrefixDecrement"] << "('--' Expr_Level14)" >> [](auto) { return "PrefixDecrement"; };

  g["Dereference"] << "('*' Expr_Level13)" >> [](auto) { return "Dereference"; };
  g["Negation"] << "('!' Expr_Level13)" >> [](auto) { return "Negation"; };

  g["Expr_Level13"] << "(PrefixIncrement | PrefixDecrement | Dereference | Negation | Expr_Level14)";

  g["Expr"] << "(Expr_Level13)";

  // starter
  g.setStart(g["Program"] << "Expr '\n'");
  // int a = a;
}

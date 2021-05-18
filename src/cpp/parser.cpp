#include <iostream>
#include <peg_parser/generator.h>

#include "table.cpp"

VariableTable variableTable;
TypeTable typeTable;

void initTypeTable(Parser &g) {
  // primitive data type
  typeTable.append("unsigned", 1, g);
  typeTable.append("unsigned char", 1, g);
  typeTable.append("unsigned int", 1, g);
  typeTable.append("unsigned short", 1, g);
  typeTable.append("unsigned long", 1, g);
  typeTable.append("signed", 1, g);
  typeTable.append("signed char", 1, g);
  typeTable.append("signed int", 1, g);
  typeTable.append("signed short", 1, g);
  typeTable.append("signed long", 1, g);
  typeTable.append("double", 1, g);
  typeTable.append("float", 1, g);
  typeTable.append("long double", 1, g);
  typeTable.append("long long", 1, g);
  typeTable.append("char", 1, g);
  typeTable.append("int", 1, g);
  typeTable.append("short", 1, g);
  typeTable.append("long", 1, g);
  typeTable.append("unsigned long long", 1, g);
  typeTable.append("signed long long", 1, g);
  typeTable.append("bool", 1, g);

  // STL
  typeTable.append("string", 2, g);
  typeTable.append("size_t", 2, g);

  typeTable.append("vector", 5, g);
  typeTable.append("map", 5, g);
  typeTable.append("set", 5, g);
  typeTable.append("unordered_map", 5, g);
  typeTable.append("unordered_set", 5, g);

  // subtype in STL
  typeTable.append("size_type", 4, g);
  typeTable.append("iterator", 4, g);

  // user-defined data types
  typeTable.append("Student_info", 3, g);

  // to be continued

  typeTable.update(g);

  // variableTable.append("x", 1, g);
  // variableTable.update(g);
}

void detail(Parser &g) {
  initTypeTable(g);
  g["SingleLineComment"] << "('//' (!'\n' .)*)" >> [](auto) { return "SingleLineComment"; };
  g["MultiLineComment"] << "('/*'  (!'*' .)* '*/')" >> [](auto) { return "MultiLineComment"; };

  g["Include"] << "(Include_bracket | Include_quote)" >> [](auto) { return "Include"; };
  g["Ifdef"] << "('#ifdef' Indent Identifier)" >> [](auto) { return "Ifdef"; };
  g["Ifndef"] << "('#ifndef' Indent Identifier)" >> [](auto) { return "Ifndef"; };
  g["Else"] << "('#else')" >> [](auto) { return "Else"; };
  g["Endif"] << "('#endif')" >> [](auto) { return "Endif"; };
  g["Define"] << "('#define' Indent Identifier (Indent Identifier)?)" >> [](auto) { return "Define"; };
  g["Pragma"] << "('#pragma' Indent (!'\n' !';' .)+)" >> [](auto) { return "Pragma"; };
  g["Include_bracket"] << "('#include' Indent '<' ([a-zA-Z] | '.' | '/' | '_')+ '>')" >> [](auto) { return "Include_bracket"; };
  g["Include_quote"] << "('#include' Indent '\"' ([a-zA-Z] | '.' | '/' | '_')+ '\"')" >> [](auto) { return "Include_quote"; };
  g["Using"] << "('using' Indent ((Identifier Indent '=' Indent QualifiedConstant) | ('namespace' Indent Identifier) | QualifiedConstant) ';')" >> [](auto) { return "using"; };

  g["Typedef"] << "('typedef' Type Identifier ';')" >> [&](auto e) {
    typeTable.append(e[1].string(), 3, g);
    // DEBUG
    std::cout << e[1].string() << " is added into typeTable." << std::endl;
    return "Typedef";
  };

  g["k_const"] << "('const')" >> [](auto) { return "const"; };
  g["k_static"] << "('static')" >> [](auto) { return "static"; };

//  g["ArgumentType"] << "( (k_const Indent)? (k_static Indent)? Typename (' '? ['*''&'])?)" >> [&](auto s) {
//    return s[s.size() - 1].string();
//  };
  g["ArgumentList"] << "'(' (Type Indent Argument (', ' Type Indent Argument)*)? ')'"
    >> [&](auto s) {
      for (int i = 0; i < s.size(); i += 3) {
        // DEBUG
        std::cout << s[i].evaluate() << std::endl;
      }
      return "ArgumentList"; 
  };
  g["Argument"] << "(Identifier)";

  g["ParameterList"] << "('('( Parameter (',' Parameter)* )? ')')" >> [](auto) { return "ParameterList"; };
  // g["Parameter"] << "(Expr_Level15)"; // in expression test

  g["TemplateList"] << "'<' ManualType (',' Indent ManualType)* '>'" >> [](auto) { return "TemplateList"; }; // tested

  g["AutoType"] << "('auto' (' '? '&')?)" >> [](auto) { return "AutoType"; };
  // ManualType will be updated once *TypeTable.update* is invoked

  g["ReturnStatement"] << "('return' Indent Expr ';')" >> [](auto) { return "ReturnStatement"; };
  g["WhileLoop"] << "('while' Indent '(' Condition ')' ['\n''\t' ]* ((Statement | ';') | Block))"
  >> [](auto) {
    return "WhileLoop";
  }; // tested
  g["DoWhileLoop"] << "('do' ['\n''\t' ]* ((Statement | ';') | Block)"
                      "['\n''\t' ]* 'while' Indent '(' Condition ')' ' '? ';')"
  >> [](auto) {
    return "DoWhileLoop";
  }; // tested
  g["ForLoop"] << "('for' Indent '(' Declaration? ';' ' '? Condition? ';' ' '? Expr? ')' ['\n''\t' ]* ((Statement | ';') | Block))"
  >> [](auto) {
    return "ForLoop";
  }; // tested

  // StringLiteral now support string concat in neighboring lines
  g["StringLiteral"] << "('\"' (!'\"' !'\n' .)* '\"' ('\n' Indent '\"' (!'\"' !'\n' .)* '\"')*)" >> [](auto) { return "StringLiteral"; };
  // TODO: potential bug, cannot recognize '\n', '\b', etc.
  g["CharLiteral"] << "([\'] ((!'\n' .) | (['\\']) ([a-z])) [\'])" >> [](auto) { return "CharLiteral"; };
 
  // DexLiteral include fraction, scientific notation, etc. tested twice
  g["DecLiteral"] << "('-'? ('0' | ([1-9][0-9]*)) ('.' [0-9]+)? ([eE] ('+' | '-')? ('0' | ([1-9][0-9]*)))?)" >> [](auto) { return "DecLiteral"; };
  g["HexLiteral"] << "('-'? '0x' ('0' | ([1-9a-fA-F][0-9a-fA-F]*)))" >> [](auto) { return "HexLiteral"; }; // tested
  g["OctLiteral"] << "('-'? '0' ('0' | ([1-7][0-7]*)))" >> [](auto) { return "OctLiteral"; }; // tested
  g["BoolLiteral"] << "('true' | 'false')" >> [](auto) { return "BoolLiteral"; };
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
  g["Preprocessing"] << "(Include | Ifdef | Ifndef | Else | Endif | Define | Pragma)"; // tested twice

  // function declaration and definition
  g["Function"] << "(('inline')? Type Identifier ArgumentList ['\n''\t' ]* (';' | Block))" >> [](auto) { return "Function"; }; // tested twice

  g["FunctionCall"] << "(Identifier ParameterList)" >> [](auto) { return "FunctionCall"; }; // tested twice

  // type
  g["Type"] << "(ManualType | AutoType)"; // tested twice

  // TODO: do not support declaration after class block
  g["Class"] << "('class' Identifier ClassBlock? ';')" >> [](auto) { return "Class"; }; // tested
  g["Struct"] << "('struct' Identifier ClassBlock? ';')" >> [](auto) { return "Struct"; }; // tested

  // statement
  g["Statement"] << "(DeclarationStatement | ReturnStatement | LoopStatement | IfStatement | ExprStatement | Typedef | Class | Struct | Block | Using)"; // tested twice

  g["LoopStatement"] << "(WhileLoop | DoWhileLoop | ForLoop)"; // tested twice
  g["ExprStatement"] << "(ExprList ';')";

  // g["Declaration"] << "Type Identifier ('=' (Identifier | MemberVariable | MemberMethod | Literal))?" >> [](auto) { return "Declaration"; };
  // declaration is where new variables are created
  // TODO: interact with variableTable in action
  g["Typename"] << "([a-zA-Z'_'':']+)" >> [&](auto s) { return "Typename"; };
  g["Declaration"] << "(Typename TemplateList? Identifier (Indent '=' Indent Expr)? (',' Indent Identifier (Indent '=' Indent Expr)?)*)" >> [&](auto s) {
    variableTable.append(s[0].string(), 3, g);
    // DEBUG
    std::cout << s[0].string() << " in Declaration" << std::endl;
    typeTable.append(s[0].string(), 0, g);
    return "Declaration";
  };
  g["DeclarationStatement"] << "(Declaration ';')";

  // condition, not in parentheses
  g["Condition"] << "(Expr)" >> [](auto) { return "Condition"; };

  g["Indent"] << "([' ''\t']*)" >> [](auto) { return "Indent"; };

  // rewritten and renamed, originated by *ControlStatement*
  g["IfStatement"] << "(('if' Indent '(' Condition ')' ['\n''\t' ]* (';' | Statement | Block))"
                 "(['\n''\t' ]* 'else if' Indent '(' Condition ')' ['\n''\t' ]* (';' | Statement | Block))*"
                 "(['\n''\t' ]* 'else' ['\n''\t' ]* (';' | Statement | Block))?)"
    >> [](auto) { return "IfStatement"; };

  // TODO: check whether the identifier is in variableTable through action
  g["DefinedVariable"] << "(Identifier)" << [&](auto s) -> bool {
    auto name = s->inner[0]->string();

    // DEBUG
    // std::cout << "in DefinedVariable" << std::endl;
    // std::cout << name << std::endl;
    // variableTable.debug();

    return variableTable.check(name);
  } >> [](auto) { return "DefinedVariable"; };
  /**
   * some problems here
   * if we want to record declared variable, we should parse each statement one by one
   * rather than take the whole file into parsing
   **/

  g["Literal"] << "(StringLiteral | CharLiteral | HexLiteral | OctLiteral | DecLiteral | BoolLiteral)";
  g["QualifiedConstant"] << "((Identifier '::')* Identifier)" >> [](auto) { return "QualifiedConstant"; };
  // however, what if parentheses? what's the precedence of parentheses? what if nested parentheses?
  // solved here
  g["ParenExpr"] << "( '(' Expr_Level2 ')' )";
  g["Expr_Level16"] << "(Literal | QualifiedConstant | ParenExpr)";

  g["Indexing"] << "(QualifiedConstant '[' Indent Expr_Level10 Indent ']')" >> [](auto) { return "Indexing"; };
  g["Parameter"] << "(Expr_Level14)"; // overload line 85
  g["Expr_Level15"] << "(Indexing | FunctionCall | Expr_Level16)";

  g["PostfixIncrement"] << "(Expr_Level14 '++')" >> [](auto) { return "PostfixIncrement"; };
  g["PostfixDecrement"] << "(Expr_Level14 '--')" >> [](auto) { return "PostfixDecrement"; };

  g["StructureProjection"] << "(Expr_Level14 ('.' | '->') (Expr_Level14 | FunctionCall))" >> [](auto) { return "StructureProjection"; };
  g["Expr_Level14"] << "(StructureProjection | PostfixIncrement | PostfixDecrement | Expr_Level15)";

  g["PrefixIncrement"] << "('++' Expr_Level14)" >> [](auto) { return "PrefixIncrement"; };
  g["PrefixDecrement"] << "('--' Expr_Level14)" >> [](auto) { return "PrefixDecrement"; };

  g["Dereference"] << "('*' Expr_Level13)" >> [](auto) { return "Dereference"; };
  g["Negation"] << "('!' Expr_Level13)" >> [](auto) { return "Negation"; };

  g["Expr_Level13"] << "(PrefixIncrement | PrefixDecrement | Dereference | Negation | Expr_Level14)";

  g["Multiplication"] << "(Expr_Level12 Indent '*' Indent Expr_Level12)" >> [](auto) { return "Multiplication"; };
  g["Division"] << "(Expr_Level12 Indent '/' Indent Expr_Level12)" >> [](auto) { return "Division"; };
  g["Remainder"] << "(Expr_Level12 Indent '%' Indent Expr_Level12)" >> [](auto) { return "Remainder"; };
  g["Expr_Level12"] << "(Multiplication | Division | Remainder | Expr_Level13)";

  g["Addition"] << "(Expr_Level11 Indent '+' Indent Expr_Level11)" >> [](auto) { return "Addition"; };
  g["Substraction"] << "(Expr_Level11 Indent '-' Indent Expr_Level11)" >> [](auto) { return "Substraction"; };
  g["Expr_Level11"] << "(Addition | Substraction | Expr_Level12)";

  g["LeftShift"] << "(Expr_Level10 ['\n''\t' ]* '<<' ['\n''\t' ]* Expr_Level10)" >> [](auto) { return "LeftShift"; };
  g["RightShift"] << "(Expr_Level10 ['\n''\t' ]* '>>' ['\n''\t' ]* Expr_Level10)" >> [](auto) { return "RightShift"; };
  g["Expr_Level10"] << "(LeftShift | RightShift | Expr_Level11)";

  g["LE"] << "(Expr_Level9 Indent '<' Indent Expr_Level9)" >> [](auto) { return "le"; };
  g["LEQ"] << "(Expr_Level9 Indent '<=' Indent Expr_Level9)" >> [](auto) { return "leq"; };
  g["GE"] << "(Expr_Level9 Indent '>' Indent Expr_Level9)" >> [](auto) { return "ge"; };
  g["GEQ"] << "(Expr_Level9 Indent '>=' Indent Expr_Level9)" >> [](auto) { return "geq"; };
  g["Expr_Level9"] << "(LE | LEQ | GE | GEQ | Expr_Level10)";

  g["EQ"] << "(Expr_Level8 Indent '==' Indent Expr_Level8)" >> [](auto) { return "EQ"; };
  g["NEQ"] << "(Expr_Level8 Indent '!=' Indent Expr_Level8)" >> [](auto) { return "NEQ"; };
  g["Expr_Level8"] << "(EQ | NEQ | Expr_Level9)";

  g["LogicAND"] << "(Expr_Level4 Indent '&&' Indent Expr_Level4)" >> [](auto) { return "LogicAND"; };
  g["Expr_Level4"] << "(LogicAND | Expr_Level8)";

  g["LogicOR"] << "(Expr_Level3 Indent '||' Indent Expr_Level3)" >> [](auto) { return "LogicOR"; };
  g["Expr_Level3"] << "(LogicOR | Expr_Level4)";
  
  g["PureAssignment"] << "(Expr_Level2 Indent '=' Indent Expr_Level2)" >> [](auto) { return "PureAssignment"; };
  g["CompositeAssignment"] << "(Expr_Level2 Indent ([+*/%&|~^] | '-' | '<<' | '>>') '=' Indent Expr_Level2)" >> [](auto) { return "CompositeAssignment"; };
  g["Conditional"] << "Expr_Level2 ['\n''\t' ]* '?' ['\n''\t' ]* Expr_Level2 ['\n''\t' ]* ':' Indent Expr_Level2" >> [](auto) { return "Conditional"; };
  g["Expr_Level2"] << "(PureAssignment | CompositeAssignment | Conditional | Expr_Level3)";

  g["ThrowException"] << "('throw' Indent Expr_Level2)" >> [](auto) { return "ThrowException"; };
  g["Expr_Level1"] << "(ThrowException | Expr_Level2)";

  g["Expr"] << "(Expr_Level1)";

  g["ExprList"] << "(Expr (',' Indent Expr)*)";

  // block: supported both versions of coding style
  // DEBUG: no statement inside block now!!!
  g["Block"] << "('\n'? '{' Indent '\n'?  Program? '}')" >> [](auto) { return "Block"; };

  // TODO: allow *private*, *protected* and *public*
  g["ClassBlock"] << "(Block)" >> [](auto) { return "ClassBlock"; };

  // g["Program"] << "(Comment | (Preprocessing '\n') | (Function '\n') | (Statement Indent Comment? '\n'?) | '\n')*";

  // unused now
  g["TypeCheck"] << "" << [](auto) { return typeTable.check(); } >> [](auto) { return "TypeCheck"; };

   g["Program"] << "(((Indent Comment? '\n') | ((Preprocessing | Function) Indent SingleLineComment? '\n') | ((Statement Indent)+ SingleLineComment? '\n'))*)";
   g.setStart(g["Program"]);
//   g["Test"] << "'(' ArgumentType Argument (', ' ArgumentType Argument)* ')' '\n'";
//   g["Test"] << "IfStatement '\n'";
//   g.setStart(g["Test"]);
}

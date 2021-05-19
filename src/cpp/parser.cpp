#include <iostream>
#include <algorithm>
#include <peg_parser/generator.h>

#include "table.cpp"

VariableTable variableTable;
TypeTable typeTable;
std::string sourceCode;
std::vector<std::size_t> idPrefixSum;

int getLineNumber(std::size_t n) {
  return int(std::count(sourceCode.begin(), sourceCode.begin() + n, '\n') + 1);
}

void getPrefixSum(std::vector<std::size_t> &positions) {
  positions.push_back(0);
  std::size_t pos = sourceCode.find('\n', 0);
  while (pos != std::string::npos) {
    positions.push_back(pos);
    pos = sourceCode.find('\n', pos + 1);
  }
}

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

void addKeywords(Parser &g) {
  // markers
  g["m_SLCommentHead"] << "'//'";
  g["m_MLCommentHead"] << "'/*'";
  g["m_MLCommentTail"] << "'*/'";
  g["m_hash"] << "('#')";
  g["m_langle"] << "('<')";
  g["m_rangle"] << "('>')";
  g["m_singleQuotes"] << "([\'])";
  g["m_doubleQuotes"] << "('\"')";
  g["m_assign"] << "('=')";
  g["m_semicolon"] << "(';')";
  g["m_backslash"] << "('\\\\')";
  // keywords
  g["k_ifdef"] << "('ifdef')";
  g["k_ifndef"] << "('ifndef')";
  g["k_endif"] << "('endif')";
  g["k_define"] << "('define')";
  g["k_pragma"] << "('pragma')";
  g["k_include"] << "('include')";
  g["k_using"] << "('using')";
  g["k_namespace"] << "('namespace')";
  g["k_typedef"] << "('typedef')";
  g["k_const"] << "('const')";
  g["k_static"] << "('static')";
  g["k_return"] << "('return')";
  g["k_while"] << "('while')" >> [](auto) { return "while"; };
  g["k_do"] << "('do')";
  g["k_for"] << "('for')";
  g["k_if"] << "('if')";
  g["k_else_if"] << "('else if')";
  g["k_else"] << "('else')";
  g["k_auto"] << "('auto')";
  g["k_true"] << "('true')";
  g["k_false"] << "('false')";
  g["k_inline"] << "('inline')";
}

void detail(Parser &g) {
  initTypeTable(g);
  addKeywords(g);
  g["SingleLineComment"] << "(m_SLCommentHead (!'\n' .)*)" >> [](auto s) {
    std::cout << "This is SingleLineComment" << std::endl;
    int line = getLineNumber(s.position() + 2);
    int startPos = s.position() + 2 - idPrefixSum[line - 1];
    int endPos = s.position() + s.length() - idPrefixSum[line - 1];
    std::cout << "line " << line << std::endl;
    std::cout << "comment interval: from " << startPos << " to " << endPos << std::endl;
    // changeAttr(attr, s.position(), s.position() + s.length());
    return "SingleLineComment";
  };
  g["MultiLineComment"] << "(m_MLCommentHead (!'*/' .)* m_MLCommentTail)" >> [](auto s) {
    std::cout << "This is MultiLineComment" << std::endl;
    int startLine = getLineNumber(s[0].position() + 2);
    int endLine = getLineNumber(s[1].position() - 1);
    int startPos = s[0].position() + 2 - idPrefixSum[startLine - 1];
    int endPos = s[1].position() - 1 - idPrefixSum[endLine - 1];
    std::cout << "comment interval: from line " << startLine << " to " << endLine << std::endl;
    std::cout << "comment interval: from " << startPos << " to " << endPos << std::endl;
    // changeAttr(attr, s.position(), s.position() + s.length());
    // italic grey
    return "MultiLineComment";
  };

  g["Ifdef"] << "(m_hash k_ifdef Identifier)" >> [](auto s) {
    std::cout << "This is Ifdef" << std::endl;
    int line = getLineNumber(s[0].position());
    std::cout << "line " << line << std::endl;
    std::cout << "'#ifdef' is from " << s[0].position() - idPrefixSum[line - 1]
              << " to " << s[1].position() + s[1].length() - idPrefixSum[line - 1] << std::endl;
    std::cout << "Identifier is from " << s[3].position() - idPrefixSum[line - 1]
              << " to " << s[3].position() + s[3].length() - idPrefixSum[line - 1] << std::endl;
    // changeAttr(attr1, s[0].position(), s[1].position() + s[1].length());
    // shallow yellow
    // changeAttr(attr2, s[3].position(), s[3].position() + s[3].length());
    // bold cyan
    return "Ifdef";
  };

  g["Ifndef"] << "(m_hash k_ifndef Indent Identifier)" >> [](auto s) {
    std::cout << "This is Ifndef" << std::endl;
    int line = getLineNumber(s[0].position());
    std::cout << "line " << line << std::endl;
    std::cout << "'#ifndef' is from " << s[0].position() - idPrefixSum[line - 1]
              << " to " << s[1].position() + s[1].length() - idPrefixSum[line - 1] << std::endl;
    std::cout << "Identifier is from " << s[3].position() - idPrefixSum[line - 1]
              << " to " << s[3].position() + s[3].length() - idPrefixSum[line - 1] << std::endl;
    // changeAttr(attr1, s[0].position(), s[1].position() + s[1].length());
    // shallow yellow
    // changeAttr(attr2, s[3].position(), s[3].position() + s[3].length());
    // bold cyan
    return "Ifndef";
  };
  g["Else"] << "(m_hash k_else)" >> [](auto s) {
    std::cout << "This is Else" << std::endl;
    int line = getLineNumber(s[0].position());
    std::cout << "line " << line << std::endl;
    std::cout << "'#else' is from " << s[0].position() - idPrefixSum[line - 1]
              << " to " << s[1].position() + s[1].length() - idPrefixSum[line - 1] << std::endl;
    // changeAttr(attr, s.position(), s.position() + s.length());
    // shallow yellow
    return "Else";
  };

  g["Endif"] << "(m_hash k_endif)" >> [](auto s) {
    std::cout << "This is Endif" << std::endl;
    int line = getLineNumber(s[0].position());
    std::cout << "line " << line << std::endl;
    std::cout << "'#else' is from " << s[0].position() - idPrefixSum[line - 1]
              << " to " << s[1].position() + s[1].length() - idPrefixSum[line - 1] << std::endl;
    // changeAttr(attr, s.position(), s.position() + s.length());
    // shallow yellow
    return "Endif";
  };

  g["Define"] << "(m_hash k_define Indent Identifier (Indent Identifier)?)" >> [](auto s) {
    // WARNING: Only support basic usage of `define' directives
    std::cout << "This is Define" << std::endl;
    int line = getLineNumber(s.position());
    std::cout << "line " << line << std::endl;
    std::cout << "'#define' is from " << s[0].position() - idPrefixSum[line - 1]
              << " to " << s[1].position() + s[1].length() - idPrefixSum[line - 1] << std::endl;
    std::cout << "Identifier1 is from " << s[3].position() - idPrefixSum[line - 1]
              << " to " << s[3].position() + s[3].length() - idPrefixSum[line - 1] << std::endl;
    // changeAttr(attr1, s[0].position(), s[1].position() + s[1].length());
    // italic grey
    // changeAttr(attr2, s[3].position(), s[3].position() + s[3].length());
    // Bold grey
    if (s.size() == 6) {
      std::cout << "Identifier2 is from " << s[5].position() - idPrefixSum[line - 1]
                << " to " << s[5].position() + s[5].length() - idPrefixSum[line - 1] << std::endl;
      // changeAttr(attr3, s[5].position(), s[5].position() + s[5].length());
      // grey
    }
    return "Define";
  };

  g["Pragma"] << "(m_hash k_pragma Indent (!'\n' .)+)" >> [](auto s) {
    std::cout << "This is Pragma" << std::endl;
    int line = getLineNumber(s.position());
    std::cout << "line " << line << std::endl;
    std::cout << "'#pragma' is from " << s[0].position() - idPrefixSum[line - 1]
              << " to " << s[1].position() + s[1].length() - idPrefixSum[line - 1] << std::endl;
    std::cout << "other is from " << s[2].position() + s[2].length() - idPrefixSum[line - 1]
              << " to " << s.position() + s.length() - idPrefixSum[line - 1] << std::endl;
    // changeAttr(attr1, s[0].position(), s[1].position() + s[1].length());
    // shallow yellow
    // changeAttr(attr2, s[2].position() + s[2].length(), s.position() + s.length());
    // default
    return "Pragma";
  };
  g["Include"] << "(m_hash k_include Indent ((m_langle ([a-zA-Z] | '.' | '/' | '_')+ m_rangle) | (m_doubleQuotes ([a-zA-Z] | '.' | '/' | '_')+ m_doubleQuotes)))" >> [](auto s) {
    std::cout << "This is Include" << std::endl;
    int line = getLineNumber(s.position());
    std::cout << "line " << line << std::endl;
    std::cout << "'#include' is from " << s[0].position() - idPrefixSum[line - 1]
              << " to " << s[1].position() + s[1].length() - idPrefixSum[line - 1] << std::endl;
    std::cout << "header file is from " << s[3].position() - idPrefixSum[line - 1]
              << " to " << s[4].position() + s[4].length() - idPrefixSum[line - 1] << std::endl;
    // changeAttr(attr1, s[0].position(), s[1].position() + s[1].length());
    // shallow yellow
    // changeAttr(attr2,s[3].position(), s[4].position() + s[4].length());
    // green
    return "Include";
  };
  g["UsingNamespace"] << "(k_using Indent k_namespace Indent Identifier m_semicolon)" >> [](auto s) {
    std::cout << "This is UsingNamespace" << std::endl;
    int line = getLineNumber(s.position());
    std::cout << "line " << line << std::endl;
    std::cout << "'using' is from " << s[0].position() - idPrefixSum[line - 1]
              << " to " << s[0].position() + s[0].length() - idPrefixSum[line - 1] << std::endl;
    std::cout << "'namespace' is from " << s[2].position() - idPrefixSum[line - 1]
              << " to " << s[2].position() + s[2].length() - idPrefixSum[line - 1] << std::endl;
    // changeAttr(attr1, s[0].position(), s[0].position() + s[0].length());
    // blue
    // changeAttr(attr2, s[2].position(), s[2].position() + s[2].length());
    // cyan
    return "UsingNamespace";
  };
  g["UsingAssignment"] << "(k_using Indent Identifier Indent m_assign Indent QualifiedConstant m_semicolon)" >> [](auto s) {
    // using attr = std::string::size_type;
    std::cout << "This is UsingAssignment" << std::endl;
    int line = getLineNumber(s.position());
    std::cout << "line " << line << std::endl;

    // changeAttr(attr1, s[0].position(), s[0].position() + s[0].length());
    // blue
    // changeAttr(attr2, s[2].position(), s[2].position() + s[2].length());
    // purple
    // changeAttr(attr3, s[6].position(), s[6].position() + s[6].length());
    // purple
    return "UsingAssignment";
  };
  g["UsingClass"] << "(k_using Indent QualifiedConstant m_semicolon)" >> [](auto s) {
    std::cout << "This is UsingClass" << std::endl;
    int line = getLineNumber(s.position());
    std::cout << "line " << line << std::endl;
    // changeAttr(attr1, s[0].position(), s[0].position() + s[0].length());
    // blue
    // changeAttr(attr2, s[2].position(), s[2].position() + s[2].length());
    // default
    return "UsingClass";
  };
  //  g["Using"] << "(k_using Indent ((Identifier Indent '=' Indent QualifiedConstant) | ('namespace' Indent Identifier) | QualifiedConstant) ';')" >> [](auto) { return "using"; };
  g["Using"] << "(UsingNamespace | UsingAssignment | UsingClass)" >> [](auto s) { return s[0].evaluate(); };

  g["Typedef"] << "(k_typedef Type Identifier ';')" >> [&](auto s) {
    std::cout << "This is Typedef" << std::endl;
    int line = getLineNumber(s.position());
    typedef std::vector<double> gala;
    typeTable.append(s[1].string(), 3, g);
    std::cout << s[1].string() << " is added into typeTable." << std::endl;
    // changeAttr(attr1, s[0].position(), s[0].position() + s[0].length());
    // blue

    // Type has its own way to change Attr
    // s[1].evaluate();
    // changeAttr(attr2, s[2].position(), s[2].position() + s[2].position());
    // purple
    return "Typedef";
  };
  g["ArgumentList"] << "'(' (Type Indent Argument (', ' Type Indent Argument)*)? ')'"
    >> [&](auto s) {
      std::cout << "This is ArgumentList" << std::endl;
      for (int i = 0; i < s.size(); i += 3) {
        // Type has its own way to change Attr
        std::cout << "argument " << (i / 3) + 1 << "'s type is " << s[i].evaluate() << std::endl;
        // changeAttr(attr2, s[i + 2].position(), s[i + 2].position() + s[i + 2].length());
        // default
      }
      return "ArgumentList"; 
  };
  g["Argument"] << "(Identifier)";

  g["ParameterList"] << "('('( Parameter (',' Parameter)* )? ')')" >> [](auto s) {
    std::cout << "This is ParameterList" << std::endl;
    // evaluate
    return "ParameterList";
  };

  g["TemplateList"] << "'<' ManualType (',' Indent ManualType)* '>'" >> [](auto s) {
    std::cout << "This is TemplateList" << std::endl;
    // evaluate
    return "TemplateList";
  };

  g["AutoType"] << "((k_const Indent)? k_auto (' '? '&')?)" >> [](auto s) {
    std::cout << "This is AutoType" << std::endl;
    return "AutoType";
  };

  g["ReturnStatement"] << "(k_return Indent Expr m_semicolon)" >> [](auto s) {
    std::cout << "This is ReturnStatement" << std::endl;
    // changeAttr(attr1, s[0].position(), s[0].position() + s[0].length());
    // blue
    // s[2].evaluate();
    return "ReturnStatement";
  };

  g["WhileLoop"] << "(k_while Indent '(' Condition ')' ['\n''\t' ]* ((Statement | m_semicolon) | Block))"
  >> [](auto s) {
    std::cout << "This is WhileLoop" << std::endl;
    // changeAttr(attr1, s[0].position(), s[0].position() + s[0].length());
    // blue
    return "WhileLoop";
  };

  g["DoWhileLoop"] << "(k_do ['\n''\t' ]* ((Statement | m_semicolon) | Block)"
                      "['\n''\t' ]* k_while Indent '(' Condition ')' ' '? m_semicolon)"
  >> [](auto s) {
    // std::cout << s[2].evaluate() << std::endl;
    std::cout << "This is DoWhileLoop" << std::endl;
    // changeAttr(attr1, s[0].position(), s[0].position() + s[0].length());
    // changeAttr(attr1, s[2].position(), s[2].position() + s[2].length());
    // blue
    return "DoWhileLoop";
  };

  g["ForLoop"] << "(k_for Indent '(' Declaration? m_semicolon ' '? Condition? m_semicolon ' '? Expr? ')' ['\n''\t' ]* ((Statement | m_semicolon) | Block))"
  >> [](auto) {
    std::cout << "This is ForLoop" << std::endl;
    // changeAttr(attr1, s[0].position(), s[0].position() + s[0].length());
    // blue
    return "ForLoop";
  };

  g["StringLiteral"] << "(m_doubleQuotes ((!'\n' !'\"' !['\\\\'] .) | ['\\\\'] [a-z])* m_doubleQuotes (['\n''\t' ]* m_doubleQuotes ((!'\n' !'\"' !['\\\\'] .) | ['\\\\'] [a-z])* m_doubleQuotes)*)" >> [](auto s) {
    std::cout << "This is StringLiteral" << std::endl;
    for (int i = 0; i < s.size(); i += 2) {
      std::cout << "from " << s[i].position() - idPrefixSum[getLineNumber(s[i].position()) - 1]
                << " to " << s[i + 1].position() + s[i + 1].length() - idPrefixSum[getLineNumber(s[i + 1].position() + s[i + 1].length()) - 1] << std::endl;
      // green
    }
    return "StringLiteral";
  };
  // TODO: potential bug, cannot recognize '\n', '\b', etc.
  g["CharAtomic"] << "((!'\n' !['\\\\'] .) | ['\\\\'] [a-z])" >> [](auto) { return "CharAtomic"; };
  g["CharLiteral"] << "(m_singleQuotes CharAtomic m_singleQuotes)" >> [](auto s) {
    std::cout << "This is CharLiteral" << std::endl;
    std::cout << "from " << s[0].position() - idPrefixSum[getLineNumber(s[0].position()) - 1]
              << " to " << s[2].position() + s[2].length() - idPrefixSum[getLineNumber(s[2].position() + s[2].length()) - 1] << std::endl;
    // blue
    return "CharLiteral";
  };

  // DexLiteral include fraction, scientific notation, etc. tested twice
  g["DecLiteral"] << "('-'? ('0' | ([1-9][0-9]*)) ('.' [0-9]+)? ([eE] ('+' | '-')? ('0' | ([1-9][0-9]*)))?)" >> [](auto s) {
    std::cout << "This is DecLiteral" << std::endl;
    std::cout << "from " << s.position() - idPrefixSum[getLineNumber(s.position()) - 1]
              << " to " << s.position() + s.length() - idPrefixSum[getLineNumber(s.position() + s.length()) - 1] << std::endl;
    // blue
    return "DecLiteral";
  };
  g["HexLiteral"] << "('-'? '0x' ('0' | ([1-9a-fA-F][0-9a-fA-F]*)))" >> [](auto s) {
    std::cout << "This is HexLiteral" << std::endl;
    std::cout << "from " << s.position() - idPrefixSum[getLineNumber(s.position()) - 1]
              << " to " << s.position() + s.length() - idPrefixSum[getLineNumber(s.position() + s.length()) - 1] << std::endl;
    // blue
    return "HexLiteral";
  };
  g["OctLiteral"] << "('-'? '0' ('0' | ([1-7][0-7]*)))" >> [](auto s) {
    std::cout << "This is OctLiteral" << std::endl;
    std::cout << "from " << s.position() - idPrefixSum[getLineNumber(s.position()) - 1]
              << " to " << s.position() + s.length() - idPrefixSum[getLineNumber(s.position() + s.length()) - 1] << std::endl;
    // blue
    return "OctLiteral";
  };
  g["BoolLiteral"] << "(k_true | k_false)" >> [](auto s) {
    std::cout << "This is BoolLiteral" << std::endl;
    std::cout << "from " << s.position() - idPrefixSum[getLineNumber(s.position()) - 1]
              << " to " << s.position() + s.length() - idPrefixSum[getLineNumber(s.position() + s.length()) - 1] << std::endl;
    // blue
    return "BoolLiteral";
  };
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
//  g["Function"] << "(('inline')? Type Identifier ArgumentList ['\n''\t' ]* (';' | Block))" >> [](auto) { return "Function"; }; // tested twice
//   g["Test"] << "Function '\n'";
//   g.setStart(g["Test"]);
}

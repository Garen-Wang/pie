//
// Created by garen on 5/19/21.
//

#include <iostream>
#include <QFont>
#include <QtCore/QDebug>
#include <fstream>
#include "pierc.h"

namespace pierc {
  std::string fontName;
  int fontSize;

  void config(ConfigParser& parser) {
    parser["Indent"] << "['\t''\n' ]*" >> [](auto s) {
      return "Indent";
    };
    parser["Indentation"] << "['\t' ]*" >> [](auto s) {
      return "Indentation";
    };
    parser["set"] << "'set'" >> [](auto s) {
      return "set";
    };
    parser["fontFamily"] << "'fontFamily'" >> [](auto s) {
      return "fontFamily";
    };
    parser["fontSize"] << "'fontSize'" >> [](auto s) {
      return "fontSize";
    };
    parser["fontWeight"] << "'fontWeight'" >> [](auto s) {
      return "fontWeight";
    };
    parser["single_equal"] << "'='" >> [](auto s) {
      return "single_equal";
    };
    parser["double_quote"] << "'\"'" >> [](auto s) {
      return "double_quote";
    };
    parser["Number"] << "('0' | ([1-9] [0-9]*))" >> [](auto s) {
      return s.string();
    };
    parser["FontName"] << "(double_quote (!double_quote .)* double_quote)" >> [](auto s) {
      return s.string();
    };
    parser["setFontFamily"] << "(set Indentation fontFamily single_equal FontName)" >> [](auto s) {
      return fontName = s[4].string();
    };
    parser["setFontSize"] << "(set Indentation fontSize single_equal Number)" >> [](auto s) {
      std::string temp = s[4].string();
      fontSize = std::stoi(temp);
      return temp;
    };
    parser["Statement"] << "(setFontFamily | setFontSize)";
    parser["Program"] << "((Statement Indent)*)";

    parser.setStart(parser["Program"]);
  }

  // can use only once at the beginning when pie starts execution
  void gatherInfo() {
    pierc::ConfigParser cp;
    pierc::config(cp);

    std::ifstream ifs("../.pierc");
    std::string input((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
    std::cout << input << std::endl;
    try {
      std::string output = cp.run(input);
      std::cout << "output is: " << output << std::endl;
    } catch (peg_parser::SyntaxError &) {
      qDebug() << "Syntax Error when parsing .pierc";
    }
  }
  QFont getFont() {
    if (!fontName.empty() && fontSize > 0) return QFont(QString().fromStdString(fontName), fontSize);
    else return QFont();
  }
}

// void test() {
//   pierc::gatherInfo();
//   std::cout << pierc::fontName << std::endl;
//   std::cout << pierc::fontSize << std::endl;
// }
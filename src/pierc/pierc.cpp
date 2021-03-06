/* This library provides some user-customized settings
 * through modifying *.pierc* at the source root of *pie*.
 * Now users can specify the font family and font size,
 * instead of default font provided by Qt, to display the text.
 */

#include "pierc.h"
#include "builtin.h"

#include <QtCore/QDebug>
#include <fstream>
#include <iostream>

namespace pierc {
    std::string fontName;
    int fontSize;

    void config(ConfigParser &parser) {
        // newline is included in *Indent*
        parser["Indent"] << "['\t''\n' ]*" >> [](auto s) {
            return "Indent";
        };
        // but *Indentation* does not contain newline
        parser["Indentation"] << "['\t' ]*" >> [](auto s) {
            return "Indentation";
        };

        // specifications of some keywords
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

    /* can be used only once at the beginning when pie starts its execution */
    void gatherInfo() {
        pierc::ConfigParser cp;
        pierc::config(cp);

        FilePath fp(1, ".pierc");
        // std::cout << fp.getPathString() << std::endl;
        std::ifstream ifs(fp.getPathString());

        // std::ifstream ifs("./.pierc");
        std::string input((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
        if (input.empty()) 
          return;
        // DEBUG
        std::cout << input << std::endl;
        try {
            std::string output = cp.run(input);
            std::cout << "output is: " << output << std::endl;
        } catch (peg_parser::SyntaxError &) {
            qDebug() << "Syntax Error when parsing .pierc";
        }
    }
    QFont getFont() {
        if (!fontName.empty() && fontSize > 0)
            return QFont(QString().fromStdString(fontName), fontSize);
        else
            return QFont();
    }
}// namespace pierc

/* test whether pierc lib works */
void test() {
    pierc::gatherInfo();
    std::cout << pierc::fontName << std::endl;
    std::cout << pierc::fontSize << std::endl;
}

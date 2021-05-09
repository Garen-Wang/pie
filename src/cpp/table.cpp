#include <vector>
#include <unordered_map>
#include <string>
#include <algorithm>

typedef peg_parser::ParserGenerator<std::string> Parser;

class VariableTable {
  private:
    // for int value:
    // 0 means global variable
    // otherwise means local variable in certain block
    std::unordered_map<std::string, int> variableTable; // store variables
  public:
    void append(const std::string &variable, int status, Parser &g) {
      variableTable[variable] = status;
      // debug();
      std::string _variable = variable;
      std::replace(_variable.begin(), _variable.end(), ' ', '_');
      g["v_" + _variable] << ("'" + variable + "'") >> [](auto) { return "variable in variableTable"; };
    }
    bool check(const std::string &name) {
      return variableTable.find(name) != variableTable.end();
    }
    void update(Parser &g) {
      std::string expr = "(";
      bool first = true;
      for (auto it : variableTable) {
        std::string _variable = it.first;
        std::replace(_variable.begin(), _variable.end(), ' ', '_');
        expr += " v_" + _variable + " ";
        if (first) first = false;
        else expr += "|";
      }
      expr += ")";

      // DEBUG
      // std::cout << expr << std::endl;

      g["DefinedVariable"] << expr;
    }
    void debug() {
      std::cout << "--- DEBUGING ---" << std::endl;
      for (auto it : variableTable) {
        std::cout << it.first << ": " << it.second << std::endl;
      }
      std::cout << "--- DEBUGING ---" << std::endl;
    }
};

class TypeTable {
  private:
    // for int value:
    // 0 means this value has been updated
    // 1 stands for c primitive data type
    // 2 stands for c++ STL data type
    // 3 stands for user-defined data type
    std::unordered_map<std::string, int> typeTable; // store data types
  public:
    void append(std::string type, int status, Parser &g, bool tp=false) {
      typeTable[type] = status;
      std::string _type = type;
      std::replace(_type.begin(), _type.end(), ' ', '_');
      if (status == 1) {
        g["t_" + _type] << ("'" + type + "'" + (tp ? " Indent TemplateList" : "")) >> [](auto) { return "Primitive data type"; };
      } else if ( status == 2) {
        g["t_" + _type] << ("'std::'? '" + type + "'" + (tp ? " Indent TemplateList" : "")) >> [](auto) { return "STL data type"; };
      } else if (status == 3) {
        g["t_" + _type] << ("'" + type + "'" + (tp ? " Indent TemplateList" : "")) >> [](auto) { return "User-defined data type"; };
      }
    }

    void update(Parser &g) {
      std::string subexpr = "(";
      bool first = true;
      std::vector<std::string> types;
      for (auto it : typeTable) types.push_back(it.first);
      std::sort(types.begin(), types.end(), [](std::string a, std::string b) { return a.length() > b.length(); });
      for (auto it : types) {
        // DEBUG
        // std::cout << it << std::endl;
        if (first) first = false;
        else subexpr += "|";
        std::string _type = it;
        std::replace(_type.begin(), _type.end(), ' ', '_');
        subexpr += " t_" + _type + " ";
      }
      subexpr += ")";

      // DEBUG
      // std::cout << "subexpr: " << subexpr << std::endl;
      g["SubType"] << subexpr;
      g["ManualType"] << "( (k_const Indent)? (k_static Indent)? SubType (' '? ['*''&'])?)" >> [](auto e) { return e[e.size() - 1].string(); };
    }
    bool check(const std::string &str) {
      return typeTable.find(str) != typeTable.end();
    }
    void debug() {
      std::cout << "--- DEBUGING ---" << std::endl;
      for (auto it : typeTable) {
        std::cout << it.first << ": " << it.second << std::endl;
      }
      std::cout << "--- DEBUGING ---" << std::endl;
    }
};

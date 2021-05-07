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
    void append(const std::string &type, int status, Parser &g) {
      typeTable[type] = status;
      std::string _type = type;
      std::replace(_type.begin(), _type.end(), ' ', '_');
      g["t_" + _type] << ("'" + type + "'") >> [](auto) { return "type in typeTable"; };
    }
    bool check(const std::string &str) {
      return typeTable.find(str) != typeTable.end();
    }
};
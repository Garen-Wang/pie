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
    // for int value `status':
    // 0 stands for types that have been used in declaration, but not explicitly defined
    // 1 stands for c primitive data type
    // 2 stands for c++ STL data type without template
    // 3 stands for user-defined data type (suppose without template)
    // 4 stands for sub-datatype inside c++ STL, e.g. iterator
    // 5 stands for c++ STL data type with template
    std::unordered_map<std::string, int> typeTable; // store data types
  public:
    void append(std::string type, int status, Parser &g, bool needUpdate=false) {
      if (type.substr(0, 5) == "std::") type = type.substr(5);
      // DEBUG
      // std::cout << type << std::endl;

      if (typeTable.find(type) != typeTable.end() && (typeTable[type] == status || (status == 0 && typeTable[type] != 0))) return;
      typeTable[type] = status;
      std::string _type = type;
      std::replace(_type.begin(), _type.end(), ' ', '_');
      if (status == 0) {
        // nothing, waiting to be changed
      } else if (status == 1) {
        g["t_" + _type] << ("'" + type + "'") >> [](auto) { return "Primitive data type"; };
      } else if ( status == 2) {
        g["t_" + _type] << ("'std::'? '" + type + "'") >> [](auto) { return "STL data type without template"; };
      } else if (status == 3) {
        g["t_" + _type] << ("'" + type + "'") >> [](auto) { return "User-defined data type"; };
      } else if (status == 4) {
        // nothing, waiting to be updated
      } else if (status == 5) {
        g["t_" + _type] << ("'std::'? '" + type + "'" + " Indent TemplateList") >> [](auto) { return "STL data type with template"; };
      } else {
        throw std::invalid_argument("unspecified status in TypeTable.append");
      }
      if (needUpdate) update(g);
    }

    void update(Parser &g) {
      std::string subexpr = "(";
      bool first = true;
      std::vector<std::string> types, innertypes;
      for (const auto& it : typeTable) {
        types.push_back(it.first);
        if (it.second == 4) innertypes.push_back(it.first);
      }
      // subtypes inside STL
      for (const auto& it : typeTable) {
        if (it.second == 5) {
          std::string _type = it.first;
          std::replace(_type.begin(), _type.end(), ' ', '_');
          for (const auto& innertype : innertypes) {
            // DEBUG
            // std::cout << ("'std::" + it.first + "'" + " Indent TemplateList" + " '::" + innertype + "'") << std::endl;

            g["t_" + _type + "_" + innertype] << ("'std::'? '" + it.first + "'" + " Indent TemplateList" + " '::" + innertype + "'") >> [](auto) { return "sub-datatype inside STL"; };
            types.push_back(_type + "_" + innertype);
          }
        }
      }
      std::sort(types.begin(), types.end(), [](const std::string& a, const std::string& b) { return a.length() > b.length(); });
      for (const auto& it : types) {
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
//      std::cout << "subexpr: " << subexpr << std::endl;

      g["SubType"] << subexpr;
      g["ManualType"] << "( (k_const Indent)? (k_static Indent)? SubType (' '? ['*''&'])?)" >> [](auto e) { return e[e.size() - 1].string(); };
    }
    bool check() {
      for (const auto& it : typeTable) {
        if (it.second == 0) return false;
      }
      return true;
    }
    void debug() {
      std::cout << "--- DEBUGING ---" << std::endl;
      for (const auto& it : typeTable) {
        std::cout << it.first << ": " << it.second << std::endl;
      }
      std::cout << "--- DEBUGING ---" << std::endl;
    }
};

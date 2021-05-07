#include <iostream>
#include <string>
#include <vector>

#include "parser.cpp"

Parser generate() {
  Parser g;
  config(g);
  return g;
}

int main() {
  auto g = generate();
  io(g);
  return 0;
}
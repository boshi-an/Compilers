#include <memory>
#include <string>
#include <iostream>
#include "ast.h"

string indent_space(int num) {
    return string(num*INDENT_SIZE, ' ');
}
#include <map>
#include <string>
#include <vector>
#include "raw_program.h"

map<ull, bool> vis;
map<ull, int> func_ret_p;
std::map<std::string, int> stack_global_table;
std::map<std::string, std::vector<int> > stack_array_shape;
int last_dim;
string last_name;

int get_remaining_size(string array, int dim) {
    int siz = 1;
    assert(stack_array_shape.find(array) != stack_array_shape.end());
    for (int i=dim; i<stack_array_shape[array].size(); i++) {
        siz *= stack_array_shape[array][i];
    }
    return siz;
}
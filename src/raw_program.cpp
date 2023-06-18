#include "raw_program.h"

bool is_sys_func(string s) {
    if (s == "@getint") return true;
    if (s == "@getch") return true;
    if (s == "@getarray") return true;
    if (s == "@putint") return true;
    if (s == "@putch") return true;
    if (s == "@putarray") return true;
    if (s == "@starttime") return true;
    if (s == "@stoptime") return true;
    return false;
}

/* Get space allocated by a function */
int get_stack_frame_size(const koopa_raw_slice_t &slice) {
    int ret = 0;
    for (size_t i = 0; i < slice.len; i++) {
        auto ptr = slice.buffer[i];
        koopa_raw_basic_block_t block;
        koopa_raw_value_t value;
        switch (slice.kind) {
        case KOOPA_RSIK_BASIC_BLOCK:
            block = (koopa_raw_basic_block_t)ptr;
            ret += get_stack_frame_size(block->insts);
            break;
        case KOOPA_RSIK_FUNCTION:
            assert(false);  // Should not declare functions in functions
            break;
        case KOOPA_RSIK_VALUE:
            value = (koopa_raw_value_t)ptr;
            ret += get_stack_frame_size(value, true);
            break;
        default:
            assert(false);  // Not implemented
        }
    }
    return ret;
}

/* Get space allocated for a value */
int get_stack_frame_size(const koopa_raw_value_t &value, bool entrance) {
    const auto &kind = value->kind;
    int ret = 0;
    switch (kind.tag)
    {
    case KOOPA_RVT_RETURN:
        ret += get_stack_frame_size(kind.data.ret);
        break;
    case KOOPA_RVT_INTEGER:
        if (!entrance)
            ret += get_stack_frame_size(kind.data.integer);
        break;
    case KOOPA_RVT_BINARY:
        if (!entrance)
            ret += get_stack_frame_size(kind.data.binary);
        break;
    case KOOPA_RVT_BRANCH:
        ret += get_stack_frame_size(kind.data.branch);
        break;
    case KOOPA_RVT_JUMP:
        ret += get_stack_frame_size(kind.data.jump);
        break;
    case KOOPA_RVT_ALLOC:
        ret += get_allocate_size(value, false);
        break;
    case KOOPA_RVT_GLOBAL_ALLOC :
        return 0;
        break;
    case KOOPA_RVT_FUNC_ARG_REF:
        ret += get_stack_frame_size(kind.data.store);
        break;
    case KOOPA_RVT_STORE:
        ret += get_stack_frame_size(kind.data.store);
        break;
    case KOOPA_RVT_LOAD:
        if (!entrance) {
            ret += get_stack_frame_size(kind.data.load);
        }
        break;
    case KOOPA_RVT_CALL:
        if (!vis[(ull)value]) {
            printf("VISITING call: %lld\n", (ull)value);
            vis[(ull)value] = true;
            ret += get_stack_frame_size(kind.data.call);
        }
        break;
    case KOOPA_RVT_GET_ELEM_PTR:
        if (!entrance)
            ret += get_stack_frame_size(kind.data.get_elem_ptr);
        break;
    case KOOPA_RVT_GET_PTR:
        if (!entrance)
            ret += get_stack_frame_size(kind.data.get_ptr);
        break;
    default:
        printf("Unknown value type %d\n", kind.tag);
        assert(false);
        break;
    }
    return ret;
}

/* Get space allocated for store */
int get_stack_frame_size(const koopa_raw_store_t &store) {
    int ret = 0;
    if (store.dest->kind.tag == KOOPA_RVT_GET_ELEM_PTR || store.dest->kind.tag == KOOPA_RVT_GET_PTR) {
        ret += get_stack_frame_size(store.dest);
    }

    if (store.value->kind.tag == KOOPA_RVT_FUNC_ARG_REF) {

    }
    else if (store.value->kind.tag == KOOPA_RVT_INTEGER) {

    }
    else {
        ret += get_stack_frame_size(store.value);
    }
    return ret;
}

/* Get space allocated for load */
int get_stack_frame_size(const koopa_raw_load_t &load) {
    int ret = 4;
    if (load.src->kind.tag == KOOPA_RVT_ALLOC) {
    }
    else if (load.src->kind.tag == KOOPA_RVT_GLOBAL_ALLOC) {
    }
    else if (load.src->kind.tag == KOOPA_RVT_GET_ELEM_PTR || load.src->kind.tag == KOOPA_RVT_GET_PTR) {
        ret += get_stack_frame_size(load.src);
    }
    else {
        printf("Unknown load type %d\n", load.src->kind.tag);
        assert(0);  // Not implemented
    }
    return ret;
}

/* Get space allocated for getting ptr */
int get_stack_frame_size(const koopa_raw_get_ptr_t &get_ptr) {
    int ret = 4;
    auto kind = get_ptr.src->kind;
    if (kind.tag == KOOPA_RVT_ALLOC) {
        ret += get_stack_frame_size(get_ptr.index);
    }
    else if (kind.tag == KOOPA_RVT_GET_ELEM_PTR || kind.tag == KOOPA_RVT_GET_PTR || kind.tag == KOOPA_RVT_LOAD) {
        ret += get_stack_frame_size(get_ptr.src);
        ret += get_stack_frame_size(get_ptr.index);
    }
    else {
        printf("Unknown value type %d\n", kind.tag);
        assert(false);
    }
    return ret;
}

/* Get space allocated for getting element ptr */
int get_stack_frame_size(const koopa_raw_get_elem_ptr_t &get_elem_ptr) {
    int ret = 4;
    auto kind = get_elem_ptr.src->kind;
    if (kind.tag == KOOPA_RVT_ALLOC) {
        ret += get_stack_frame_size(get_elem_ptr.index);
    }
    else if (kind.tag == KOOPA_RVT_GLOBAL_ALLOC) {
        ret += get_stack_frame_size(get_elem_ptr.index);
    }
    else if (kind.tag == KOOPA_RVT_GET_ELEM_PTR || kind.tag == KOOPA_RVT_GET_PTR) {
        ret += get_stack_frame_size(get_elem_ptr.src);
        ret += get_stack_frame_size(get_elem_ptr.index);
    }
    else {
        printf("Unknown value type %d\n", kind.tag);
        assert(false);
    }
    return ret;
}

/* Get space allocated for a func ret */
int get_stack_frame_size(const koopa_raw_call_t &call) {
    int ret = 4;
    for (int i=0; i<call.args.len; i++) {
        ret += get_stack_frame_size((koopa_raw_value_t)call.args.buffer[i]);
    }
    return ret;
}

/* Get space allocated for a variable */
int get_allocate_size(const koopa_raw_value_t &value, bool do_allocate = false) {
    int ret = 0;
    if (value->ty->data.pointer.base->tag == KOOPA_RTT_INT32) {
        ret = 4;
        if (do_allocate) {
            stack_global_table[value->name+1] = delta_in_stack_frame;
        }
    }
    else if (value->ty->data.pointer.base->tag == KOOPA_RTT_ARRAY) {
        koopa_raw_type_kind_t* base = (koopa_raw_type_kind_t*)value->ty->data.pointer.base;
        ret = base->data.array.len * 4;
        if (do_allocate) {
            stack_global_table[value->name+1] = delta_in_stack_frame;
            stack_array_shape[value->name+1] = vector<int>();
            stack_array_shape[value->name+1].push_back(base->data.array.len);
        }
        while (base->data.array.base->tag != KOOPA_RTT_INT32) {
            base = (koopa_raw_type_kind_t*)base->data.array.base;
            ret *= base->data.array.len;
            if (do_allocate) {
                stack_array_shape[value->name+1].push_back(base->data.array.len);
            }
        }
    }
    else if (value->ty->data.pointer.base->tag == KOOPA_RTT_POINTER) {
        koopa_raw_type_kind_t* base = (koopa_raw_type_kind_t*)value->ty->data.pointer.base;
        ret = 4;
        if (do_allocate) {
            stack_global_table[value->name+1] = delta_in_stack_frame;
            stack_array_shape[value->name+1] = vector<int>();
            stack_array_shape[value->name+1].push_back(1);
        }
        while (base->data.array.base->tag != KOOPA_RTT_INT32) {
            base = (koopa_raw_type_kind_t*)base->data.array.base;
            if (do_allocate) {
                stack_array_shape[value->name+1].push_back(base->data.array.len);
            }
        }
    }
    else {
        assert(false);
    }
    return ret;
}

/* Get space allocated for a branch */
int get_stack_frame_size(const koopa_raw_branch_t &branch) {
    int ret = 0;
    ret += get_stack_frame_size(branch.cond);
    // ret += get_stack_frame_size(branch.true_bb);
    // ret += get_stack_frame_size(branch.false_bb);
    return ret;
}

/* Get space allocated for a jump */
int get_stack_frame_size(const koopa_raw_jump_t &jump) {
    return 0;
}

/* Get space allocated for a return value */
int get_stack_frame_size(const koopa_raw_return_t &statement) {
    int ret = 4;
    ret += get_stack_frame_size(statement.value);
    return ret;
}

/* Get space allocated for a integer constant */
int get_stack_frame_size(const koopa_raw_integer_t &integer) {
    return 4;
}

/* Get space allocated for a binary operation */
int get_stack_frame_size(const koopa_raw_binary_t &binary) {
    int ret = 4;
    if (binary.lhs->kind.tag == KOOPA_RVT_INTEGER) {
        ;
    }
    else {
        ret += get_stack_frame_size(binary.lhs);
        if (binary.rhs->kind.tag == KOOPA_RVT_INTEGER) {
            ;
        }
        else {
            ret += get_stack_frame_size(binary.rhs);
        }
    }
    return ret;
}

/* Get space allocated for a basic block */
int get_stack_frame_size(const koopa_raw_basic_block_t &block) {
    return get_stack_frame_size(block->insts);
}

/* Determine which function to use by slice's type */
string dfs_raw(const koopa_raw_slice_t &slice) {
    string ret;
    for (size_t i = 0; i < slice.len; i++) {
        auto ptr = slice.buffer[i];
        switch (slice.kind) {
        case KOOPA_RSIK_BASIC_BLOCK:
            ret += dfs_raw((koopa_raw_basic_block_t)ptr); // Why the tutorial used reinterpret_cast?
            break;
        case KOOPA_RSIK_FUNCTION:
            ret += dfs_raw((koopa_raw_function_t)ptr);
            break;
        case KOOPA_RSIK_VALUE:
            ret += dfs_raw((koopa_raw_value_t)ptr, true);
            break;
        default:
            assert(false);
        }
    }
    return ret;
}

/* Go through a function */
string dfs_raw(const koopa_raw_function_t &func) {
    string ret;
    if (is_sys_func(func->name)) return ret;
    current_stack_frame_size = get_stack_frame_size(func->bbs);
    // Align current_stack_frame_size to 16
    current_stack_frame_size = (current_stack_frame_size+4+15)/16*16;
    delta_in_stack_frame = 4;
    last_delta_p = 0;
    // printf("%d\n", current_stack_frame_size);
    ret += "    .globl " + string(func->name + 1) + "\n";
    ret += string(func->name + 1) + ":\n";
    ret += addi("sp", "sp", -current_stack_frame_size);
    ret += sw("ra", 0, "sp");
    ret += dfs_raw(func->bbs);
    return ret;
}

/* Go through a basic block */
string dfs_raw(const koopa_raw_basic_block_t &block) {
    string ret;
    ret += string(block->name+1) + ":\n";
    ret += dfs_raw(block->insts);
    return ret;
}

/* Go through a value */
string dfs_raw(const koopa_raw_value_t &value, bool entrance) {
    const auto &kind = value->kind;
    switch (kind.tag)
    {
    case KOOPA_RVT_RETURN:
        return dfs_raw(kind.data.ret);
        break;
    case KOOPA_RVT_INTEGER:
        if (!entrance)
            return dfs_raw(kind.data.integer);
        break;
    case KOOPA_RVT_BINARY:
        if (!entrance)
            return dfs_raw(kind.data.binary);
        break;
    case KOOPA_RVT_BRANCH:
        return dfs_raw(kind.data.branch);
        break;
    case KOOPA_RVT_JUMP:
        return dfs_raw(kind.data.jump);
        break;
    case KOOPA_RVT_ALLOC:
        return allocate(value);
        break;
    case KOOPA_RVT_GLOBAL_ALLOC:
        return allocate(value);
        break;
    case KOOPA_RVT_FUNC_ARG_REF:
        return dfs_raw(kind.data.store);
        break;
    case KOOPA_RVT_STORE:
        return dfs_raw(kind.data.store);
        break;
    case KOOPA_RVT_LOAD:
        if (!entrance)
            return dfs_raw(kind.data.load);
        break;
    case KOOPA_RVT_CALL:
        if (func_ret_p.find((ull)value) == func_ret_p.end()) {
            func_ret_p[(ull)value] = delta_in_stack_frame;
            last_delta_p = delta_in_stack_frame;
            printf("TRANSLATING call %lld: %d %d\n", (ull)value, entrance, vis[(ull)value]);
            return dfs_raw(kind.data.call);
        }
        else {
            last_delta_p = func_ret_p[(ull)value];
        }
        break;
    case KOOPA_RVT_GET_ELEM_PTR:
        if (!entrance)
            return dfs_raw(kind.data.get_elem_ptr);
        break;
    case KOOPA_RVT_GET_PTR:
        if (!entrance)
            return dfs_raw(kind.data.get_ptr);
        break;
    default:
        printf("Unknown value type %d\n", kind.tag);
        assert(false);
        break;
    }
    return string("");
}

/* Deal with get_ptr */
string dfs_raw(const koopa_raw_get_ptr_t &get_ptr) {
    auto kind = get_ptr.src->kind;
    string ret;
    if (kind.tag == KOOPA_RVT_ALLOC) {
        string name = get_ptr.src->name+1;
        last_dim = 0;
        // printf("GET PTR %s\n", name.c_str());
        int rem_size = get_remaining_size(name, last_dim+1);
        ret += dfs_raw(get_ptr.index);
        ret += "// GET PTR\n";
        ret += li("a0", rem_size);
        ret += li("a1", 4);
        ret += mul("a0", "a0", "a1");
        ret += lw("a3", last_delta_p, "sp");
        ret += mul("a0", "a0", "a3");
        ret += add("a0", "a0", "sp");
        ret += addi("a0", "a0", stack_global_table[name]);
        last_delta_p = delta_in_stack_frame;
        delta_in_stack_frame += 4;
        ret += sw("a0", last_delta_p, "sp");
        ret += "// GET PTR END\n";
        last_dim ++;
        last_name = name;
    }
    else if (kind.tag == KOOPA_RVT_GET_ELEM_PTR || kind.tag == KOOPA_RVT_GET_PTR || kind.tag == KOOPA_RVT_LOAD) {
        ret += dfs_raw(get_ptr.src);
        int src_delta_p = last_delta_p;
        int src_dim = last_dim;
        string src_name = last_name;
        ret += dfs_raw(get_ptr.index);
        int idx_delta_p = last_delta_p;
        int idx_dim = last_dim;
        string idx_name = last_name;
        // printf("GET PTR %s %d %s %d\n", src_name.c_str(), src_dim, idx_name.c_str(), idx_dim);
        int rem_size = get_remaining_size(src_name, src_dim+1);
        ret += "// GET PTR\n";

        ret += li("a0", rem_size);
        ret += li("a1", 4);
        ret += mul("a0", "a0", "a1");
        ret += lw("a2", src_delta_p, "sp");
        ret += lw("a3", idx_delta_p, "sp");
        ret += mul("a0", "a0", "a3");
        ret += add("a0", "a0", "a2");
        last_delta_p = delta_in_stack_frame;
        delta_in_stack_frame += 4;
        ret += sw("a0", last_delta_p, "sp");

        ret += "// GET PTR END\n";
        last_dim = src_dim + 1;
        last_name = src_name;
    }
    else {
        printf("Unknown value type %d\n", kind.tag);
        assert(false);
    }
    return ret;
}

/* Deal with get_elem_ptr */
string dfs_raw(const koopa_raw_get_elem_ptr_t &get_elem_ptr) {
    // string name = get_elem_ptr.index->name;
    auto kind = get_elem_ptr.src->kind;
    string ret;
    if (kind.tag == KOOPA_RVT_ALLOC) {
        string name = get_elem_ptr.src->name+1;
        last_dim = 0;
        printf("GET PTR %s\n", name.c_str());
        int rem_size = get_remaining_size(name, last_dim+1);
        ret += dfs_raw(get_elem_ptr.index);
        ret += "// GET ELEM PTR\n";
        ret += li("a0", rem_size);
        ret += li("a1", 4);
        ret += mul("a0", "a0", "a1");
        ret += lw("a3", last_delta_p, "sp");
        ret += mul("a0", "a0", "a3");
        ret += add("a0", "a0", "sp");
        ret += addi("a0", "a0", stack_global_table[name]);
        last_delta_p = delta_in_stack_frame;
        delta_in_stack_frame += 4;
        ret += sw("a0", last_delta_p, "sp");
        ret += "// GET ELEM PTR END\n";
        last_dim ++;
        last_name = name;
    }
    else if (kind.tag == KOOPA_RVT_GLOBAL_ALLOC) {
        string name = get_elem_ptr.src->name+1;
        last_dim = 0;
        int rem_size = get_remaining_size(name, last_dim+1);
        ret += dfs_raw(get_elem_ptr.index);
        ret += "// GET ELEM PTR\n";
        ret += li("a0", rem_size);
        ret += li("a1", 4);
        ret += mul("a0", "a0", "a1");
        ret += lw("a3", last_delta_p, "sp");
        ret += mul("a0", "a0", "a3");
        ret += la("a1", name);
        ret += add("a0", "a0", "a1");
        last_delta_p = delta_in_stack_frame;
        delta_in_stack_frame += 4;
        ret += sw("a0", last_delta_p, "sp");
        ret += "// GET ELEM PTR END\n";
        last_dim ++;
        last_name = name;
    }
    else if (kind.tag == KOOPA_RVT_GET_ELEM_PTR || kind.tag == KOOPA_RVT_GET_PTR) {
        ret += dfs_raw(get_elem_ptr.src);
        int src_delta_p = last_delta_p;
        int src_dim = last_dim;
        string src_name = last_name;
        ret += dfs_raw(get_elem_ptr.index);
        int idx_delta_p = last_delta_p;
        int idx_dim = last_dim;
        string idx_name = last_name;
        int rem_size = get_remaining_size(src_name, src_dim+1);
        ret += "// GET ELEM PTR\n";

        ret += li("a0", rem_size);
        ret += li("a1", 4);
        ret += mul("a0", "a0", "a1");
        ret += lw("a2", src_delta_p, "sp");
        ret += lw("a3", idx_delta_p, "sp");
        ret += mul("a0", "a0", "a3");
        ret += add("a0", "a0", "a2");
        last_delta_p = delta_in_stack_frame;
        delta_in_stack_frame += 4;
        ret += sw("a0", last_delta_p, "sp");

        ret += "// GET ELEM PTR END\n";
        last_dim = src_dim + 1;
        last_name = src_name;
    }
    else {
        printf("Unknown value type %d\n", kind.tag);
        assert(false);
    }
    return ret;
}

/* Go through a function call */
string dfs_raw(const koopa_raw_call_t &call) {
    string ret;
    ret += "// CALL\n";
    // assert(0);
    // ret += dfs_raw(call.args);
    int ret_val_p = delta_in_stack_frame;
    delta_in_stack_frame += 4;
    int sp_move = std::max((int)(call.args.len-params_in_reg)*4, (int)0);
    
    // Prepare arguments
    vector<int> call_arg_p;
    for (int i=0; i<call.args.len; i++) {
        ret += dfs_raw((koopa_raw_value_t)call.args.buffer[i]);
        call_arg_p.push_back(last_delta_p);
    }

    if (call.args.len > params_in_reg) {
        ret += addi("sp", "sp", -(call.args.len-params_in_reg)*4);
    }
    for (int i=0; i<call.args.len; i++) {
        if (i<params_in_reg) {
            assert(call.args.kind == KOOPA_RSIK_VALUE);
            ret += lw("a"+to_string(i), call_arg_p[i]+sp_move, "sp", "call");
        }
        else {
            ret += lw("t1", call_arg_p[i]+sp_move, "sp", "call");
            ret += sw("t1", (i-params_in_reg)*4, "sp");
        }
    }
    ret += "    call " + string(call.callee->name+1) + "\n";
    ret += sw("a0", ret_val_p+sp_move, "sp");
    if (call.args.len > params_in_reg) {
        ret += addi("sp", "sp", (call.args.len-params_in_reg)*4);
    }
    last_delta_p = ret_val_p;
    ret += "// CALL END\n";
    return ret;
}

/* Store an array */
string store_array(const koopa_raw_value_t &value, const string &name, bool first_elem) {
    string ret;
    if (value->kind.tag == KOOPA_RVT_INTEGER) {
        ret += "    .word " + std::to_string(value->kind.data.integer.value) + "\n";
    }
    else {
        auto elems = value->kind.data.aggregate.elems;
        if (first_elem) {
            stack_array_shape[name].push_back(elems.len);
        }
        for (int i=0; i<elems.len; i++) {
            ret += store_array((koopa_raw_value_t)elems.buffer[i], name, first_elem&(!i));
        }
    }
    return ret;
}

/* Allocate a variable */
string allocate(const koopa_raw_value_t &value) {
    string ret;
    if (value->kind.tag == KOOPA_RVT_GLOBAL_ALLOC) {
        ret += "    .globl " + string(value->name+1) + "\n";
        ret += string(value->name+1) + ":\n";
        if (value->kind.data.global_alloc.init->kind.tag == KOOPA_RVT_INTEGER) {
            ret += "    .word " + std::to_string(value->kind.data.global_alloc.init->kind.data.integer.value) + "\n";
        }
        else if (value->kind.data.global_alloc.init->kind.tag == KOOPA_RVT_AGGREGATE){
            ret += store_array(value->kind.data.global_alloc.init, value->name+1, true);
        }
        else if (value->kind.data.global_alloc.init->kind.tag == KOOPA_RVT_ZERO_INIT) {
            int size = get_allocate_size(value, true);
            ret += "    .zero " + std::to_string(size) + "\n";
        }
        else {
            printf("Unknown global init type %d\n", value->kind.data.global_alloc.init->kind.tag);
            assert(0);
        }
        // cout << value->name << " :" << endl;
        // for (auto x : stack_array_shape[value->name+1]) {
        //     cout << x << " ";
        // }
        // cout << endl;
        // ret += "    .word " + std::to_string(value->kind.data.global_alloc.init->kind.data.integer.value) + "\n";
    }
    else {
        int alloc_size = get_allocate_size(value, true);
        delta_in_stack_frame += alloc_size;
    }
    return ret + "\n";
}

/* Go through a store statement */
string dfs_raw(const koopa_raw_store_t &store) {
    string ret;
    string src_reg = "a0";
    int ptr_delta_p = 0;

    if (store.dest->kind.tag == KOOPA_RVT_GET_ELEM_PTR || store.dest->kind.tag == KOOPA_RVT_GET_PTR) {
        ret += dfs_raw(store.dest);
        ptr_delta_p = last_delta_p;
    }

    if (store.value->kind.tag == KOOPA_RVT_FUNC_ARG_REF) {
        ret += "// STORE FUNC VALUE\n";
        auto arg = store.value->kind.data.func_arg_ref;
        if (arg.index < params_in_reg) {
            src_reg = "a"+to_string(arg.index);
            // ret += sw("a"+to_string(arg.index), stack_global_table[store.dest->name], "sp");
        }
        else {
            ret += lw("a0", (arg.index-params_in_reg)*4 + current_stack_frame_size, "sp", "store func arg");
            // ret += sw("a0", stack_global_table[store.dest->name], "sp");
        }
    }
    else if (store.value->kind.tag == KOOPA_RVT_INTEGER) {
        ret += "// STORE DIRECT VALUE\n";
        ret += li("a0", store.value->kind.data.integer.value);
        // ret += sw("a0", stack_global_table[store.dest->name], "sp");
    }
    else {
        ret += "// STORE COMPUTE VALUE\n";
        ret += dfs_raw(store.value);
        ret += lw("a0", last_delta_p, "sp", "store others");
        // ret += sw("a0", stack_global_table[store.dest->name], "sp");
    }

    if (store.dest->kind.tag == KOOPA_RVT_ALLOC) {
        ret += sw(src_reg, stack_global_table[store.dest->name+1], "sp");
    }
    else if (store.dest->kind.tag == KOOPA_RVT_GLOBAL_ALLOC) {
        ret += "    la t0, " + string(store.dest->name+1) + "\n";
        ret += sw(src_reg, 0, "t0", "store");
    }
    else if (store.dest->kind.tag == KOOPA_RVT_GET_ELEM_PTR || store.dest->kind.tag == KOOPA_RVT_GET_PTR) {
        ret += lw("t0", ptr_delta_p, "sp", "store");
        ret += sw(src_reg, 0, "t0", "store");
    }
    else {
        printf("Unknown store dest type %d\n", store.dest->kind.tag);
        assert(0);
    }

    ret += "// STORE END\n";
    return ret;
}

/* Go through a load statement */
string dfs_raw(const koopa_raw_load_t &load) {
    string ret;
    if (load.src->kind.tag == KOOPA_RVT_ALLOC) {
        ret += lw("a0", stack_global_table[load.src->name+1], "sp", "load");
        ret += sw("a0", delta_in_stack_frame, "sp");
        last_delta_p = delta_in_stack_frame;
        delta_in_stack_frame += 4;
        last_name = load.src->name+1;
        last_dim = 0;
    }
    else if (load.src->kind.tag == KOOPA_RVT_GLOBAL_ALLOC) {
        ret += "    la a0, " + string(load.src->name+1) + "\n";
        ret += lw("a0", 0, "a0", "load");
        ret += sw("a0", delta_in_stack_frame, "sp");
        last_delta_p = delta_in_stack_frame;
        delta_in_stack_frame += 4;
        last_name = load.src->name+1;
        last_dim = 0;
    }
    else if (load.src->kind.tag == KOOPA_RVT_GET_ELEM_PTR || load.src->kind.tag == KOOPA_RVT_GET_PTR) {
        ret += dfs_raw(load.src);
        ret += lw("a0", last_delta_p, "sp", "load");
        ret += lw("a0", 0, "a0");
        ret += sw("a0", delta_in_stack_frame, "sp");
        last_delta_p = delta_in_stack_frame;
        delta_in_stack_frame += 4;
    }
    else {
        printf("Unknown load type %d\n", load.src->kind.tag);
        assert(0);  // Not implemented
    }
    return ret;
}

/* Go through a return statement */
string dfs_raw(const koopa_raw_return_t &statement) {
    string ret;
    ret += "// RET VAL\n";
    ret += dfs_raw(statement.value);
    ret += "// RET VAL END\n";
    ret += lw("a0", last_delta_p, "sp", "return");
    ret += lw("ra", 0, "sp", "return");
    ret += addi("sp", "sp", current_stack_frame_size);
    // ret += "    lw a0, " + std::to_string(last_delta_p) + "(sp)\n";
    // ret += "    addi sp, sp, " + std::to_string(current_stack_frame_size) + "\n";
    // if (last_reg != "a0")
    //     ret += string("    li a0, ") + last_reg + string("\n");
    ret += string("    ret\n\n");
    return ret;
}

/* Go through a integer constant */
string dfs_raw(const koopa_raw_integer_t &integer) {
    string ret;
    ret += li("a0", integer.value);
    ret += sw("a0", delta_in_stack_frame, "sp");
    // ret += "    li a0, " + std::to_string(integer.value) + "\n";
    // ret += "    sw a0, " + std::to_string(delta_in_stack_frame) + "(sp)\n";
    last_delta_p = delta_in_stack_frame;
    delta_in_stack_frame += 4;
    return ret;
}

/* Go through a binary operation */
string dfs_raw(const koopa_raw_binary_t &binary) {
    string ret;
    // cout << binary.lhs->kind.tag << " " << binary.rhs->kind.tag << endl;
    if (binary.lhs->kind.tag == KOOPA_RVT_INTEGER) {
        ret += dfs_raw(binary.rhs);
        ret += li("a0", binary.lhs->kind.data.integer.value);
        ret += lw("a1", last_delta_p, "sp", "binary");
    }
    else {
        ret += dfs_raw(binary.lhs);
        if (binary.rhs->kind.tag == KOOPA_RVT_INTEGER) {
            ret += lw("a0", last_delta_p, "sp", "binary");
            ret += li("a1", binary.rhs->kind.data.integer.value);
        }
        else {
            int p1 = last_delta_p;
            ret += dfs_raw(binary.rhs);
            int p2 = last_delta_p;
            ret += lw("a0", p1, "sp", "binary");
            ret += lw("a1", p2, "sp", "binary");
        }
    }
    switch (binary.op) {
    case KOOPA_RBO_ADD:
        ret += string("    add a0, a0, a1\n");
        break;
    case KOOPA_RBO_SUB:
        ret += string("    sub a0, a0, a1\n");
        break;
    case KOOPA_RBO_MUL:
        ret += string("    mul a0, a0, a1\n");
        break;
    case KOOPA_RBO_DIV:
        ret += string("    div a0, a0, a1\n");
        break;
    case KOOPA_RBO_MOD:
        ret += string("    rem a0, a0, a1\n");
        break;
    case KOOPA_RBO_EQ:
        ret += string("    sub a0, a0, a1\n");
        ret += string("    seqz a0, a0\n");
        break;
    case KOOPA_RBO_NOT_EQ:
        ret += string("    sub a0, a0, a1\n");
        ret += string("    snez a0, a0\n");
        break;
    case KOOPA_RBO_GT:
        ret += string("    sgt a0, a0, a1\n");
        break;
    case KOOPA_RBO_LT:
        ret += string("    slt a0, a0, a1\n");
        break;
    case KOOPA_RBO_GE:
        ret += string("    sgt a2, a0, a1\n");
        ret += string("    sub a3, a0, a1\n");
        ret += string("    seqz a3, a3\n");
        ret += string("    or a0, a2, a3\n");
        break;
    case KOOPA_RBO_LE:
        ret += string("    slt a2, a0, a1\n");
        ret += string("    sub a3, a0, a1\n");
        ret += string("    seqz a3, a3\n");
        ret += string("    or a0, a2, a3\n");
        break;
    case KOOPA_RBO_OR:
        ret += string("    or a0, a0, a1\n");
        break;
    case KOOPA_RBO_AND:
        ret += string("    and a0, a0, a1\n");
        break;
    case KOOPA_RBO_XOR:
        ret += string("    xor a0, a0, a1\n");
        break;
    default:
        printf("Unknown binary operation %d\n", binary.op);
        assert(false);
        break;
    }
    int p3 = delta_in_stack_frame;
    ret += sw("a0", p3, "sp");
    // ret += "    sw a0, " + std::to_string(p3) + "(sp)\n";
    last_delta_p = delta_in_stack_frame;
    delta_in_stack_frame += 4;
    return ret;
}

/* Go through a if-else branch */
string dfs_raw(const koopa_raw_branch_t &branch) {
    string ret;
    ret += dfs_raw(branch.cond);
    int p1 = last_delta_p;
    ret += "//BRANCH START!=================\n";
    ret += lw("a0", p1, "sp", "if-else");
    // ret += "    lw a0, " + std::to_string(p1) + "(sp)\n";
    ret += "    beqz a0, " + string(branch.false_bb->name+1) + "\n";
    // ret += dfs_raw(branch.true_bb);
    ret += "    j " + string(branch.true_bb->name+1) + "\n";
    // ret += dfs_raw(branch.false_bb);
    ret += "//BRANCH END!=================\n";
    return ret;
}

/* Go thourgh a direct jump */
string dfs_raw(const koopa_raw_jump_t &jump) {
    string ret;
    ret += "    j " + string(jump.target->name+1) + "\n";
    return ret;
}

/* Go through the raw program */
string dfs_raw(const koopa_raw_program_t &program) {
    string ret;
    ret += string("    .data\n");
    ret += dfs_raw(program.values);
    ret += string("    .text\n");
    ret += dfs_raw(program.funcs);
    return ret;
}

string ir_to_raw_program(string ir) {
    koopa_program_t program;
    koopa_error_code_t ret = koopa_parse_from_string(ir.c_str(), &program);
    assert(ret == KOOPA_EC_SUCCESS);
    koopa_raw_program_builder_t builder = koopa_new_raw_program_builder();
    koopa_raw_program_t raw = koopa_build_raw_program(builder, program);
    koopa_delete_program(program);

    // Parse the raw program
    string riscv = dfs_raw(raw);

    koopa_delete_raw_program_builder(builder);

    return riscv;
}
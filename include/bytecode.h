#pragma once

#include <cstdint>
#include <iostream>
#include <set>
#include <string>
#include <vector>

#include <runtime.h>
#include <semantics.h>

namespace bytecode {

enum class op_code_t {
    band_, // bitwise and
    bneg_, // bitwise negation
    getfield_, // get a field value of an object objectref
    goto_, // goes to another instruction at branchoffset
    goto_if_false_, // if value is false (0), goes to another instruction at branchoffset
    iadd_, // add two ints
    iaload_, // load an int from an array
    iastore_, // store an int into an array
    ilt_, // less than
    imul_, // multiply two integers
    invoke_, // invoke instance method on object objectref and puts result on the stack
    isub_, // subtract two integers
    load_, // load a reference onto the stack from a local variable #index
    ldc_, // push a constant onto the stack
    length_, // array length
    new_, // create new object of type identified by class reference
    newarray_, // create new array of integers
    putfield_, // set field to value in an object objectref
    print_, // print integer
    return_, // return from method
    store_, // store value into variable
};

struct instruction_t {
    op_code_t op_code;
    long operand, operand2;
    std::string as_str()
    {
        switch (op_code) {
        case op_code_t::band_: return "band";
        case op_code_t::bneg_: return "bneg";
        case op_code_t::getfield_: return "getfield " + std::to_string(operand);
        case op_code_t::goto_: return "goto " + std::to_string(operand);
        case op_code_t::goto_if_false_: return "goto_if_false " + std::to_string(operand);
        case op_code_t::iadd_: return "iadd";
        case op_code_t::iaload_: return "iaload";
        case op_code_t::iastore_: return "iastore";
        case op_code_t::ilt_: return "ilt";
        case op_code_t::imul_: return "imul";
        case op_code_t::invoke_:
            return "invoke " + std::to_string(operand) + " " + std::to_string(operand2);
        case op_code_t::isub_: return "isub";
        case op_code_t::load_: return "load " + std::to_string(operand);
        case op_code_t::ldc_: return "ldc " + std::to_string(operand);
        case op_code_t::length_: return "length";
        case op_code_t::new_: return "new " + std::to_string(operand);
        case op_code_t::newarray_: return "newarray";
        case op_code_t::putfield_: return "putfield " + std::to_string(operand);
        case op_code_t::print_: return "print";
        case op_code_t::return_: return "return";
        case op_code_t::store_: return "store " + std::to_string(operand);
        default: std::cerr << "Unknown op code" << std::endl; exit(1);
        }
    }
};

} // namespace bytecode

namespace bc_compiler {

struct method_layout_t;

struct class_layout_t {
    std::string parent;
    std::string name;
    std::vector<std::string> fields;
    std::vector<std::pair<std::string, std::string>> vtbl;
    std::vector<method_layout_t *> vtable;
};

enum class bb_state_t {
    fresh,
    jmp_target_computed,
    linearized,
};

struct basic_block_t {
    static size_t bb_num;
    static size_t bb_inst_num;
    size_t bb_id;
    size_t bb_inst_start;
    bb_state_t bb_state;
    std::vector<bytecode::instruction_t> instructions;
    basic_block_t *then_branch;
    basic_block_t *else_branch;
    basic_block_t()
      : bb_state(bb_state_t::fresh), then_branch(nullptr), else_branch(nullptr)
    {
        bb_id = bb_num++;
    }
    void compute_jmp_targets_(size_t n, std::set<basic_block_t *> &visited);
    void compute_jmp_targets(size_t n);
    void fix_goto_(size_t n, std::set<basic_block_t *> &visited);
    void fix_goto(size_t n);
    void try_linearize_(std::vector<bytecode::instruction_t> &vi, size_t n,
                        std::set<basic_block_t *> &visited);
    void try_linearize(std::vector<bytecode::instruction_t> &vi, size_t n);
};

struct method_layout_t {
    std::pair<std::string, std::string> method_name;
    std::vector<std::string> args;
    std::vector<std::string> locals;
    std::vector<bytecode::instruction_t> instructions;
};

class layout_visitor_t : public visitor::visitor_t {
public:
    std::vector<class_layout_t> classes;
    std::vector<method_layout_t> methods;
    std::string current_class;
    void visit(parser::goal_t *node) override;
    void visit(parser::main_class_t *node) override;
    void visit(parser::class_decl_t *node) override;
    void visit(parser::var_decl_t *node) override{};
    void visit(parser::method_decl_t *node) override;
    void visit(parser::type_t *node) override{};
    void visit(parser::statement_t *node) override{};
    void visit(parser::block_statement_t *node) override{};
    void visit(parser::if_statement_t *node) override{};
    void visit(parser::while_statement_t *node) override{};
    void visit(parser::print_statement_t *node) override{};
    void visit(parser::assign_statement_t *node) override{};
    void visit(parser::array_assign_statement_t *node) override{};
    void visit(parser::expression_t *node) override{};
    void visit(parser::binary_expression_t *node) override{};
    void visit(parser::array_index_expression_t *node) override{};
    void visit(parser::array_length_expression_t *node) override{};
    void visit(parser::method_call_expression_t *node) override{};
    void visit(parser::integer_literal_expression_t *node) override{};
    void visit(parser::true_literal_expression_t *node) override{};
    void visit(parser::false_literal_expression_t *node) override{};
    void visit(parser::identifier_expression_t *node) override{};
    void visit(parser::this_expression_t *node) override{};
    void visit(parser::new_integer_array_expression_t *node) override{};
    void visit(parser::new_object_expression_t *node) override{};
    void visit(parser::not_expression_t *node) override{};
    void visit(parser::parentheses_expression_t *node) override{};
};

class vt_visitor_t : public visitor::visitor_t {
public:
    std::vector<class_layout_t> classes;
    std::vector<method_layout_t> methods;
    vt_visitor_t(std::vector<class_layout_t> classes, std::vector<method_layout_t> methods)
      : classes(classes), methods(methods)
    {
    }
    void visit(parser::goal_t *node) override;
    void visit(parser::main_class_t *node) override;
    void visit(parser::class_decl_t *node) override;
    void visit(parser::var_decl_t *node) override{};
    void visit(parser::method_decl_t *node) override{};
    void visit(parser::type_t *node) override{};
    void visit(parser::statement_t *node) override{};
    void visit(parser::block_statement_t *node) override{};
    void visit(parser::if_statement_t *node) override{};
    void visit(parser::while_statement_t *node) override{};
    void visit(parser::print_statement_t *node) override{};
    void visit(parser::assign_statement_t *node) override{};
    void visit(parser::array_assign_statement_t *node) override{};
    void visit(parser::expression_t *node) override{};
    void visit(parser::binary_expression_t *node) override{};
    void visit(parser::array_index_expression_t *node) override{};
    void visit(parser::array_length_expression_t *node) override{};
    void visit(parser::method_call_expression_t *node) override{};
    void visit(parser::integer_literal_expression_t *node) override{};
    void visit(parser::true_literal_expression_t *node) override{};
    void visit(parser::false_literal_expression_t *node) override{};
    void visit(parser::identifier_expression_t *node) override{};
    void visit(parser::this_expression_t *node) override{};
    void visit(parser::new_integer_array_expression_t *node) override{};
    void visit(parser::new_object_expression_t *node) override{};
    void visit(parser::not_expression_t *node) override{};
    void visit(parser::parentheses_expression_t *node) override{};
};

class bc_compiler_visitor_t : public visitor::visitor_t {
public:
    basic_block_t *current_basic_block;
    std::vector<class_layout_t> classes;
    std::vector<method_layout_t> methods;
    size_t current_method;
    semantics::semantic_vis_type_check_t type_checker;
    bc_compiler_visitor_t(std::vector<class_layout_t> classes,
                          std::vector<method_layout_t> methods,
                          semantics::semantic_vis_type_check_t type_checker)
      : classes(classes), methods(methods), current_method(0), type_checker(type_checker)
    {
    }
    void print();
    void visit(parser::goal_t *node) override;
    void visit(parser::main_class_t *node) override;
    void visit(parser::class_decl_t *node) override;
    void visit(parser::var_decl_t *node) override;
    void visit(parser::method_decl_t *node) override;
    void visit(parser::type_t *node) override;
    void visit(parser::statement_t *node) override;
    void visit(parser::block_statement_t *node) override;
    void visit(parser::if_statement_t *node) override;
    void visit(parser::while_statement_t *node) override;
    void visit(parser::print_statement_t *node) override;
    void visit(parser::assign_statement_t *node) override;
    void visit(parser::array_assign_statement_t *node) override;
    void visit(parser::expression_t *node) override;
    void visit(parser::binary_expression_t *node) override;
    void visit(parser::array_index_expression_t *node) override;
    void visit(parser::array_length_expression_t *node) override;
    void visit(parser::method_call_expression_t *node) override;
    void visit(parser::integer_literal_expression_t *node) override;
    void visit(parser::true_literal_expression_t *node) override;
    void visit(parser::false_literal_expression_t *node) override;
    void visit(parser::identifier_expression_t *node) override;
    void visit(parser::this_expression_t *node) override;
    void visit(parser::new_integer_array_expression_t *node) override;
    void visit(parser::new_object_expression_t *node) override;
    void visit(parser::not_expression_t *node) override;
    void visit(parser::parentheses_expression_t *node) override;
};
}
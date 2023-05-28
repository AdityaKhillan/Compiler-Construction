#ifndef AST_HH
#define AST_HH

#include <llvm/IR/Value.h>
#include <string>
#include <vector>

struct LLVMCompiler;

/**
Base node class. Defined as `abstract`.
*/
struct Node {
    enum NodeType {
        TERN_OP, BIN_OP, INT_LIT, STMTS, ASSN, DBG, IDENT, ASSIGN, IF
    } type;

    virtual std::string to_string() = 0;
    virtual llvm::Value *llvm_codegen(LLVMCompiler *compiler) = 0;
};

/**
    Node for list of statements
*/
struct NodeStmts : public Node {
    std::vector<Node*> list;

    NodeStmts();
    void push_back(Node *node);
    std::string to_string();
    llvm::Value *llvm_codegen(LLVMCompiler *compiler);
};

struct NodeScope : public Node {
    NodeStmts* mid;
    NodeScope(NodeStmts* mid);
    std::string to_string();
    llvm::Value *llvm_codegen(LLVMCompiler *compiler);
};

/**
    Node for binary operations
*/
struct NodeBinOp : public Node {
    enum Op {
        PLUS, MINUS, MULT, DIV
    } op;

    Node *left, *right;

    NodeBinOp(Op op, Node *leftptr, Node *rightptr);
    std::string to_string();
    llvm::Value *llvm_codegen(LLVMCompiler *compiler);
};

/**
    Node for ternary operations
*/

struct NodeTernaryOp : public Node {
    enum Op {
        QMARK
    } op;

    Node *left, *right, *mid;

    NodeTernaryOp(Op op, Node *leftptr, Node *midptr, Node *rightptr);
    std::string to_string();
    llvm::Value *llvm_codegen(LLVMCompiler *compiler);
};

struct NodeIf : public Node {
    Node *left, *right, *mid;

    NodeIf(Node *leftptr, Node *midptr, Node *rightptr);
    std::string to_string();
    llvm::Value *llvm_codegen(LLVMCompiler *compiler);
};



/**
    Node for integer literals
*/
struct NodeInt : public Node {
    int value;

    NodeInt(int val);
    std::string to_string();
    llvm::Value *llvm_codegen(LLVMCompiler *compiler);
};

/**
    Node for variable assignments
*/
struct NodeAssn : public Node {
    std::string identifier;
    Node *expression;
    std::string dtype;

    NodeAssn(std::string id, Node *expr, std::string dtype);
    std::string to_string();
    llvm::Value *llvm_codegen(LLVMCompiler *compiler);
};

/**
    Node for variable assignments LAB
*/

struct NodeAssign : public Node {
    std::string identifier;
    Node *expression;

    NodeAssign(std::string id, Node *expr);
    std::string to_string();
    llvm::Value *llvm_codegen(LLVMCompiler *compiler);
};

/**
    Node for `dbg` statements
*/
struct NodeDebug : public Node {
    Node *expression;

    NodeDebug(Node *expr);
    std::string to_string();
    llvm::Value *llvm_codegen(LLVMCompiler *compiler);
};

/**
    Node for idnetifiers
*/
struct NodeIdent : public Node {
    std::string identifier;

    NodeIdent(std::string ident);
    std::string to_string();
    llvm::Value *llvm_codegen(LLVMCompiler *compiler);
};

struct FunctNode: public Node{
    NodeStmts* param_list;
    std::string identifier;
    NodeStmts* Program;
    std::string retType;
    FunctNode(std::string ident,NodeStmts* param_list, NodeStmts* program, std::string datatype);
    std::string to_string();
    llvm::Value *llvm_codegen(LLVMCompiler *compiler);
};


#endif
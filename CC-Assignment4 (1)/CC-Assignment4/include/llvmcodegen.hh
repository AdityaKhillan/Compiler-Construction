#ifndef LLVMCODEGEN_HH
#define LLVMCODEGEN_HH

#include <llvm/IR/Instructions.h>
#include <string>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <unordered_map>
#include "ast.hh"

using namespace llvm;

/**
    Compiler struct to store state of the LLVM IRBuilder.
    The `compile` method recursively calls the llvmcodegen method for a given 
    `Node`.
*/
struct SymbolTableTreeNode{
    std::unordered_map<std::string, std::pair<AllocaInst*,Type*>> locals;
    SymbolTableTreeNode* parent;
    SymbolTableTreeNode(){};
    
    SymbolTableTreeNode(SymbolTableTreeNode* parent){
        this->parent=parent;
    }
};

typedef struct SymbolTableTreeNode SymbolTableTreeNode;
struct LLVMCompiler {
    LLVMContext *context;
    IRBuilder<> builder;
    Module module;
    
    SymbolTableTreeNode* current_table;
    LLVMCompiler(LLVMContext *context, std::string file_name) : 
        context(context), builder(*context), module(file_name, *context) {
            current_table=new SymbolTableTreeNode(nullptr);
        }
    
    void compile(Node *root);
    void dump();
    void write(std::string file_name);
};

#endif
#include "llvmcodegen.hh"
#include "ast.hh"
#include <iostream>
#include <llvm/Support/FileSystem.h>
#include <llvm/IR/Constant.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/GlobalValue.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Bitcode/BitcodeWriter.h>
#include <vector>

#define MAIN_FUNC compiler->module.getFunction("main")

/*
The documentation for LLVM codegen, and how exactly this file works can be found
ins `docs/llvm.md`
*/
// SymbolTableTreeNode* current_parent=nullptr;
void LLVMCompiler::compile(Node *root) {
    /* Adding reference to print_i in the runtime library */
    // void printi();
    FunctionType *printi_func_type_s = FunctionType::get(
        builder.getVoidTy(),
        {builder.getInt16Ty()},
        false
    );
    FunctionType *printi_func_type_i = FunctionType::get(
        builder.getVoidTy(),
        {builder.getInt32Ty()},
        false
    );
    FunctionType *printi_func_type_l = FunctionType::get(
        builder.getVoidTy(),
        {builder.getInt64Ty()},
        false
    );
    Function::Create(
        printi_func_type_s,
        GlobalValue::ExternalLinkage,
        "printi_s",
        &module
    );
    Function::Create(
        printi_func_type_i,
        GlobalValue::ExternalLinkage,
        "printi_i",
        &module
    );
    Function::Create(
        printi_func_type_l,
        GlobalValue::ExternalLinkage,
        "printi_l",
        &module
    );
    /* we can get this later 
        module.getFunction("printi");
    */

    /* Main Function */
    // int main();
    // FunctionType *main_func_type = FunctionType::get(
    //     builder.getInt32Ty(), {}, false /* is vararg */
    // );
    // Function *main_func = Function::Create(
    //     main_func_type,
    //     GlobalValue::ExternalLinkage,
    //     "main",
    //     &module
    // );

    // create main function block
    // BasicBlock *main_func_entry_bb = BasicBlock::Create(
    //     *context,
    //     "entry",
    //     main_func
    // );

    // move the builder to the start of the main function block
    // builder.SetInsertPoint(main_func_entry_bb);

    root->llvm_codegen(this);

    // return 0;
    // builder.CreateRet(builder.getInt32(0));
}

void LLVMCompiler::dump() {
    outs() << module;
    
}

void LLVMCompiler::write(std::string file_name) {
    std::error_code EC;
    raw_fd_ostream fout(file_name, EC, sys::fs::OF_None);
    WriteBitcodeToFile(module, fout);
    fout.flush();
    fout.close();
}

//  ┌―――――――――――――――――――――┐  //
//  │ AST -> LLVM Codegen │  //
// └―――――――――――――――――――――┘   //

// codegen for statements
Value *NodeStmts::llvm_codegen(LLVMCompiler *compiler) {
    Value *last = nullptr;
    std::cout<<"sCOPING2"<<std::endl;

    for(auto node : list) {
        last = node->llvm_codegen(compiler);
    }
    Value* cast=compiler->builder.CreateSExt(last,Type::getInt64Ty(*compiler->context));

    return cast;
}

Value *NodeDebug::llvm_codegen(LLVMCompiler *compiler) {
    Value *expr = expression->llvm_codegen(compiler);
    Type* type=expr->getType();
    Function *printi_func;
    if(type->getIntegerBitWidth()==16){
        printi_func = compiler->module.getFunction("printi_s");
    }else if(type->getIntegerBitWidth()==32){
        printi_func = compiler->module.getFunction("printi_i");
    }else if(type->getIntegerBitWidth()==64){
        printi_func = compiler->module.getFunction("printi_l");
    }


    compiler->builder.CreateCall(printi_func, {expr});
    return expr;
}

Value *NodeInt::llvm_codegen(LLVMCompiler *compiler) {
    if(value > (unsigned long)(1<<31))
        {std::cout<<(1<<31)<<std::endl;return compiler->builder.getInt64(value);}
    else 
        return compiler->builder.getInt32(value);
}

Value *NodeBinOp::llvm_codegen(LLVMCompiler *compiler) {
    Value *left_expr = left->llvm_codegen(compiler);
    Value *right_expr = right->llvm_codegen(compiler);

    Type* type1=left_expr->getType();
    Type* type2=right_expr->getType();

    
    if(type1->getIntegerBitWidth()>type2->getIntegerBitWidth()){
        right_expr=compiler->builder.CreateSExtOrTrunc(right_expr,type1);
    }
    else if(type1->getIntegerBitWidth()<type2->getIntegerBitWidth()){
        left_expr=compiler->builder.CreateSExtOrTrunc(left_expr,type2);

    }




    switch(op) {
        case PLUS:
        return compiler->builder.CreateAdd(left_expr, right_expr, "addtmp");
        case MINUS:
        return compiler->builder.CreateSub(left_expr, right_expr, "minustmp");
        case MULT:
        return compiler->builder.CreateMul(left_expr, right_expr, "multtmp");
        case DIV:
        return compiler->builder.CreateSDiv(left_expr, right_expr, "divtmp");
    }
}

Value *NodeTernaryOp::llvm_codegen(LLVMCompiler *compiler) {
    Value *left_expr = left->llvm_codegen(compiler);
    Value *right_expr = right->llvm_codegen(compiler);
    Value *mid_expr = mid->llvm_codegen(compiler);

    return nullptr;
}


Value *NodeAssn::llvm_codegen(LLVMCompiler *compiler) {
    Value *expr = expression->llvm_codegen(compiler);

    // IRBuilder<> temp_builder(
    //     &MAIN_FUNC->getEntryBlock(),
    //     MAIN_FUNC->getEntryBlock().begin()
    // );


    AllocaInst *alloc;
    Type* type;

    Value* cast=expr;
    if(dtype=="short"){
        type=compiler->builder.getInt16Ty();
        cast=compiler->builder.CreateSExtOrTrunc(expr,type);
        alloc=compiler->builder.CreateAlloca(type, 0, identifier);
    }else if(dtype=="int"){
        type=compiler->builder.getInt32Ty();
        alloc=compiler->builder.CreateAlloca(type, 0, identifier);
    }else if(dtype=="long"){
        type=compiler->builder.getInt64Ty();
        cast=compiler->builder.CreateSExtOrTrunc(expr,type);
        alloc=compiler->builder.CreateAlloca(type, 0, identifier);
    }
    //AllocaInst* alloca = temp_builder.CreateAlloca(compiler->builder.getInt32Ty(), 0, identifier);
    
    
    compiler->current_table->locals[identifier].first = alloc;
    compiler->current_table->locals[identifier].second = type;
    
    compiler->builder.CreateStore(cast, alloc);
    return cast;
}

Value *NodeAssign::llvm_codegen(LLVMCompiler *compiler) {
    Value *expr = expression->llvm_codegen(compiler);

    // IRBuilder<> temp_builder(
    //     &MAIN_FUNC->getEntryBlock(),
    //     MAIN_FUNC->getEntryBlock().begin()
    // );

    Type* type=expr->getType();
    SymbolTableTreeNode* temp=compiler->current_table;
    while(compiler->current_table!=nullptr){
        if(compiler->current_table->locals.find(identifier)!=compiler->current_table->locals.end()) break;
        compiler->current_table=compiler->current_table->parent;
    }
    expr=compiler->builder.CreateSExtOrTrunc(expr,compiler->current_table->locals[identifier].second);
    AllocaInst *alloc = compiler->current_table->locals[identifier].first;
    compiler->current_table=temp;
    

    compiler->builder.CreateStore(expr, alloc);
    return expr;
}

Value *NodeIdent::llvm_codegen(LLVMCompiler *compiler) {
    
    // if your LLVM_MAJOR_VERSION >= 14
    SymbolTableTreeNode* temp=compiler->current_table;
    while(compiler->current_table!=nullptr){
        if(compiler->current_table->locals.find(identifier)!=compiler->current_table->locals.end()) break;
        compiler->current_table=compiler->current_table->parent;
    }
    Type* type = compiler->current_table->locals[identifier].second;
    AllocaInst *alloc = compiler->current_table->locals[identifier].first;
    compiler->current_table=temp;
    return compiler->builder.CreateLoad(type, alloc, identifier);
}

Value *NodeIf::llvm_codegen(LLVMCompiler *compiler){
    Value* cond=this->left->llvm_codegen(compiler);
    // Value* if_part=this->mid->llvm_codegen(compiler);
    // Value* else_part=this->right->llvm_codegen(compiler);
        std::cout<<"condoing"<<std::endl;

    if (!cond)
        return nullptr;
    // cond=compiler->builder.CreateSIToFP(cond,Type::getDoubleTy(*compiler->context));
    
//   // Convert condition to a bool by comparing non-equal to 0.0.
    Value* cast=compiler->builder.CreateSExt(cond,Type::getInt64Ty(*compiler->context));
    
    cond = compiler->builder.CreateICmpNE(cast,compiler->builder.getInt64(0),"condition");

    Function *TheFunction = compiler->builder.GetInsertBlock()->getParent();

    // Create blocks for the then and else cases.  Insert the 'then' block at the
    // end of the function.
    BasicBlock *ThenBB = BasicBlock::Create(*compiler->context, "then", TheFunction);
    BasicBlock *ElseBB = BasicBlock::Create(*compiler->context, "else",nullptr);
    BasicBlock *MergeBB = BasicBlock::Create(*compiler->context, "ifcont",nullptr);
    compiler->builder.CreateCondBr(cond, ThenBB, ElseBB);




    compiler->builder.SetInsertPoint(ThenBB);
    Value* if_part=this->mid->llvm_codegen(compiler);
    if (!if_part)
        return nullptr;
    compiler->builder.CreateBr(MergeBB);
    ThenBB = compiler->builder.GetInsertBlock();
    
    TheFunction->getBasicBlockList().push_back(ElseBB);
    compiler->builder.SetInsertPoint(ElseBB);
    Value* else_part=this->right->llvm_codegen(compiler);
    if (!else_part)
        return nullptr;
    compiler->builder.CreateBr(MergeBB);
    ElseBB = compiler->builder.GetInsertBlock();
    
    
    
    TheFunction->getBasicBlockList().push_back(MergeBB);
    compiler->builder.SetInsertPoint(MergeBB);


    
    PHINode *phi = compiler->builder.CreatePHI(Type::getInt64Ty(*compiler->context), 2, "iftmp");
    
    // if_part=compiler->builder.CreateSIToFP(if_part,Type::getDoubleTy(*compiler->context));
    // else_part=compiler->builder.CreateSIToFP(else_part,Type::getDoubleTy(*compiler->context));
    phi->addIncoming(if_part, ThenBB);
    phi->addIncoming(else_part, ElseBB);
    
    return phi;
}

Value *NodeScope::llvm_codegen(LLVMCompiler *compiler){
    std::cout<<"Hi"<<std::endl;
    SymbolTableTreeNode* current=new SymbolTableTreeNode(compiler->current_table);
    compiler->current_table=current;
    if(!this->mid){
        return nullptr;
    }
    Value* scope=this->mid->llvm_codegen(compiler);
    compiler->current_table=compiler->current_table->parent;
    return scope;
}

// Value* FunctNode::llvm_codegen(LLVMCompiler *compiler){
//     // std:: cout << "why?" <<  std::endl;
//     Type* type;
//     std::vector<llvm::Type*> param_types;
//     for (void* i : this->param_list->list){
//         NodeAssn* param=(NodeAssn*) i;
//         if(param->dtype=="short"){
//             type=compiler->builder.getInt16Ty();
//         }else if(param->dtype=="int"){
//             type=compiler->builder.getInt32Ty();

//         }else if(param->dtype=="long"){
//             type=compiler->builder.getInt64Ty();

//         }
//         param_types.push_back(type);

//     }
//     if(this->retType=="short"){
//         compiler->builder.getInt16Ty();
//     }else if(this->retType=="int"){
//         compiler->builder.getInt32Ty();

//     }else if(this->retType=="long"){
//         compiler->builder.getInt64Ty();

//     }
    
//     FunctionType *func_type = FunctionType::get(
//         type,
//         llvm::ArrayRef<llvm::Type*>(param_types),
//         false
//     );

//     Function* funct= Function::Create(func_type,GlobalValue::ExternalLinkage,this->identifier,&compiler->module);
//     if(this->identifier=="main"){
//         BasicBlock *func_entry_bb = BasicBlock::Create(
//                 *compiler->context,
//                 "entry",
//                 funct
//             );
//         compiler->builder.SetInsertPoint(func_entry_bb);
//     }
//     Value* ret=this->Program->llvm_codegen(compiler);
//     if(this->identifier=="main"){

//         compiler->builder.CreateRet(compiler->builder.getInt32(0));

//     }

//     return ret;
// }  

Value* FunctNode::llvm_codegen(LLVMCompiler *compiler){
    Type* type;

    std::vector<llvm::Type*> param_types;
    SymbolTableTreeNode* current=new SymbolTableTreeNode(compiler->current_table);
    compiler->current_table=current;
    std::vector<std::string> param_names;
    for (void* i : this->param_list->list){
        NodeAssn* param=(NodeAssn*) i;
        if(param->dtype=="short"){
            type=compiler->builder.getInt16Ty();
        }else if(param->dtype=="int"){
            type=compiler->builder.getInt32Ty();

        }else if(param->dtype=="long"){
            type=compiler->builder.getInt64Ty();

        }
        param_types.push_back(type);
        param_names.push_back(param->identifier);
    }

    if(this->retType=="short"){
        type=compiler->builder.getInt16Ty();
    }else if(this->retType=="int"){
        
        type=compiler->builder.getInt32Ty();

    }else if(this->retType=="long"){
        type=compiler->builder.getInt64Ty();

    }
    
    FunctionType *func_type;
    if(param_list->list.empty()){
        
        func_type = FunctionType::get(
        type,
        {},
        false
    );
    }else{

    func_type = FunctionType::get(
        type,
        llvm::ArrayRef<llvm::Type*>(param_types),
        false
    );
    }

    Function* funct= Function::Create(func_type,GlobalValue::ExternalLinkage,this->identifier,&compiler->module);
    
        BasicBlock *func_entry_bb = BasicBlock::Create(
                *compiler->context,
                "entry",
                funct
            );
        compiler->builder.SetInsertPoint(func_entry_bb);
    
    Value* ret=this->Program->llvm_codegen(compiler);
    

        compiler->builder.CreateRet(compiler->builder.getInt32(0));

    unsigned Idx = 0;
    for (auto &Arg : funct->args())
    Arg.setName(param_names[Idx++]);

    std::cout<<"Exited: "<<identifier<<" codegen!!"<<std::endl;
    compiler->current_table=current->parent;
    return ret;
}
#undef MAIN_FUNC
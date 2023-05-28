#include "ast.hh"

#include <string>
#include <vector>

NodeBinOp::NodeBinOp(NodeBinOp::Op ope, Node *leftptr, Node *rightptr) {
    type = BIN_OP;
    op = ope;
    left = leftptr;
    right = rightptr;
}

std::string NodeBinOp::to_string() {
    std::string out = "(";
    switch(op) {
        case PLUS: out += '+'; break;
        case MINUS: out += '-'; break;
        case MULT: out += '*'; break;
        case DIV: out += '/'; break;
    }

    out += ' ' + left->to_string() + ' ' + right->to_string() + ')';

    return out;
}

NodeTernaryOp::NodeTernaryOp(NodeTernaryOp::Op ope, Node *leftptr, Node *midptr, Node *rightptr) {
    type = TERN_OP;
    op = ope;
    left = leftptr;
    right = rightptr;
    mid = midptr;
}

std::string NodeTernaryOp::to_string() {
    std::string out = "(";
    switch(op) {

        case QMARK: out += "?:"; break;
    }

    out += ' ' + left->to_string() + ' ' + mid->to_string() + ' ' + right->to_string() + ')';

    return out;
}

NodeIf::NodeIf(Node *leftptr, Node *midptr, Node *rightptr) {
    type = IF;
    left = leftptr;
    right = rightptr;
    mid = midptr;
}

std::string NodeIf::to_string() {
    std::string out = "(";
    out += "if";
    out += ' ' + left->to_string() + ' ' + mid->to_string() + ' ' + "else" + ' ' + right->to_string() + ')';

    return out;
}

NodeInt::NodeInt(int val) {
    type = INT_LIT;
    // int val=0;
    value = val;
}

std::string NodeInt::to_string() {
    return std::to_string(value);
}

NodeStmts::NodeStmts() {
    type = STMTS;
    list = std::vector<Node*>();
}

void NodeStmts::push_back(Node *node) {
    list.push_back(node);
}




std::string NodeStmts::to_string() {
    std::string out = "(begin";
    for(auto i : list) {
        out += " " + i->to_string();
    }

    out += ')';

    return out;
}

NodeScope::NodeScope(NodeStmts* mid){
    this->mid=mid;
}

std::string NodeScope::to_string(){
    std::string out;
    out=out+"{";
    out=out+mid->to_string();
    out=out+"}";
    return out;
}

NodeAssn::NodeAssn(std::string id, Node *expr, std::string datatype) {
    type = ASSN;
    identifier = id;
    expression = expr;
    dtype  = datatype;
}

std::string NodeAssn::to_string() {
    return "(let " + identifier + " : " + dtype + " " + expression->to_string() + ")";
}

NodeAssign::NodeAssign(std::string id, Node *expr) {
    type = ASSIGN;
    identifier = id;
    expression = expr;
}

std::string NodeAssign::to_string() {
    return "(assign " + identifier + " " + expression->to_string() + ")";
}


NodeDebug::NodeDebug(Node *expr) {
    type = DBG;
    expression = expr;
}

std::string NodeDebug::to_string() {
    return "(dbg " + expression->to_string() + ")";
}

NodeIdent::NodeIdent(std::string ident) {
    identifier = ident;
}
std::string NodeIdent::to_string() {
    return identifier;
}

FunctNode::FunctNode(std::string ident, NodeStmts* param_list, NodeStmts* Program, std::string datatype){
    this->param_list=param_list;
    identifier=ident;
    this->Program=Program;
    this->retType=datatype;

}

std::string FunctNode::to_string(){

    return "(fun " + identifier + " (" + param_list->to_string()+ ") : " + retType + "\n{"+Program->to_string()+"\n}" + ")";
}
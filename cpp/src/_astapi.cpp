#include "_astapi.h"
#include "_ast.hpp"
#include "crobj.hpp"

extern "C" {
    
    CRobjHandle* ASTnode_CRmake(ASTnodeHandle* hnode, double x, double h){
        ASTnode* node = reinterpret_cast<ASTnode*>(hnode);
        try {
            CRobj* cr = node->crmake(x,h);
            return reinterpret_cast<CRobjHandle*>(cr);
        } catch (...){
            return nullptr;
        }
    }

    ASTnodeHandle* ASTnode_NewNum(double value) {
        return reinterpret_cast<ASTnodeHandle*>(new ASTnum(value));
    }

    ASTnodeHandle* ASTnode_NewVar() {
        return reinterpret_cast<ASTnodeHandle*>( new ASTvar());
    }



    ASTnodeHandle* ASTnode_NewBin(bt o, ASTnodeHandle* left, ASTnodeHandle* right){
        auto L = reinterpret_cast<ASTnode*>(left);
        auto R = reinterpret_cast<ASTnode*>(right); 
        return reinterpret_cast<ASTnodeHandle*>(new ASTbin(static_cast<bt>(o), L, R));
    }

    void ASTnode_Destroy(ASTnodeHandle* node) {
        delete reinterpret_cast<ASTnode*>(node);
    }

    void ASTnode_Init(ASTnodeHandle* node, double x, double h) {
        auto n = reinterpret_cast<ASTnode*>(node);
        n->crinit(x, h);
    }

    CRobjHandle* ASTnode_Crmake(ASTnodeHandle* node,double x, double h) {
        auto n = reinterpret_cast<ASTnode*>(node);
        try {
            CRobj* cr = n->crmake(x, h);
            return reinterpret_cast<CRobjHandle*>(cr);
        } catch (...) {
            return nullptr;
        }
    }

    void CRobj_Destroy(CRobjHandle* obj) {
        delete reinterpret_cast<CRobj*>(obj);
    }

}
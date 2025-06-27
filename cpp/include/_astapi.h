#pragma once 
#include "_ast.hpp"
#ifdef __cplusplus
extern "C" {
    #endif
    typedef struct ASTnode_ ASTnodeHandle;
    typedef struct CRobj_ CRobjHandle;
    
    CRobjHandle* ASTnode_CRmake(ASTnodeHandle* node, double x, double h);
    void CRobj_Destroy(CRobjHandle* obj);
    void ASTnode_Init(ASTnodeHandle* node, double x, double h);
    
    ASTnodeHandle* ASTnode_NewNum(double value);
    ASTnodeHandle* ASTnode_NewVar();


    ASTnodeHandle* ASTnode_NewBin(bt o, ASTnodeHandle* left, ASTnodeHandle* right);
    void ASTnode_Destroy(ASTnodeHandle* node);

    #ifdef __cplusplus
} 
#endif


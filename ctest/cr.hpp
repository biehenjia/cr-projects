# pragma once 
#include <vector>
#include <memory>
#include <algorithm>

// AST supported types
typedef enum OPTYPE {
    ADD, SUB, MUL, DIV, POW,
    EXP, LOG,
    SIN, COS, TAN, COT,
    // factorial
    FCT
};

// CR supported types
typedef enum CRTYPE { 
    CRSUM, CRPROD, CREXPR, CRTRIG, CRNUM
}; 


class CRnode {
    public:
        CRnode(double v) noexcept : type(CRNUM), value(v) {};
        CRnode (int i, double v1, double v2) : type(CRSUM), index(i), fastvalues{v1,v2} {};
        CRnode (int i, int l, CRTYPE t): type(t), index(i), fastvalues(l), operands(l) {};

        CRTYPE type;
        int index;
        bool initialized;
        double value;

        std::vector<double> fastvalues;
        std::vector<double> canshift;
        std::vector<std::unique_ptr<CRnode>> operands;

        inline std::unique_ptr<CRnode> copy() const noexcept;
        // TODO: optimize for cone DP;
        inline void shift(int index) const noexcept;

        std::unique_ptr<CRnode> add(const CRnode& t) const noexcept;
        std::unique_ptr<CRnode> mul(const CRnode& t) const noexcept;
        std::unique_ptr<CRnode> pow(const CRnode& t) const noexcept;
        std::unique_ptr<CRnode> exp() const noexcept;
        std::unique_ptr<CRnode> ln() const noexcept;
        std::unique_ptr<CRnode> sin() const noexcept;
        std::unique_ptr<CRnode> cos() const noexcept;
};

class ASTnode {
    public:
        CRTYPE type;
        double value;
        int index;

        std::unique_ptr<ASTnode> left;
        std::unique_ptr<ASTnode> right;
        
        std::unique_ptr<CRnode> crmake();
};





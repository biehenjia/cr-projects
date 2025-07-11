#include "crobj.hpp"
#pragma once 

enum class bt {ADD, SUB, MUL, DIV, POW};
enum class ut {NEG, FAC, EXP, LN, SIN, COS, TAN, COT};

class ASTnode {
    public:
        //default
        ASTnode() {};
        virtual ~ASTnode() = default;
        virtual CRobj* crmake() = 0;
        ASTnode* left;
        ASTnode* right;
        // exposed
        void crinit(std::vector<size_t>);

        std::vector<double> result;
        std::vector<size_t> params;

        void _creval(const CRobj& c, size_t i);
        std::vector<double> creval();

        CRobj* cr;
        virtual void view();
        
};

class ASTnum : public ASTnode {
    public:
        ASTnum(double v) : value(v) {};
        double value;
        CRobj* crmake() override;
        void view() override;
};

class ASTvar : public ASTnode {
    public:
        ASTvar(size_t i, double x, double h) : index(i), start(x), step(h) {};
        CRobj* crmake() override;
        void view() override;
        size_t index;
        double start;
        double step;

};  


class ASTbin : public ASTnode { 
    public:
        ASTbin(bt o, ASTnode* l, ASTnode* r) {
            left = l;
            right = r;
            optype = o;
        };
        ~ASTbin();
        CRobj* crmake() override;
        bt optype;
        void view() override;
};

class ASTun : public ASTnode {
    public:
        ASTun(ut o, ASTnode* l){
            left = l;
            optype = o;
        }
        ~ASTun();
        CRobj* crmake() override;
        ut optype;
        void view() override;
};






#pragma once 
#include "crobj.hpp"

enum class bt {ADD, SUB, MUL, DIV, POW};
enum class ut {NEG, FAC, EXP, LN, SIN, COS};

class ASTnode {
    public:
        //default
        ASTnode() {};
        virtual ~ASTnode() = default;
        virtual CRobj* crmake(double x, double h) = 0;
        ASTnode* left;
        ASTnode* right;
        // exposed
        void crinit(double x, double h);
        std::vector<double> creval(double q);
        CRobj* cr;

        virtual void view();
        
};

class ASTnum : public ASTnode {
    public:
        ASTnum(double v) : value(v) {};
        double value;
        CRobj* crmake(double x, double h) override;
        void view() override;
};

class ASTvar : public ASTnode {
    public:
        ASTvar() {};
        CRobj* crmake(double x, double h) override;
        void view() override;

};  


class ASTbin : public ASTnode { 
    public:
        ASTbin(bt o, ASTnode* l, ASTnode* r) {
            left = l;
            right = r;
            optype = o;
        };
        ~ASTbin();
        CRobj* crmake(double x, double h) override;
        bt optype;
        void view() override;
};

class ASTun : public ASTnode {
    public:
        ASTun(ut o, ASTnode* l){
            left = l;
        }
        ~ASTun();
        CRobj* crmake(double x, double h) override;
        ut optype;
        void view() override;
};




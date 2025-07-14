#include "crobj.hpp"
#pragma once 

enum class bt {ADD, SUB, MUL, DIV, POW};
enum class ut {NEG, FAC, EXP, LN, SIN, COS, TAN, COT};

class ASTnode {
    public:
        //default
        ASTnode() {};
        virtual ~ASTnode() = default;
        virtual std::unique_ptr<CRobj> crmake() = 0;
        std::unique_ptr<ASTnode> left;
        std::unique_ptr<ASTnode> right;
        // exposed
        void crinit(std::vector<size_t>);

        std::vector<double> result;
        std::vector<size_t> params;

        void _creval(const CRobj& c, size_t i);
        std::vector<double> creval();

        std::unique_ptr<CRobj> cr;
        virtual void view();
        
};

class ASTnum : public ASTnode {
    public:
        ASTnum(double v) : value(v) {};
        double value;
        std::unique_ptr<CRobj> crmake() override;
        void view() override;
};

class ASTvar : public ASTnode {
    public:
        ASTvar(size_t i, double x, double h) : index(i), start(x), step(h) {};
        std::unique_ptr<CRobj> crmake() override;
        void view() override;
        size_t index;
        double start;
        double step;

};  


class ASTbin : public ASTnode { 
    public:
        ASTbin(bt o, const ASTnode& l, const ASTnode& r) {
            left = l;
            right = r;
            optype = o;
        };
        std::unique_ptr<CRobj> crmake() override;
        bt optype;
        void view() override;
};

class ASTun : public ASTnode {
    public:
        ASTun(ut o, const ASTnode& l){
            left = std::unique_ptr<ASTnode>(l);
            optype = o;
        }
        ~ASTun();
        std::unique_ptr<CRobj> crmake() override;
        ut optype;
        void view() override;
};






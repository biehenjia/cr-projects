#include "crexpr.hpp"



CRexpr::CRexpr(oc ot, const CRobj& o1) {
    optype = ot;
    length = 1; 
    operands.resize(1);
    operands[0] = o1.copy();
}

CRexpr::CRexpr(oc ot, const CRobj& o1, const CRobj& o2) {
    optype = ot;
    length = 2;
    operands.resize(2);
    operands[0] = o1.copy();
    operands[1] = o2.copy();
}

CRexpr::CRexpr(oc ot, size_t l){
    optype = ot;
    length = l;
    operands.resize(length);
}

CRobj* CRexpr::add(const CRobj&) const {
    CRobj* left = operands[0]->copy();
    CRobj* right = operands[1]->copy();
    CRexpr* result = new CRexpr(oc::ADD, *left, *right);
    return result;
}

CRobj* CRexpr::mul(const CRobj&) const {
    std::cout<<"here! mul\n";
    CRobj* left = operands[0]->copy();
    CRobj* right = operands[1]->copy();
    CRexpr* result = new CRexpr(oc::MUL, *left, *right);
    return result;
}

CRobj* CRexpr::pow(const CRobj&) const { 
    std::cout<<"here! pow\n";
    CRobj* left = operands[0]->copy();
    CRobj* right = operands[1]->copy();
    CRexpr* result = new CRexpr(oc::POW, *left, *right);
    return result;
}

CRobj* CRexpr::exp() const {
    CRobj* left = operands[0]->copy();
    CRexpr* result = new CRexpr(oc::EXP, *left);
    return result;
}

CRobj* CRexpr::ln() const { 
    CRobj* left = operands[0]->copy();
    CRexpr* result = new CRexpr(oc::LN, *left);
    return result;
}

CRobj* CRexpr::sin() const { 
    CRobj* left = operands[0]->copy();
    CRexpr* result = new CRexpr(oc::SIN, *left);
    return result;
}

CRobj* CRexpr::cos() const {
    CRobj* left = operands[0]->copy();
    CRexpr* result = new CRexpr(oc::COS, *left);
    return result;
}

CRobj* CRexpr::copy() const{
    auto result = new CRexpr(optype, length);
    for (size_t i = 0; i < length; i++){ 
        result->operands[i] = operands[i]->copy(); 
    }

    if (initialized){
        result->fastvalues.resize(length);
        result->isnumbers.resize(length);
    }

    for (size_t i = 0; i < length; i++){ 
        result->fastvalues[i] = fastvalues[i];
        result->isnumbers[i] = isnumbers[i];
    }
    return result;
}


double CRexpr::valueof() const{
    double result = 0;
    switch (optype) {
        case oc::ADD:
            result = operands[0]->valueof() + operands[1]->valueof();
            break;
        
        case oc::MUL:
            result = operands[0]->valueof() * operands[1]->valueof();
            break;
        
        case oc::POW:
            result = std::pow(operands[0]->valueof() ,operands[1]->valueof());
            break;
        case oc::EXP:
            result = std::exp(operands[0]->valueof());
            break;
        case oc::LN:
            result = std::log(operands[0]->valueof());
            break;
        case oc::SIN:
            result = std::sin(operands[0]->valueof());
            break;
        case oc::COS:
            result = std::cos(operands[0]->valueof());
            break;
    }
    return result; 
}

void CRexpr::shift() {
    for (size_t i = 0; i < length; i++){
        if (!isnumbers[i]){
            operands[i]->shift();
            fastvalues[i] = operands[i]->valueof();
        }
    }
}
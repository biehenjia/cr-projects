#include "crexpr.hpp"

CRexpr::CRexpr(oc ot, const CRobj& o1) {
    optype = ot;
    length = 1; 
    operands.resize(1);
    index = o1.index;
    operands[0] = o1.copy();
}

CRexpr::CRexpr(oc ot, const CRobj& o1, const CRobj& o2) {
    optype = ot;
    length = 2;
    operands.resize(2);
    index = o1.index > o2.index ? o1.index : o2.index;
    operands[0] = o1.copy();
    operands[1] = o2.copy();
}

CRexpr::CRexpr(oc ot, size_t l){
    optype = ot;
    length = l;
    operands.resize(l);
}

std::unique_ptr<CRobj> CRexpr::add(const CRobj& target) const {
    auto left = copy();
    auto right = target.copy();
    auto result = std::make_unique< CRexpr>(oc::ADD, *left, *right);
    return result;
}

std::unique_ptr<CRobj> CRexpr::mul(const CRobj& target) const {
    auto left = copy();
    auto right = target.copy();
    auto result = std::make_unique< CRexpr>(oc::MUL, *left, *right);
    return result;
}

std::unique_ptr<CRobj> CRexpr::pow(const CRobj& target) const { 
    auto left = copy();
    auto right = target.copy();
    auto result = std::make_unique< CRexpr>(oc::POW, *left, *right);
    return result;
}

std::unique_ptr<CRobj> CRexpr::exp() const {
    auto left = operands[0]->copy();
    auto result = std::make_unique< CRexpr>(oc::EXP, *left);
    return result;
}

std::unique_ptr<CRobj> CRexpr::ln() const { 
    auto left = operands[0]->copy();
    auto result = std::make_unique< CRexpr>(oc::LN, *left);
    return result;
}

std::unique_ptr<CRobj> CRexpr::sin() const { 
    auto left = operands[0]->copy();
    auto result = std::make_unique< CRexpr>(oc::SIN, *left);
    return result;
}

std::unique_ptr<CRobj> CRexpr::cos() const {
    auto left = operands[0]->copy();
    auto result = std::make_unique< CRexpr>(oc::COS, *left);
    return result;
}

std::unique_ptr<CRobj> CRexpr::copy() const{
    auto result = std::make_unique< CRexpr>(optype, length);
    result->index = index;
    for (size_t i = 0; i < length; i++){ 
        result->operands[i] = operands[i]->copy(); 
    }

    if (initialized){
        result->initialized = true;
        result->fastvalues.resize(length);
        result->isnumbers.resize(length);
        for (size_t i = 0; i < length; i++){ 
            result->fastvalues[i] = fastvalues[i];
            result->isnumbers[i] = isnumbers[i];
        }
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

void CRexpr::shift(size_t i) {
    
    for (size_t j = 0; j < length; j++){
        if (!isnumbers[j]){
            operands[j]->shift(i);
            fastvalues[j] = operands[j]->valueof();
        }
    }
}



void CRexpr::print_tree() const { 
    std::cout <<"CRexpr"<<"["<<valueof()<<"]"<<"(";
    for (size_t i = 0; i < operands.size(); i++){
        operands[i]->print_tree();

        if (i+1 < operands.size()){ 
            std::cout<<", ";
        }
    } std::cout<<")";
}

std::string CRexpr::genCode(size_t parent, size_t order, ssize_t place,std::string indent) const {
    std::string res = "";
    
    for (size_t i = 0; i < operands.size(); i++){ 
        if (!operands[i]->isnumber()){
            res += operands[i]->genCode(crposition, order, i, indent);
        }
    }
    
    if (place != -1 && res.size()){
        res += std::format("{}{}[{}]={}{}[0] + {}{}[1]\n",
            crprefix,parent,place,
            crprefix,crposition,
            crprefix,crposition
        );
        std::cout<<std::format("{}{}[{}]={}{}[0] + {}{}[1]\n",
            crprefix,parent,place,
            crprefix,crposition,
            crprefix,crposition
        );
    }
    return res;
}
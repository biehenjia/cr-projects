#include "crnum.hpp"
#include "crsum.hpp"
#include "crexpr.hpp"
#include "crprod.hpp"

CRnum::CRnum(double v){
    std::cout<<"init CRnum "<<v<<"\n";
    value = v;
}

CRobj* CRnum::add(const CRobj& target) const  { 

    if (auto p = dynamic_cast<const CRnum*>(&target)){
        return new CRnum(this->value + p->value);
    } else if (auto p = dynamic_cast<const CRsum*>(&target)){
        return p->add(*this);
    } 
    return new CRexpr(oc::ADD, *this->copy(), *target.copy());
    
}

CRobj* CRnum::mul(const CRobj& target) const { 
    if (auto p= dynamic_cast<const CRnum*>(&target)){
        return new CRnum(this->value * p->value);
    } else if (auto p = dynamic_cast<const CRsum*>(&target)){
        return p->mul(*this);
    } 
    return new CRexpr(oc::MUL, *this->copy(), *target.copy());
    
}

CRobj* CRnum::pow(const CRobj& target) const {
    if (auto p= dynamic_cast<const CRnum*>(&target)){
        return new CRnum(std::pow(this->value,p->value));
    } else if (auto p = dynamic_cast<const CRsum*>(&target)){
        return p->mul(*this);
    }
    return new CRexpr(oc::MUL, *this->copy(), *target.copy());
}


CRnum* CRnum::ln() const { 
    return new CRnum(std::log(value));
}

CRnum* CRnum::sin() const { 
    return new CRnum(std::sin(value));
}

CRnum* CRnum::cos() const { 
    return new CRnum(std::cos(value));
}

CRnum* CRnum::copy() const{
    return new CRnum(this->value);
}

bool CRnum::isnumber() const {
    return false;
}

CRnum* CRnum::exp() const { 
    return new CRnum(std::exp(value ));
}

double CRnum::initialize(){
    return value;
}

double CRnum::valueof() const { 
    return value; 
}


void CRnum::simplify() {
    return;
}

void CRnum::shift(){
    return;
}
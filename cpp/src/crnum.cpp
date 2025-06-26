#include "crnum.hpp"
#include "crsum.hpp"
#include "crexpr.hpp"

CRnum::CRnum(double v){
    value = v;
}

CRobj* CRnum::add(const CRobj& target) const { 
    return new CRexpr(oc::ADD, *this->copy(), *target.copy());
}
CRobj* CRnum::mul(const CRobj& target) const { 
    return new CRexpr(oc::MUL, *this->copy(), *target.copy());
}

CRobj* CRnum::pow(const CRobj& target) const { 
    return new CRexpr(oc::POW, *this->copy(), *target.copy());
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




CRnum* CRnum::add(const CRnum& target) const {
    return new CRnum(this->value + target.value);
}

CRsum* CRnum::add(const CRsum& target) const {
    return target.add(*this);
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
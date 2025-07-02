#include "crprod.hpp"
#include "crexpr.hpp"
#include "crnum.hpp"
#include "crsum.hpp"

CRprod::CRprod(size_t l){
    length = l;
    operands.resize(l,nullptr);
}

CRobj* CRprod::add(const CRobj& target) const {
    return new CRexpr(oc::ADD, *this->copy(), *target.copy());
}

CRobj* CRprod::mul(const CRobj& target) const {
    if (auto p = dynamic_cast<const CRnum*>(&target) ){
        auto result = this->copy();
        auto temp = new CRnum(result->operands[0]->valueof() * target.valueof());
        delete result->operands[0];
        result->operands[0] = temp;
        result->simplify();
        return result;
    } else if (auto p = dynamic_cast<const  CRprod*> (&target)){
        size_t newlength = std::max(length, p->length);
        auto result = new CRprod(newlength);
        for (size_t i = 0; i < newlength; ++i) {
            double a = (i < length) ? this->operands[i]->valueof() : 1.0;
            double b = (i < target.length) ? p->operands[i]->valueof() : 1.0;
            result->operands[i] = new CRnum(a * b);
        }
        result->simplify();
        return result;
    }
    return new CRexpr(oc::MUL, *this->copy(), *target.copy());
}

CRobj* CRprod::pow(const CRobj& target) const { 
    if (auto p = dynamic_cast<const CRnum*>(&target) ){
        auto result = this->copy();
        auto temp = new CRnum(result->operands[0]->valueof() * target.valueof());
        delete result->operands[0];
        result->operands[0] = temp;
        result->simplify();
        return result;
    } else if (auto p = dynamic_cast<const  CRsum*> (&target)){
        size_t newlength = length + p->length -1;
        size_t n,m;
        auto result = new CRprod(newlength);
        if (length > p->length){ 
            m = length;
            n = p->length;
        } else {
            m = p->length;
            n = length;
        }
        for (size_t i = 0; i < newlength; i++){
            double crs1 = 1.0;
            size_t bound11 = std::max(size_t(0), i - n);
            size_t bound12 = std::min(i, m);

            for (size_t j = bound11; j < bound12+1; j++){
                double crs2 = 1.0;
                size_t bound21 = i-j;
                size_t bound22 = std::min(i, n);

                for (size_t k = bound21; k < bound22 +1; k++){
                    double crt1 = p->operands[k]->valueof() * choose(j,i-k);
                    double crt2 = crt1 * choose(i,j);
                    double crt3 = std::pow(operands[j]->valueof(),crt2);

                    double crt = crs2 * crt3;
                    crs2 = crt;
                }

                double crt4 = crs2 * crs1;
                crs1 = crt4;
            }
            result->operands[i] = new CRnum(crs1);
        }
        result->simplify();
        return result;
    }
    return new CRexpr(oc::POW, *this->copy(), *target.copy());
}

void CRprod::simplify() {
    size_t found = length;
    for (size_t i = length -1; i > 0; i--){ 
        if (operands[i]->valueof() == 0){
            found = i-1;
        }
    }
    // note: will be (a1^a2^...^ak^0^...an) = (1)^...^an = 1
    if (found < length){
        operands.resize(1);
        auto temp = new CRnum(1);
        length = 1;
        delete operands[0];
        operands[0] = temp;
    } else {
        size_t p = 0;
        for (size_t i = 0; i < length; i++){
            while (p < i && operands[p]->valueof() != 1){
                p++;
            }
            if (operands[i]->valueof() != 1){
                std::swap(operands[i],operands[p]);
            }
        }
        for (size_t i = p; i < length; i++){
            delete operands[i];
        }
        if (p == 0){
            operands.assign(1,new CRnum(1));
            length = 1;
        } else{ 
            operands.resize(p);
            length = p;
        }

    }
}

void CRprod::shift() { 
    for (size_t i = 0; i < length-1; i++){
        fastvalues[i] *= fastvalues[i+1 ];
    }
}

CRobj* CRprod::exp() const { 
    return new CRexpr(oc::EXP, *this->copy());
}

CRobj* CRprod::ln() const { 
    auto result = new CRsum(length);

    for (size_t i = 0; i < length; i++){ 
        auto temp = new CRnum(std::log(operands[i]->valueof()));
        result->operands[i] = temp;
    }
    result->simplify();
    return result;

}

CRobj* CRprod::sin() const { 
    return new CRexpr(oc::SIN, *this->copy());
}

CRobj* CRprod::cos() const { 
    return new CRexpr(oc::COS, *this->copy());
}

CRprod* CRprod::copy() const{
    auto result = new CRprod(length);
    for (size_t i = 0; i < length; i++ ){ 
        result->operands[i] = operands[i]->copy(); 
    }

    if (initialized){
        result->fastvalues.resize(length);
        result->isnumbers.resize(length);
        for (size_t i = 0; i < length; i++){ 
            result->fastvalues[i] = fastvalues[i];
            result->isnumbers[i] = isnumbers[i];
        }
    }

    
    return result;
}


CRobj* CRprod::correctp(size_t nl) const{
    auto result = copy();
    result->operands.resize(nl);
    for (size_t i = length; i< nl; i++){ 
        result->operands[i] = new CRnum(1.0);
    }
    return result;
}
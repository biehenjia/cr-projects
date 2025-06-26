#include "crprod.hpp"
#include "crexpr.hpp"
#include "crnum.hpp"
#include "crsum.hpp"

CRprod::CRprod(size_t l){
    length = l;
    operands.reserve(l);
}

CRobj* CRprod::add(const CRobj& target) const {
    return new CRexpr(oc::ADD, *this->copy(), *target.copy());
}

CRobj* CRprod::mul(const CRobj& target) const {
    return new CRexpr(oc::MUL, *this->copy(), *target.copy());

}

CRprod* CRprod::mul(const CRnum& target) const{
    auto result = this->copy();
    auto temp = new CRnum(result->operands[0]->valueof() * target.valueof());
    delete result->operands[0];
    result->operands[0] = temp;
    result->simplify();
    return result;
};

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

CRprod* CRprod::mul(const CRprod& target) const { 
    if (length < target.length){
        auto result = new CRprod(target.length);
        for (size_t i = 0; i < length; i++){
            result->operands[i] = new CRnum(this->operands[i]->valueof() * target.operands[i]->valueof());
        }
        for (size_t i = length; i < target.length; i++){
            result->operands[i] = new CRnum(target.operands[i]->valueof());
        }
        result->simplify(); 
        return result;
    } else {
        auto result = new CRprod(length);
        for (size_t i = 0; i < target.length; i++){ 
            result->operands[i] = new CRnum(this->operands[i]->valueof() * target.operands[i]->valueof());
        }
        for (size_t i = target.length; i < length; i++){
            result->operands[i] = new CRnum(this->operands[i]->valueof());
        }
        result->simplify();
        return result;
    }
}

CRobj* CRprod::pow(const CRobj& target) const { 
    return new CRexpr(oc::POW, *this->copy(), *target.copy());
}

CRprod* CRprod::pow(const CRnum& target) const { 
    auto result = new CRprod(length);
    for (size_t i = 0; i < length; i++){
        result->operands[i] = new CRnum(std::pow(operands[i]->valueof(),target.valueof() ));
    }
    result->simplify();
    return result;
}

CRprod* CRprod::pow(const CRsum& target) const{
    double newlength = length + target.length -1;
    double n,m;
    auto result = new CRprod(newlength);
    if (length > target.length){ 
        m = length;
        n =target.length;
    } else {
        m = target.length;
        n = length;
    }
    for (double i = 0; i < newlength; i++){
        double crs1 = 1.0;
        double bound11 = std::max(0.0, i - n);
        double bound12 = std::min(i, m);

        for (double j = bound11; j < bound12+1; j++){
            double crs2 = 1.0;
            double bound21 = i-j;
            double bound22 = std::min(i, n);

            for (double k = bound21; k < bound22 +1; k++){
                double crt1 = target.operands[i]->valueof() * choose(i,i-k);
                double crt2 = crt1 * choose(i,j);
                double crt3 = std::pow(operands[i]->valueof(),crt2);

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

};

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
    }

    for (size_t i = 0; i < length; i++){ 
        result->fastvalues[i] = fastvalues[i];
        result->isnumbers[i] = isnumbers[i];
    }
    return result;
}


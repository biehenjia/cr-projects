#include "crsum.hpp"
#include "crnum.hpp"
#include "crexpr.hpp"
#include "crprod.hpp"
#include "crtrig.hpp"


CRsum::CRsum(size_t l){
    length = l;
    operands.resize(length,nullptr);
}

CRsum::CRsum(double x, double h){ 
    length = 2;
    operands = {new CRnum(x), new CRnum(h)};
}


CRsum* CRsum::copy() const{
    auto result = new CRsum(length);
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

CRobj* CRsum::add(const CRobj &target) const { 
    if (auto p = dynamic_cast<const CRsum*>(&target)){
        size_t maxLen = std::max(length, p->length);
        auto result = new CRsum(maxLen);
        for (size_t i = 0; i < maxLen; ++i) {
            double a = (i < length) ? this->operands[i]->valueof(): 0.0;
            double b = (i < target.length) ? p->operands[i]->valueof() : 0.0;
            result->operands[i] = new CRnum(a + b);
        }
        //std::cout<< "add called!\n";
        
        result->simplify();
        // for (size_t i = 0; i < result->operands.size(); i++){
        //         std::cout<< result->operands[i]->valueof() << " ";
        //     }
        //     std::cout<<"\n";
        return result;
    } else if (auto p = dynamic_cast<const CRnum*>(&target)){
        auto result = this->copy();
        auto temp = new CRnum(result->operands[0]->valueof() + target.valueof());
        delete result->operands[0];
        result->operands[0] = temp;
        result->simplify();
        return result;
    } 
    return new CRexpr(oc::ADD, *this->copy(), *target.copy());
}

CRobj* CRsum::mul(const CRobj&target )const {
    //std::cout<<"mul called from crsum \n";
    if (auto p = dynamic_cast<const CRnum*>(&target)) {
        auto result = new CRsum(length);
        for (size_t i = 0; i < length; i++){ 
            result->operands[i] = new CRnum(operands[i]->valueof() * target.valueof());
        }
        result->simplify(); 
        return result;
    } else if (auto p = dynamic_cast<const CRsum*> (&target)){
        if (length >= target.length){
            size_t  newlength = length + target.length -1;
            auto result = new CRsum(newlength);
            double rtemp2,r1;
            size_t n = length - 1;
            size_t m = target.length -1;
            for (size_t i= 0; i < newlength; i++){
                double r1 = 0;
                size_t ibound11 = (i > m ? i - m : 0);
                size_t ibound12 = std::min(i, n);
                for (size_t j = ibound11; j < ibound12+1; j++){
                    double r2 = 0;
                    size_t ibound21 = (i > j  ? i - j : 0);
                    size_t ibound22 = std::min(i,m);
                    for (size_t k = ibound21; k < ibound22+1; k++){
                        double rtemp1 = choose(j,i-k);
                        double rtemp11 = target.operands[k]->valueof(); 
                        rtemp1 *= rtemp11;
                        r2 += rtemp1;
                    }
                    double rtemp2 = choose(i, j);  
                    r2 *= rtemp2;
                    r2 *= this->operands[j]->valueof(); 
                    r1 += r2;   
                }
                //std::cout<< "CALLED MUL\n";

                result->operands[i] = new CRnum(r1);
                
            }
            result->length = newlength;
            // for (size_t i = 0; i < result->operands.size(); i++){
            //     std::cout<< result->operands[i]->valueof() << " ";
            // }
            // std::cout<<"\n";
            result->simplify();
            return result;
        } else { 
            return target.mul(*this);
        }
    }
    return new CRexpr(oc::MUL, *this->copy(), *target.copy());

}


CRobj* CRsum::pow(const CRobj& target) const { 
    //std::cout<<"pow called from crsum\n";
    if (auto p = dynamic_cast<const CRnum*>(&target)) {
        double pv = p->valueof();
        //std::cout<<"mul type\n";
        if (pv >= 0 && std::floor(pv) == pv) {
            size_t exp = size_t(pv);
            CRobj* result = new CRnum(1.0); 
            CRobj* base = copy(); 
            while (exp > 0) {
                //std::cout << "exp = " << exp << std::endl;

                if (exp & 1) {
                    CRobj* tmp = result->mul(*base);
                    delete result;
                    result = tmp;
                }
                exp >>= 1;
                if (exp) {
                    CRobj* tmp = base->mul(*base);
                    delete base;
                    base = tmp;
                }
                // for (size_t i = 0; i < base->operands.size(); i++){
                //     std::cout<< base->operands[i]->valueof() << " ";
                // }
                // std::cout<<"\n";
                
            }
            delete base;
            //std::cout<<"at the end...\n";
            // for (size_t i = 0; i < result->operands.size(); i++){
            //     std::cout<< result->operands[i]->valueof() << " ";
            // }
            // std::cout<<"\n";
            //result->simplify();
            return result;
        }
    }
    
    return new CRexpr(oc::POW, *this->copy(), *target.copy());
}


void CRsum::simplify() {
    size_t j = length - 1;
    while (j > 0 && operands[j]->valueof() == 0){
        delete operands[j];
        j--;
    }

    operands.resize(j+1);
}

void CRsum::shift() {
    for (size_t i = 0; i < length-1; i++){
        fastvalues[i] += fastvalues[i+1];
    }
}

CRobj* CRsum::exp() const {
    auto result = new CRprod(length);
    for (size_t i = 0; i< length; i++){ 
        result->operands[i] = new CRnum(std::exp(operands[i]->valueof()));
    }
    result->simplify();
    return result;
}

CRobj* CRsum::ln() const {
    return new CRexpr(oc::LN, *this->copy());
}

CRobj* CRsum::sin() const { 
    auto result = new CRtrig(oc::SIN, length* 2);
    for (size_t i = 0; i < length; i++){ 
        result->operands[i] = new CRnum(std::sin(operands[i]->valueof()));
        result->operands[i+length] = new CRnum(std::cos(operands[i]->valueof()));
    }
    return result;
}

CRobj* CRsum::cos() const { 
    auto result = new CRtrig(oc::COS, length* 2);
    for (size_t i = 0; i < length; i++){ 
        result->operands[i] = new CRnum(std::sin(operands[i]->valueof()));
        result->operands[i+length] = new CRnum(std::cos(operands[i]->valueof()));
    }
    return result;
}
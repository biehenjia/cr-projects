#pragma once
#include <vector>
#include <algorithm>
#include <cmath>

//predefine
class CRsum ;
class CRnum;
class CRprod ;
class CRtrig ;
class CRexpr ;

//nCk
inline double choose(double n,double k){
    double result = 1;
    for (size_t i = 1; i <= k; i++){
        result *= (n-k+i)/i;
    }
    return result;
}


// n!

// operation code
enum class oc {
    ADD,
    MUL,
    POW,
    EXP,
    LN,
    SIN,
    COS
};


class CRobj {
    public:
        CRobj(){};
        CRobj(size_t l);
        virtual ~CRobj();

        virtual CRobj* addto(const CRobj& target) const = 0;
        virtual CRobj* multo(const CRobj& target) const = 0;
        virtual CRobj* powto(const CRobj& target) const = 0;
        virtual CRobj* rpow(const CRobj& target) const = 0;

        virtual CRobj* add(const CRobj& target) const = 0;
        virtual CRobj* mul(const CRobj& target) const= 0;
        virtual CRobj* pow(const CRobj& target) const= 0;

        virtual CRobj* exp() const= 0;
        virtual CRobj* ln() const= 0;
        virtual CRobj* sin()  const= 0;
        virtual CRobj* cos() const = 0;

        virtual void simplify();
        virtual CRobj* copy() const = 0;

        virtual double initialize();
        virtual double valueof() const;
        virtual bool isnumber() const;

        virtual void shift();
    
        std::vector<CRobj*> operands;

        std::vector<double> fastvalues;
        std::vector<bool> isnumbers;
        size_t length;
        bool initialized = false;
    
};
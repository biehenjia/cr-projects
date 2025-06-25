#pragma once
#include <vector>
#include <algorithm>

//predefine
class CRsum;
class CRnum;
class CRprod;
class CRtrig;
class CRexpr;

//nCk
double choose(double n,double k){
    double result = 1;
    for (size_t i = 1; i <= k; i++){
        result *= (n-k+i)/i;
    }
    return result;
}


// n!


class CRobj {
    public:
        CRobj();
        virtual ~CRobj();

        virtual CRobj* add(const CRobj& target) const = 0;
        virtual CRobj* mul(const CRobj& target) const= 0;
        virtual CRobj* pow(const CRobj& target) const= 0;

        virtual CRobj* exp() const= 0;
        virtual CRobj* ln() const= 0;
        virtual CRobj* sin()  const= 0;
        virtual CRobj* cos() const = 0;

        virtual void simplify() = 0;
        virtual CRobj* copy() const = 0;

        virtual double initialize();
        virtual double valueof() const;
        virtual bool isnumber() const;

        virtual double shift();
    
    protected:
        std::vector<CRobj*> operands;

        std::vector<double> fastvalues;
        std::vector<bool> isnumbers;
        size_t length;
        bool initialized;

};
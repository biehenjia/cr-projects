#pragma once
#include "crobj.hpp"

class CRprod :public CRobj {
    public: 
        CRprod(size_t l);
        
        CRobj* addto(const CRobj& target) const override;
        CRobj* multo(const CRobj& target) const override;
        CRobj* powto(const CRobj& target) const override ;
        CRobj* rpow(const CRobj& target) const override;
        
        CRobj* add(const CRobj& target) const override;
        
        CRobj* mul(const CRobj& target) const override;
        CRprod* mul(const CRnum& target) const;
        CRprod* mul(const CRprod& target) const;

        //CRtrig* 

        CRobj* pow(const CRobj& target) const override;
        CRprod* pow(const CRnum& target) const;
        CRprod* pow(const CRsum& target) const;

        //todo;
        CRobj* exp() const override;
        CRobj* ln() const override;

        CRobj* sin() const override;
        CRobj* cos() const override;

        void simplify() override;
        void shift() override;

        CRprod* copy() const override;

};
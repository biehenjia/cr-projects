#pragma once
#include "crobj.hpp"


class CRsum : public CRobj {
    public: 
        CRsum(size_t l); 
        CRsum(double x, double h);
        
        CRobj* add(const CRobj& target) const override;
        CRsum* add(const CRsum& target) const;
        CRsum* add(const CRnum& target) const;

        CRobj* mul(const CRobj& target) const override;
        CRsum* mul(const CRnum& target) const;
        CRsum* mul(const CRsum& target) const;

        // handle negative power in the visitor
        CRobj* pow(const CRobj& target) const  override;
        CRsum* pow(const CRnum& target) const;

        //covariant
        CRobj* exp() const override;
        CRobj* ln() const override;

        CRobj* sin() const override;
        CRobj* cos() const override;

        void simplify() override;
        CRsum* copy() const override;
        void shift() override;
        double valueof() const override;

};